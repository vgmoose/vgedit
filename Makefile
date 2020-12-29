BINARY      := vgedit

APP_TITLE	:= vgedit+
APP_AUTHOR 	:= vgmoose

SOURCES		+=	. gui edit
CFLAGS		+= -DUSE_KEYBOARD

include libs/chesto/Makefile
