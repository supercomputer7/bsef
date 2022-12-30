# Define required macros here
SHELL = /bin/sh

OBJS = core/main.c core/arguments.c
CFLAG = -Wall -g
CC = gcc
INCLUDE =
LIBS = -lm

bsef: ${OBJ}
	${CC} ${CFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS} -I./
