#include <string>
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/SDL_FontCache/SDL_FontCache.h"

class TextInputElement : public ListElement
{
public:
	TextInputElement(const char* text);
	void updateText(const char* text);
	void render(Element* parent);

	int selected_x = 0;
	int selected_y = 0;
	int selected_width = 1;
	int selected_height = 1;

  int COLS = 100;
  int lineSpacing = 2;

  int fontHeight;
  int fontWidth;

	bool insertMode = false;

  std::string* text;
	FC_Font* font = NULL;
};