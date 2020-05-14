#include "TextInputElement.hpp"
#include "MainDisplay.hpp"

TextInputElement::TextInputElement(const char* text)
{
  int size = 20;

  renderer = (RootDisplay::mainDisplay)->renderer;

  font = FC_CreateFont();  
  FC_LoadFont(font, renderer, RAMFS "./res/mono.ttf", size, FC_MakeColor(0,0,0,255), TTF_STYLE_NORMAL);  

  fontHeight = FC_GetLineHeight(font) + lineSpacing;
  fontWidth = FC_GetWidth(font, "A"); // width of one char

	updateText(text);
}

void TextInputElement::updateText(const char* text)
{
	this->text = new std::string(text);
}

void TextInputElement::render(Element* parent)
{
	if (this->hidden)
		return;

  int xPos = this->x + parent->x + 20;
  int yPos = this->y + parent->y + 70;

  int w = fontWidth, h = fontHeight;

	// // draw the currently selected block
	SDL_Rect cursor_pos = { xPos + selected_x * w, yPos + selected_y * h,
		selected_width * w, selected_height * h };

	if (insertMode) // TODO: use block cursor for overwrite mode too
	{
		// traditional line cursor, between characters
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderDrawLine(renderer, cursor_pos.x + 2, cursor_pos.y, cursor_pos.x + 2, cursor_pos.y + cursor_pos.h);
	}
	else
	{
		// highlight cursor for overview screen
		SDL_SetRenderDrawColor(renderer, 0xDD, 0xDD, 0xDD, 0xFF);
		SDL_RenderFillRect(renderer, &cursor_pos);
	}

  // go through and divvy up the text in each line
  // by visiting COLS characters at a time
  int curPos = 0;
  int linePos = 0;
  int len = text->size();

  // for the entire string
  while (curPos < len)
  {
    int lpos = curPos;
    int bonusWidth = 0; // add to this in cases like tab, where "one" character takes 4 spaces 
    while ((curPos + bonusWidth) - lpos < COLS)
    {
      char curChar = text->at(curPos);
      if (curChar == '\t')
        bonusWidth += 3;

      if (curChar == '\n')
        break;

      curPos ++;
    }
    FC_Draw(font, renderer, xPos, yPos + linePos * fontHeight, text->substr(lpos, curPos - lpos + 1).c_str());
    curPos ++;
    linePos ++;
  }

}