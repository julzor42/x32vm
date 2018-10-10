#ifndef _GUI_H_
#define _GUI_H_

#ifdef USE_GUI
#include <SDL.h>
#endif

#include "device.h"

typedef struct gui_res_s
{
#ifdef USE_GUI
  SDL_Surface *pSurf;
#endif
} gui_res_t;

typedef struct gui_led_s
{
  uint8_t w;
  uint8_t h;
  gui_res_t *on;
  gui_res_t *off;
  gui_res_t *unk;
} gui_led_t;

void gui_init(device_t* dev);
void gui_free();
void gui_refresh();

void gui_res_load(gui_res_t *res, const char *s);
void gui_res_free(gui_res_t *res);
void gui_res_draw(gui_res_t *res, int x, int y);

gui_res_t* gui_led_getres(gui_led_t* led, uint8_t state);
void gui_led_draw(gui_led_t* led, int x, int y, uint8_t state);
void gui_led_map(gui_led_t* led, int x, int y, uint32_t bits, int sz);

void gui_leds(int x, int y, uint32_t bits, int sz);

void gui_begin();
void gui_end();

void gui_cpu_refresh(device_t* dev);

#endif /* _GUI_H_ */
