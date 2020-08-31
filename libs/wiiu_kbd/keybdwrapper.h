#ifndef _KEYBDWRAPPER_H
#define _KEYBDWRAPPER_H

#include <SDL2/SDL.h>

class KBWrapper {
    public:
        KBWrapper(bool initKBD, bool clearKBD);
        ~KBWrapper();
        void init_kbd();
        void shutdown_kbd();
        void clearKeys();
        const Uint8* getKeyboardState();
};

#endif