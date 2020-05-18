#include "TextInputElement.hpp"
#include "MainDisplay.hpp"

TextInputElement::TextInputElement(Editor* editor)
{
  int size = 20;

  renderer = (RootDisplay::mainDisplay)->renderer;

  font = FC_CreateFont();  
  FC_LoadFont(font, renderer, RAMFS "./res/mono.ttf", size, FC_MakeColor(0,0,0,255), TTF_STYLE_NORMAL);  

  fontHeight = FC_GetLineHeight(font) + lineSpacing;
  fontWidth = FC_GetWidth(font, "A"); // width of one char

	this->editor = editor;
}

bool TextInputElement::process(InputEvents* event)
{
  bool ret = ListElement::process(event);

  ret |= (selectedXPos != lastSelectedXPos || selectedYPos != lastSelectedYPos);

  lastSelectedXPos = selectedXPos;
  lastSelectedYPos = selectedYPos;

  return ret;
}

void TextInputElement::render(Element* parent)
{
	if (this->hidden)
		return;

  std::string* text = editor->text;

  int xPos = this->x + parent->x + 20;
  int yPos = this->y + parent->y + 70;

  int w = fontWidth, h = fontHeight;

	// // draw the currently selected block
	SDL_Rect cursor_pos = { selectedXPos, selectedYPos, selectedWidth * w, 1 * h };

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
  int lineYPos = 0;

  int prev_lpos = 0;
  bool isOnNextLine = false;

  // below loop will go through the text string from the front until it would go offscreen
  // and determine lines/spacing based on the content of the text (newlines and wrapping)

  // this means that the further down in the file you scroll, the more text we have to
  // traverse to determine where you on (since it changes with spacing/linebreaks)

  // TODO: keep track of the "last on screen" position in the text and more intelligently
  //       set curPos to start formatting from that position (and save time traversing downwards)

  // for the entire string, or when going offscreen
  while (curPos < len && lineYPos < SCREEN_HEIGHT + 10)
  {
    lineYPos = yPos + linePos * fontHeight;

    bool isOnCurrentLine = false; // becomes true when we want to mark our current line length

    int lpos = curPos;
    int bonusWidth = 0; // add to this in cases like tab, where "one" character takes 4 spaces 
    while (curPos < len && (curPos + bonusWidth) - lpos < COLS)
    {
      if (curPos == selectedPos)
      {
        // we found where our selected cursor position is, mark its coordinates
        // kind of a weird render flow, but our text cursor position is
        // dependent on wherever the actual text is going to be drawn
        // so now is a good time to update it and have it be sync'd
        selectedXPos = xPos + ((curPos + bonusWidth) - lpos) * fontWidth;
        selectedYPos = lineYPos;

        isOnCurrentLine = true;
      }

      char curChar = text->at(curPos);
      if (curChar == '\t')
        bonusWidth += 3;

      if (curChar == '\n')
        break;

      curPos ++;
    }

    if (isOnNextLine)
    {
      // we're on the line after the currnet line...
      // we'll note the length of this here and now
      editor->nextLineLength = curPos - lpos;
      isOnNextLine = false;
    }

    if (isOnCurrentLine)
    {
      // we're on the current line, let's set some variables
      // that will help us find top and bottom lines later
      editor->curLinePos = lpos;
      editor->prevLinePos = prev_lpos;
      editor->curLineLength = curPos - lpos;
      isOnNextLine = true;
    }

    prev_lpos = lpos;

    if (lineYPos > -10) // only draw if it would be onscreen
      FC_Draw(font, renderer, xPos, lineYPos, text->substr(lpos, curPos - lpos + 1).c_str());

    curPos ++;
    linePos ++;
  }

}