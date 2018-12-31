#include "../libs/hb-appstore/gui/ListElement.hpp"

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

  bool insertMode = false;

  int letter_width = 10;
  int letter_height = 20;

  SDL_Texture* textSurface = NULL;
  TTF_Font* font = NULL;
};