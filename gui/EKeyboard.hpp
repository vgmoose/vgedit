#include "../libs/chesto/src/Element.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include <SDL2/SDL_image.h>
#pragma once

class EditorView;

#define KEYCODE_COUNT 47

class EKeyboard : public Element
{
public:
	EKeyboard(EditorView* editorView);
	~EKeyboard();
	void render(Element* parent);
	bool process(InputEvents* event);

	// setup field variables
	void updateSize();
	void just_type(const char input);
	bool listenForPhysicalKeys(InputEvents* e);

	EditorView* editorView = NULL;
	std::function<void(char)> typeAction = NULL;

	// information representing a default qwerty EKeyboard, lower and upper
	// alongside default EKeyboards will also be: tab, caps lock, return, delete, and shifts
	const char* lower_keys = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./";
	const char* upper_keys = "~!@#$\%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
	int breaks[4] = { 13, 13, 11, 10 };

	// the rest of the keys will be dynamically drawn by going through hex iterations
	// these EKeyboards will fill the extra space with more characters

	std::vector<std::string*> rows;

	const CST_Keycode usbKeys[KEYCODE_COUNT] = {
		SDLK_BACKQUOTE, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_0, SDLK_MINUS, SDLK_EQUALS,
		 SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i, SDLK_o, SDLK_p, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET, SDLK_BACKSLASH,
		  SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON, SDLK_QUOTE,
		   SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA, SDLK_PERIOD, SDLK_SLASH
	};

	inline int rowCount() {
		return (int)rows.size();
	}

	inline int rowLength(int row) {
		return (int)rows[row]->size() / 2;
	}

	bool shiftOn = false;
	bool capsOn = false;
	int mode = 0; // the mode of which EKeyboard type we're on (first is special and has shift)

	// the currently selected row and index
	int curRow = -1;
	int index = -1;

	// the below variables are stored to be used in processing touch events
	// and rendering the drawings to screen

	// attributes of each key
	int keyWidth = 0;
	int padding = 0;
	int textSize = 0;

	// attributes of delete and backspace keys
	int dPos = 0;
	int dHeight = 0;
	int sPos = 0;
	int enterPos = 0;
	int enterHeight = 0;
	int dWidth = 0;
	int sWidth = 0;
	int enterWidth = 0;

	// positions of key location offset information
	int kXPad = 0;
	int kXOff = 0;
	int yYOff = 0;
	int kYPad = 0;
	int ySpacing = 0;

	bool touchMode = false;

	void space();
	void backspace();
	void type(int y, int x);
	void updateView();
	void generateEKeyboard();
};
