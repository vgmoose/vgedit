BINARY      := vgedit

APP_TITLE	:= vgedit+
APP_AUTHOR 	:= vgmoose

SOURCES		+=	. gui edit
CFLAGS		+= -DUSE_KEYBOARD

SOURCES += $(CHESTO_DIR)/libs/SDL_FontCache
VPATH   += $(CHESTO_DIR)/libs/SDL_FontCache

include libs/chesto/Makefile