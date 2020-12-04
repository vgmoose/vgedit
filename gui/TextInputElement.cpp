#include "MainDisplay.hpp"
#include "EditorView.hpp"
#include <iomanip> 

TextInputElement::TextInputElement(Editor* editor)
{
  int size = 20;

  renderer = (RootDisplay::mainDisplay)->renderer;

  auto fontPath = RAMFS "./res/mono.ttf";

  font = FC_CreateFont();  
  FC_LoadFont(font, renderer, fontPath, size, FC_MakeColor(0,0,0,255), TTF_STYLE_NORMAL); 

  lineFont = FC_CreateFont();  
  FC_LoadFont(lineFont, renderer, fontPath, size - 5, FC_MakeColor(0x40,0x40,0x40,255), TTF_STYLE_NORMAL);

  fontHeight = FC_GetLineHeight(font) + lineSpacing;
  fontWidth = FC_GetWidth(font, "A"); // width of one char

	this->editor = editor;
}

bool TextInputElement::process(InputEvents* event)
{
  bool ret = false;

  auto editorView = ((MainDisplay*)RootDisplay::mainDisplay)->editorView;
  bool isImmersive = editorView && editorView->keyboard && editorView->keyboard->immersiveMode;
  bool blockKeyboardDraws = insertMode && !isImmersive;

  if (hasRendered && event->isTouchUp() && elasticCounter == 0 && (!blockKeyboardDraws || event->yPos < 300)) {
    lastTouchX = event->xPos;
    lastTouchY = event->yPos;
    ret |= true;
  }

  ret |= ListElement::process(event);

  ret |= (selectedXPos != lastSelectedXPos || selectedYPos != lastSelectedYPos);

  lastSelectedXPos = selectedXPos;
  lastSelectedYPos = selectedYPos;

  return ret;
}

void TextInputElement::drawLineNo(int actualLineNo, int lineXPos, int actualLineYPos)
{
  std::stringstream stream;
  stream << std::setw(editor->lineNoPlaces) << std::setfill('0') << actualLineNo;
  auto res = stream.str();
  FC_Draw(lineFont, renderer, lineXPos, actualLineYPos + 1, res.c_str());
}

void TextInputElement::setStatus(const char* status)
{
  auto editorView = ((MainDisplay*)RootDisplay::mainDisplay)->editorView;
  auto toolbar = editorView->toolbar;
  toolbar->stats->setText(status);
  toolbar->stats->update();
  this->customStatus = true;
}

void TextInputElement::render(Element* parent)
{
	if (this->hidden)
		return;

  hasRendered = true;

  auto editorView = ((MainDisplay*)RootDisplay::mainDisplay)->editorView;
  editorView->reset_bounds();

  std::string* text = editor->text;

  int len = text->length();

  int lineNoPlaces = editor->lineNoPlaces;

  int lineXPos = this->x + parent->x + 20;
  int xPos = lineXPos + (lineNoPlaces + 1) * fontWidth;
  int yPos = this->y + parent->y + 70;

  int w = fontWidth, h = fontHeight;

  bonusWidthInSelection = 0;

  // go through and divvy up the text in each line
  // by visiting COLS characters at a time
  int curPos = 0;
  int linePos = 0;
  int actualLineYPos = 0; // true start of line's y coordinate (for line nums)

  int currentLineNo = 0; // "editor" lines (wrap at screen width)
  int actualLineNo = 0; // real lines, defined by \n
  int prevActualLineNo = -1; // so we can keep track of when to draw the line no

  int lineYPos = 0;

  int prev_lpos = 0;
  bool isOnNextLine = false;
  bool hasWrapped = false; // becomes true after we wrap around
  bool nextTimeCountLine = false; // becomes true when \n is at the end of the width of the line

  // below loop will go through the text string from the front until it would go offscreen
  // and determine lines/spacing based on the content of the text (newlines and wrapping)

  // this means that the further down in the file you scroll, the more text we have to
  // traverse to determine where you on (since it changes with spacing/linebreaks)

  // TODO: keep track of the "last on screen" position in the text and more intelligently
  //       set curPos to start formatting from that position (and save time traversing downwards)

  // for the entire string, and: (when going offscreen, or haven't seen cursor yet)
  while (curPos < len && (lineYPos < SCREEN_HEIGHT + 10 || curPos <= selectedPos))
  {
    lineYPos = yPos + linePos * fontHeight;

    bool isOnCurrentLine = false; // becomes true when we want to mark our current line length

    int lpos = curPos;
    int bonusWidth = 0; // add to this in cases like tab, where "one" character takes 4 spaces 

    if (!hasWrapped || nextTimeCountLine) {
      // update the position of the line number only if we didn't wrap
      actualLineYPos = lineYPos;
    }
    nextTimeCountLine = false;

    while (curPos < len && (curPos + bonusWidth) - lpos <= COLS)
    {
      // if we have a last touch, and we're nearby it, set our current selection position
      // and reset that last touch position
      if (lastTouchX > 0 && lastTouchY > 0) {
        if (abs(lastTouchY - lineYPos) < 15
            && (abs(lastTouchX - (xPos + ((curPos + bonusWidth) - lpos) * fontWidth)) < 10)) {
          selectedPos = curPos;
          lastTouchX = lastTouchY = -1;
        }
      }

      bool isLastCharOnLine = (curPos + bonusWidth) - lpos == COLS;

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
      if (curChar == '\t') {
        bonusWidth += 3;
        if (curPos >= selectedPos && curPos < selectedPos + selectedWidth)
          bonusWidthInSelection += 3;
      }

      if (curChar == '\n') {
        actualLineNo ++;
        if (isLastCharOnLine)
          nextTimeCountLine = true; // special case for \n on same line, force draw next time
        hasWrapped = false; // we're going to a new, real line next
        break;
      }

      // special case of end of the line not incrementing curPos
      if (isLastCharOnLine)
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
      currentLineNo = linePos;
      isOnNextLine = true;

      // we can update the cursor right now
      CST_Rect cursor_pos = { selectedXPos, selectedYPos - 3, (selectedWidth + bonusWidthInSelection) * w, 1 * h };

      if (insertMode) // TODO: use block cursor for overwrite mode too
      {
        // traditional line cursor, between characters
        // CST_SetDrawColor(renderer, { 0x00, 0x00, 0x00, 0xFF });
        CST_SetDrawColor(renderer, { 0x3e, 0xcc, 0xb1, 0xFF });
        for (int i=-1; i<2; i++) {
          CST_DrawLine(renderer, cursor_pos.x + i, cursor_pos.y - 1, cursor_pos.x + i, cursor_pos.y + cursor_pos.h + 1);
        }
      }
      else
      {
        // highlight cursor for overview screen
        // CST_SetDrawColor(renderer, { 0xDD, 0xDD, 0xDD, 0xFF });
        CST_SetDrawColor(renderer, { 0xd6, 0xf5, 0xef, 0xFF });
        CST_FillRect(renderer, &cursor_pos);
        // CST_SetDrawColor(renderer, { 0x90, 0x90, 0x90, 0xFF });
        CST_SetDrawColor(renderer, { 0x3e, 0xcc, 0xb1, 0xFF });
        for (int i=0; i<2; i++) {
          cursor_pos.x -= 1;
          cursor_pos.y -= 1;
          cursor_pos.w += 2;
          cursor_pos.h += 2;
          CST_DrawRect(renderer, &cursor_pos);
        }
      }
    }

    prev_lpos = lpos;

    if (actualLineYPos > -10 && actualLineNo != 0 && actualLineNo != prevActualLineNo) {
      // line numbers for this row, only over start of the "actual" (pre-wrapped) line
      drawLineNo(actualLineNo, lineXPos, actualLineYPos);
      prevActualLineNo = actualLineNo;
    }

    if (lineYPos > -10) // only draw if it would be onscreen
    {
      // draw this part of current line
      auto lineText = text->substr(lpos, curPos - lpos + 1);
      FC_Draw(font, renderer, xPos, lineYPos, lineText.c_str());
      hasWrapped = curPos - lpos + bonusWidth == COLS; // we wrapped if our line width (and tabs) is equal to max
    }
  
    curPos ++;
    linePos ++;
  }

  // if we had more of the file to go through (we stopped rendering early)
  // draw the final line numbers
  if (curPos != len)
    drawLineNo(actualLineNo + 1, lineXPos, actualLineYPos);

  editorView->reset_bounds();

  // counts for the status bar
  auto toolbar = editorView->toolbar;
  if (toolbar->stats != NULL && !customStatus) {
    std::ostringstream status;
    status << len << " characters" << " (";
    status << currentLineNo << ", " << (selectedPos - editor->curLinePos);
    status << ", " << selectedPos << ")";
    toolbar->statusText = status.str();
    toolbar->stats->setText(toolbar->statusText.c_str());
    toolbar->stats->update();
  }

  customStatus = false;  
}