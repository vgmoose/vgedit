#include "TextInputElement.hpp"
#include "MainDisplay.hpp"

TextInputElement::TextInputElement(const char* text)
{
  int size = 20;
  font = TTF_OpenFont(ROMFS "./res/mono.ttf", size);

  // get a size of just one "square" letter so we can use it for cursor spacing later
  updateText("A");
  letter_width = this->width;
  letter_height = this->height;

  // initialize and draw the text surface
  updateText(text);
}

void TextInputElement::updateText(const char* text)
{
  // free old surface if existing first
  if (this->textSurface)
    SDL_DestroyTexture(this->textSurface);

	// font didn't load, don't render anything
	if (!font)
		return;

  SDL_Color color = { 0x00, 0x00, 0x00 };
	SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, text, color, 1240);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surf);
	SDL_FreeSurface(surf);

  textSurface = texture;

  int w, h;
	SDL_QueryTexture(this->textSurface, NULL, NULL, &w, &h);
	this->width = w;
	this->height = h;
}

void TextInputElement::render(Element* parent)
{
  if (this->hidden)
    return;

  // TODO: don't copy paste code from TextElement? (inherit?)
  //       might need to be done after pulling hb-appstore gui files into own lib
  SDL_Rect textLocation;
	textLocation.x = this->x + parent->x;
	textLocation.y = this->y + parent->y;
	textLocation.w = this->width;
	textLocation.h = this->height;
 
  int border = 2;

  SDL_Rect dimens = { textLocation.x - border, textLocation.y - border,
                      textLocation.w + border*2, textLocation.h + border*2 };

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderFillRect(parent->renderer, &dimens);

  int w = letter_width, h = letter_height;

  // draw the currently selected block
  SDL_Rect cursor_pos = { textLocation.x + selected_x * w - 2, textLocation.y + selected_y * h - 2,
                          selected_width * w + 4, selected_height * h + 4 };

  if (insertMode) // TODO: use block cursor for overwrite mode too
  {
    // traditional line cursor, between characters
    SDL_SetRenderDrawColor(parent->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawLine(parent->renderer, cursor_pos.x + 2, cursor_pos.y, cursor_pos.x + 2, cursor_pos.y + cursor_pos.h);
  }
  else
  {
    // highlight cursor for overview screen
    SDL_SetRenderDrawColor(parent->renderer, 0xDD, 0xDD, 0xDD, 0xFF);
    SDL_RenderFillRect(parent->renderer, &cursor_pos);
  }

	SDL_RenderCopy(MainDisplay::mainRenderer, this->textSurface, NULL, &textLocation);
}