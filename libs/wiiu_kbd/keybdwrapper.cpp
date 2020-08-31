#include "keybdwrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _KBDCountry 
{
    KBD_WIIU_DAN                = 0x03,
    KBD_WIIU_FR                 = 0x05,
    KBD_WIIU_GER                = 0x06,
    KBD_WIIU_ITA                = 0x08,
    KBD_WIIU_JAP                = 0x09,
    KBD_WIIU_DUTCH              = 0x0B,
    KBD_WIIU_POR                = 0x0D,
    KBD_WIIU_SPA                = 0x0E,
    KBD_WIIU_UK                 = 0x10,
    KBD_WIIU_US                 = 0x11,
    KBD_WIIU_RUS                = 0x12
} KBDCountry;

typedef enum _KEYState
{
    KBD_WIIU_DOWN                = 0x01,
    KBD_WIIU_REPEAT              = 0x02,
    KBD_WIIU_NULL                = 0x00
} KEYState;

typedef enum _KBDModifier
{   
    KBD_WIIU_NOMOD               = 0x0000,
    KBD_WIIU_CTRL                = 0x0001,
    KBD_WIIU_SHIFT               = 0x0002,
    KBD_WIIU_ALT                 = 0x0004,
    KBD_WIIU_WIN                 = 0x0008,
    KBD_WIIU_MENU                = 0x0010,
    KBD_WIIU_ALTGR               = 0x0020,
    KBD_WIIU_NUM_LOCK            = 0x0100,
    KBD_WIIU_CAPS_LOCK           = 0x0200,
    KBD_WIIU_SCROLL_LOCK         = 0x0400,
    KBD_WIIU_ALLOW_SHIFT         = 0x0800,
} KBDModifier;

typedef struct _KBDKeyEvent
{			 
    unsigned char  channel;
    unsigned char  scancode;    // scancode
    KEYState		state;            // when held, value is 0x03, which is KBD_DOWN & KBD_REPEAT
    KBDModifier		modifier;       // modifier state
    unsigned short  UTF16;	    // unicode, if any
} KBDKeyEvent;

char KBDSetup(void *connection_callback, void *disconnection_callback, void *key_callback);
char KBDTeardown();
unsigned short KBDTranslateHidCode(unsigned char scancode, KBDModifier modifier, KBDCountry country);

static Uint8 kbdstate[SDL_NUM_SCANCODES];

void kb_connection_callback(unsigned char *ch) {
    //__os_snprintf(buf, 255, "kb connected at channel %d", *ch);
}

void kb_disconnection_callback(unsigned char *ch) {
    //__os_snprintf(buf, 255, "kb disconnected at channel %d", *ch);
}

SDL_Keycode wiiuCodeToSDL(SDL_Scancode scancode)
{
	const int LETTER_OFF = 0x04;
	const int NUM_OFF = 0x1e;
	const int OTHER_OFF = 0x27;

	// not a code we're interested in, pass through
	if (scancode < LETTER_OFF || scancode > 0x39)
		return SDL_SCANCODE_TO_KEYCODE(scancode);

	if (scancode < NUM_OFF) 	return scancode - LETTER_OFF + SDLK_a;
	if (scancode < OTHER_OFF)	return scancode - NUM_OFF + SDLK_1;

	const  SDL_Keycode others[19] = { SDLK_0, SDLK_RETURN, SDLK_ESCAPE, SDLK_BACKSPACE,
      SDLK_TAB, SDLK_SPACE, SDLK_MINUS, SDLK_EQUALS, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET,
	  SDLK_BACKSLASH, SDLK_HASH, SDLK_SEMICOLON, SDLK_QUOTE, SDLK_BACKQUOTE, SDLK_COMMA,
	  SDLK_PERIOD, SDLK_SLASH, SDLK_CAPSLOCK };

	return others[scancode - OTHER_OFF];
}

void kb_key_callback(KBDKeyEvent *key) {
  if (key->state == 1) {
    SDL_Scancode scancode = (SDL_Scancode)key->scancode;
    if ((scancode >= 0) && (scancode < SDL_NUM_SCANCODES))
        kbdstate[scancode] = 0x01;
    SDL_Event sdlevent = {};
    sdlevent.type = SDL_KEYDOWN;
    SDL_Keycode keyCode;
    keyCode = wiiuCodeToSDL(scancode);
    if ((scancode != SDL_SCANCODE_LALT) || (scancode != SDL_SCANCODE_RALT) || (scancode != SDL_SCANCODE_LSHIFT)
        || (scancode != SDL_SCANCODE_RSHIFT) || (scancode != SDL_SCANCODE_LCTRL) || (scancode != SDL_SCANCODE_RCTRL)) {
        if (kbdstate[SDL_SCANCODE_LALT] == 0x01)
            sdlevent.key.keysym.mod = KMOD_LALT;
        if (kbdstate[SDL_SCANCODE_RALT] == 0x01)
            sdlevent.key.keysym.mod = KMOD_RALT;
        if (kbdstate[SDL_SCANCODE_LSHIFT] == 0x01)
            sdlevent.key.keysym.mod = KMOD_LSHIFT;
        if (kbdstate[SDL_SCANCODE_RSHIFT] == 0x01)
            sdlevent.key.keysym.mod = KMOD_RSHIFT;
        if (kbdstate[SDL_SCANCODE_LCTRL] == 0x01)
            sdlevent.key.keysym.mod = KMOD_LCTRL;
        if (kbdstate[SDL_SCANCODE_RCTRL] == 0x01)
            sdlevent.key.keysym.mod = KMOD_RCTRL;
    }
    sdlevent.key.keysym.sym = keyCode;
    SDL_PushEvent(&sdlevent);
  }
  else if (key->state == 0) {
    SDL_Scancode scancode = (SDL_Scancode)key->scancode;
    if ((scancode >= 0) && (scancode < SDL_NUM_SCANCODES))
        kbdstate[scancode] = 0x00;
    SDL_Event sdlevent = {};
    sdlevent.type = SDL_KEYUP;
    SDL_Keycode keyCode;
    keyCode = wiiuCodeToSDL(scancode);
    sdlevent.key.keysym.sym = keyCode;
    SDL_PushEvent(&sdlevent);
  }
}

void clearKeyStates() {
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        kbdstate[i] = 0x00;
    }
}

const Uint8* returnKeyboardState() {
    const Uint8* tmp = kbdstate;
    return tmp;
}

void setup_keyboard_callbacks() {
  char kb_ret_value = KBDSetup((void *)kb_connection_callback, (void *)kb_disconnection_callback, (void *)kb_key_callback);
}

void teardown_keyboard() {
    KBDTeardown();
}

#ifdef __cplusplus
}
#endif

KBWrapper::KBWrapper(bool initKBD, bool clearKBD)
{
    init_kbd();
    clearKeys();
}

KBWrapper::~KBWrapper()
{
    shutdown_kbd();
}

void KBWrapper::init_kbd()
{
  setup_keyboard_callbacks();
}

void KBWrapper::shutdown_kbd()
{
  teardown_keyboard();
}

void KBWrapper::clearKeys()
{
    clearKeyStates();
}

const Uint8* KBWrapper::getKeyboardState()
{
    const Uint8* tmp = kbdstate;
    return tmp;
}