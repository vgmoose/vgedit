#include <string>
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/SDL_FontCache/SDL_FontCache.h"
#include "../edit/Editor.hpp"

class TextInputElement : public ListElement
{
public:
	TextInputElement(Editor* editor);
	void updateText(const char* text);

	bool process(InputEvents* event);
	void render(Element* parent);

	int selectedPos = 0;
	int selectedWidth = 1;

	int selectedXPos = 0;
	int selectedYPos = 0;

	int lastSelectedXPos = -1;
	int lastSelectedYPos = -1;

  int COLS = 100;
  int lineSpacing = 2;

  int fontHeight;
  int fontWidth;

	bool insertMode = false;

	Editor* editor;
	FC_Font* font = NULL;
};