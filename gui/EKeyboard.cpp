#include "EKeyboard.hpp"
#include "EditorView.hpp"

EKeyboard::EKeyboard(EditorView* editorView)
{
	this->x = 30;
	this->y = 300;

  this->editorView = editorView;

	curRow = index = -1;

	// position the EKeyboard based on this x and y
	updateSize();
}

void EKeyboard::render(Element* parent)
{
	if (hidden)
		return;

	SDL_Rect dimens = { this->x, this->y, this->width + 305, this->height + 140};

	this->window = parent->window;
	this->renderer = parent->renderer;

	SDL_SetRenderDrawColor(parent->renderer, 0xf9, 0xf9, 0xf9, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);

	for (int y = 0; y < rows.size(); y++)
		for (int x = 0; x < rows[y]->size()/2+1; x++)
		{
			SDL_Rect dimens2 = { this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth };
			SDL_SetRenderDrawColor(parent->renderer, 0xf4, 0xf4, 0xf4, 0xff);
			SDL_RenderFillRect(parent->renderer, &dimens2);
		}

	// if there's a highlighted piece set, color it in
	if (curRow >= 0 || index >= 0)
	{
		SDL_Rect dimens2 = { this->x + kXPad + index * kXOff + curRow * yYOff, this->y + kYPad + curRow * ySpacing, keyWidth, keyWidth };

		// if we're on DEL or SPACE, expand the dimens width of the highllighted button
		if (curRow == 2 && index < 0)
		{
			SDL_Rect dimens3 = { this->x + dPos, this->y + dHeight, dWidth, textSize };
			dimens2 = dimens3;
		}
		// if (curRow == 2 && index > 6)
		// {
		// 	SDL_Rect dimens4 = { this->x + sPos, this->y + dHeight, sWidth, textSize };
		// 	dimens2 = dimens4;
		// }

		// draw the currently selected tile if these index things are set
		if (touchMode)
		{
			SDL_SetRenderDrawColor(parent->renderer, 0xad, 0xd8, 0xe6, 0x90); // TODO: matches the DEEP_HIGHLIGHT color
			SDL_RenderFillRect(parent->renderer, &dimens2);
		}
		else
		{
      SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xff, 0xff, 0xff); // TODO: matches the DEEP_HIGHLIGHT color
			SDL_RenderFillRect(parent->renderer, &dimens2);

			// border
			for (int z = 4; z >= 0; z--)
			{
				SDL_SetRenderDrawColor(parent->renderer, 0x66 - z * 10, 0x7c + z * 20, 0x89 + z * 10, 0xFF);
				dimens2.x--;
				dimens2.y--;
				dimens2.w += 2;
				dimens2.h += 2;
				SDL_RenderDrawRect(parent->renderer, &dimens2);
			}
		}
	}

	//   SDL_Rect dimens3 = {this->x+dPos, this->y + dHeight, dWidth, textSize};
	//   SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
	//   SDL_RenderFillRect(parent->renderer, &dimens3);
	//
	//   SDL_Rect dimens4 = {this->x+sPos, this->y + dHeight, sWidth, textSize};
	//   SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
	//   SDL_RenderFillRect(parent->renderer, &dimens4);

	super::render(this);
}

bool EKeyboard::process(InputEvents* event)
{
	// don't do anything if we're hidden, or there's a sidebar and it's active
	if (hidden)
		return false;

	if (event->isTouchDown())
	{
		curRow = index = -1;
		touchMode = true;
	}

	if (event->isKeyDown())
		touchMode = false;

	bool ret = false;

	if (!touchMode)
	{
		if (curRow < 0 && index < 0)
		{
			// switched into EKeyboard, set to 0 and return
			curRow = 1;
      index = 0;
			return true;
		}

		if (event->isKeyDown())
		{
			curRow += (event->held(DOWN_BUTTON) - event->held(UP_BUTTON));
			index += (event->held(RIGHT_BUTTON) - event->held(LEFT_BUTTON));

			if (curRow < 0) curRow = 0;
			if (index < -1 * (curRow >= 2)) index = -1 * (curRow >= 2);
			if (curRow >= rows.size()) curRow = rows.size() - 1;
			if (index >= rows[curRow]->size()/2+1) index = rows[curRow]->size()/2;

			if (event->held(A_BUTTON))
			{
				type(curRow, index);
			}

			updateView();
			return true;
		}

		return false;
	}

  int extWidth = width + 305;

	if (event->isTouchDown() && event->touchIn(this->x, this->y, extWidth, height))
	{
		for (int y = 0; y < rows.size(); y++)
			for (int x = 0; x < rows[y]->size()/2+1; x++)
				if (event->touchIn(this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth))
				{
					ret |= true;
					curRow = y;
					index = x;
				}
		return true;
	}

	if (event->isTouchUp())
	{
		// only proceed if we've been touchdown'd
		// reset current row and info
		curRow = -1;
		index = -1;

		if (event->touchIn(this->x, this->y, extWidth, height))
		{

			for (int y = 0; y < rows.size(); y++)
				for (int x = 0; x < rows[y]->size()/2+1; x++)
					if (event->touchIn(this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth))
					{
						ret |= true;
						type(y, x);
					}

			if (event->touchIn(this->x + dPos, this->y + dHeight, dWidth, textSize))
			{
				ret |= true;
				backspace();
			}

			if (event->touchIn(this->x + sPos, this->y + dHeight, sWidth, textSize))
			{
				ret |= true;
				space();
			}

			if (ret)
				updateView();

			return ret;
		}

		return false;
	}

	return false;
}

void EKeyboard::updateSize()
{
	this->elements.clear();

	this->width = 900;
	this->height = (304 / 900.0) * width;

	// set up lots of scaling variables based on the width/height

	this->keyWidth = (int)(0.08 * width);
	this->padding = keyWidth / 2.0;

	// these field variables are for displaying the QWERTY keys (touching and displaying)
	kXPad = (int)((23 / 400.0) * width);
	kXOff = (int)((36.5 / 400.0) * width);
	yYOff = (int)((22 / 400.0) * width);
	kYPad = (int)((17 / 135.0) * height);
	ySpacing = (int)((33 / 400.0) * width);

	// these local variables position only the text, and has nothing to do with the
	// touch. They should likely be based on the above field variables so those
	// can change quickly
	int kXPad = (int)((30 / 400.0) * width);
	int kXOff = (int)((22 / 400.0) * width);
	int kYPad = (int)((14 / 400.0) * width);
	int kYOff = (int)((33 / 400.0) * width);

	this->textSize = 0.9375 * keyWidth;

	// delete and space key dimensions
	dPos = (int)((13 / 400.0) * width);
	dHeight = (int)((85 / 135.0) * height);
	sPos = (int)((326 / 400.0) * width);

	dWidth = (int)(1.4125 * textSize);
	sWidth = (int)(1.91875 * textSize);

	// set up the keys vector based on the current EKeyboard selection
	generateEKeyboard();

	SDL_Color gray = { 0x52, 0x52, 0x52, 0xff };

	// go through and draw each of the three rows at the right position
	for (int x = 0; x < rows.size(); x++)
	{
		TextElement* rowText = new TextElement(rows[x]->c_str(), textSize, &gray, true);
		rowText->position(kXPad + x * kXOff, kYPad + x * kYOff);
		this->elements.push_back(rowText);
	}

	// these are local variables, similar to how the other ones are global events
	int dPos2 = (int)((20 / 400.0) * width);
	int dHeight2 = (int)((90 / 135.0) * height);
	int sPos2 = (int)((330 / 400.0) * width);

	int textSize2 = (int)((16 / 400.0) * width);

	// TextElement* delButton = new TextElement("DEL", textSize2, &gray, false);
	// delButton->position(dPos2, dHeight2);
	// this->elements.push_back(delButton);

	// TextElement* spaceButton = new TextElement("SPACE", textSize2, &gray, false);
	// spaceButton->position(sPos2, dHeight2);
	// this->elements.push_back(spaceButton);
}

void EKeyboard::type(int y, int x)
{
	const char input = (*(rows[y]))[x * 2];
  auto line = editorView->mainTextField->selected_y;
  auto pos = editorView->mainTextField->selected_x;
  editorView->editor->type(line, pos, input);

  editorView->mainTextField->selected_x++;
  editorView->syncText();
}

void EKeyboard::backspace()
{
	// if (!myText->empty())
	// 	myText->pop_back();
}

void EKeyboard::space()
{
	// myText->append(" ");
}

void EKeyboard::updateView()
{

}

void EKeyboard::generateEKeyboard()
{
  int count = 0;
  string keys;

  if (mode == 0)
    keys = string((shiftOn || capsOn) ? upper_keys : lower_keys);
  else
  {
    // depending on the mode, grab a bunch of characters from unicode starting from
    // upside exclamation mark (U+00A1) and onward https://unicode-table.com/en/
    // TODO: don't hardcode amount here, or hardcode a better one
    int offset = 47*(mode - 1) + 0x00a1;
    char chars[47 + (mode == 2)];
    for (int x=0; x<47+(mode==2); x++)
    {
      chars[x] = offset + x;
    }

    keys = string(chars);
  }

  breaks[3] += (mode==2); // one more key for bottom row of mode 2

  for (int& end : breaks)
  {
    string* row = new string(keys.substr(count, end));
    for (int x=1; x<row->size(); x+=2)
    {
      row->insert(row->begin() + x, ' ');
    }

    rows.push_back(row);
    count += end;
  }
}

EKeyboard::~EKeyboard()
{
}
