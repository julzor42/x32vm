NAME=pvm
SRC=$(wildcard src/*.c)
OBJ=$(patsubst %.c,%.o,$(SRC))
CFLAGS=`sdl2-config --cflags` -O3 -DUSE_GUI
LDFLAGS=`sdl2-config --libs` -O3

CC=gcc

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
