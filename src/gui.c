#include <stdio.h>
#include <signal.h>

#include "gui.h"
#include "sfr.h"

/////////////////////////////////////////////////////////////////////////////
//
// SDL objects
//
SDL_Window *wnd = 0;
SDL_Surface *surf = 0;
int sfr_line;

/////////////////////////////////////////////////////////////////////////////
//
// Resources
//
gui_res_t r_background;
gui_res_t r_ledOn;
gui_res_t r_ledOff;
gui_res_t r_ledUnk;

/////////////////////////////////////////////////////////////////////////////
//
// Renderer
//
void device_renderer(device_t *dev, uint32_t event, void *p)
{
  switch (event)
  {
  case DEVICE_EVENT_RESET:
  case DEVICE_EVENT_CYCLE:
    gui_cpu_refresh(dev);
    break;

  case DEVICE_EVENT_SFRWRITE:
    dev->refresh = 1;
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// GUI objects
//
gui_led_t led;

void gui_init(device_t *dev)
{
  struct sigaction action;
  sigaction(SIGINT, NULL, &action);
  SDL_Init(SDL_INIT_VIDEO);
  sigaction(SIGINT, &action, NULL);

  wnd = SDL_CreateWindow("PIC32 Virtual Machine",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         640, 480,
                         SDL_WINDOW_SHOWN);

  surf = SDL_GetWindowSurface(wnd);

  gui_res_load(&r_background, "./resources/background.bmp");
  gui_res_load(&r_ledOn, "./resources/led-1.bmp");
  gui_res_load(&r_ledOff, "./resources/led-0.bmp");
  gui_res_load(&r_ledUnk, "./resources/led-unk.bmp");

  led.on = &r_ledOn;
  led.off = &r_ledOff;
  led.unk = &r_ledUnk;
  led.w = 16;
  led.h = 16;

  device_register_event(dev, device_renderer);
}

void gui_free()
{
  if (wnd == NULL)
    return;

  gui_res_free(&r_background);

  SDL_DestroyWindow(wnd);
  SDL_Quit();

  wnd = NULL;
}

void gui_res_load(gui_res_t *res, const char *s)
{
  res->pSurf = SDL_LoadBMP(s);
}

void gui_res_free(gui_res_t *res)
{
  SDL_FreeSurface(res->pSurf);
  res->pSurf = NULL;
}

void gui_res_draw(gui_res_t *res, int x, int y)
{
  SDL_Rect rct;
  rct.x = x;
  rct.y = y;

  SDL_BlitSurface(res->pSurf, NULL, surf, &rct);
}

gui_res_t *gui_led_getres(gui_led_t *led, uint8_t state)
{
  switch (state)
  {
  case 0:
    return led->off;
  case 1:
    return led->on;
  case 2:
    return led->unk;
  }

  return 0;
}

void gui_led_draw(gui_led_t *led, int x, int y, uint8_t state)
{
  gui_res_draw(gui_led_getres(led, state), x, y);
}

void gui_led_map(gui_led_t *led, int x, int y, uint32_t bits, int sz)
{
  int i;
  int b;
  int dec;

  for (i = 0; i < sz; i++)
  {
    b = bits & (1 << ((sz - 1) - i));

    if ((i + 1) % 8 == 0)
      dec = 8;
    else if ((i + 1) % 4 == 0)
      dec = 6;
    else
      dec = 2;

    gui_led_draw(led, x, y, b != 0);

    x += (led->w + dec);
  }
}

void gui_leds(int x, int y, uint32_t bits, int sz)
{
  gui_led_map(&led, x, y, bits, sz);
}

void gui_begin()
{
  sfr_line = 0;
  SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0, 0, 0x40));
  //gui_res_draw(&r_background, 0, 0);
}

void gui_end()
{
  SDL_UpdateWindowSurface(wnd);
}

void gui_sfr(device_t *dev, uint32_t sfr)
{
  gui_leds(12, 8 + sfr_line * 24, *sfr_ptr(dev, sfr), 32);
  sfr_line++;
}

void gui_cpu_refresh(device_t *dev)
{
  if (!dev->refresh)
    return;

  gui_begin();

  gui_sfr(dev, SFR_TRISA);
  gui_sfr(dev, SFR_PORTA);
  gui_sfr(dev, SFR_LATA);
  gui_sfr(dev, SFR_ODCA);

  gui_end();

  dev->refresh = 0;
}