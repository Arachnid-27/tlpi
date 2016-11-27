LIB=../libtlpi.a
HDR=../include

CC=gcc

CFLAGS=-std=c99 -D_XOPEN_SOURCE=600 -D_DEFAULT_SOURCE -g -I $(HDR) $(LIB) -g
