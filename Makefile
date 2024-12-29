BINARY      := vgedit

APP_TITLE	:= vgedit+
APP_AUTHOR 	:= vgmoose
APP_VERSION := 2.2

SOURCES		+=	. gui edit
CFLAGS		+= -DUSE_KEYBOARD 
# -D_3DS_MOCK

ifeq (wiiu,$(MAKECMDGOALS))
SOURCES 	+= $(CHESTO_DIR)/libs/wiiu_kbd libs/librpxloader/source
VPATH 		+= $(CHESTO_DIR)/libs/wiiu_kbd
INCLUDES  	+= ../libs/librpxloader/include
endif

include libs/chesto/Makefile
