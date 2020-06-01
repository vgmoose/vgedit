#include "EKeyboard.hpp"
#include "EditorView.hpp"

EKeyboard::EKeyboard(EditorView* editorView)
{
	this->x = 30;
	this->y = 300;

	this->editorView = editorView;

	curRow = index = -1;
	this->isAbsolute = true;

	// position the EKeyboard based on this x and y
	updateSize();
}

void EKeyboard::render(Element* parent)
{
	if (hidden)
		return;

	CST_Rect dimens = { this->x, this->y, this->width + 305, this->height + 140 };

	this->window = parent->window;
	auto renderer = getRenderer();

	CST_SetDrawColor(renderer, { 0xf9, 0xf9, 0xf9, 0xFF });
	CST_FillRect(renderer, &dimens);

	for (int y = 0; y < rowCount(); y++)
		for (int x = 0; x < rowLength(y) + 1; x++)
		{
			CST_Rect dimens2 = { this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth };
			CST_SetDrawColor(renderer, { 0xf4, 0xf4, 0xf4, 0xff });
			CST_FillRect(renderer, &dimens2);
		}

	CST_Rect dimensSpace = { this->x + sPos, this->y + dHeight, sWidth, textSize };
	CST_Rect dimensEnter = { this->x + enterPos, this->y + enterHeight, enterWidth, (int)(1.5 * textSize) };
	CST_Rect dimensTab = { this->x + dPos, this->y + enterHeight, enterWidth, (int)(1.5 * textSize) };

	// if there's a highlighted piece set, color it in
	if (curRow >= 0 || index >= 0)
	{
		CST_Rect dimens2 = { this->x + kXPad + index * kXOff + curRow * yYOff, this->y + kYPad + curRow * ySpacing, keyWidth, keyWidth };

		if (curRow >= rowCount())
		{
			switch (index)
			{
				case 0:
					dimens2 = dimensTab;
					break;
				case 1:
					// if we're on SPACE, expand the dimens width of the highlighted button
					dimens2 = dimensSpace;
					break;
				case 2:
					// if we're on ENTER, expand the dimens width of the highlighted button
					dimens2 = dimensEnter;
					break;
				default:
					break;
			}
		}

		// draw the currently selected tile if these index things are set
		if (touchMode)
		{
			CST_SetDrawColor(renderer, { 0xad, 0xd8, 0xe6, 0x90 }); // TODO: matches the DEEP_HIGHLIGHT color
			CST_FillRect(renderer, &dimens2);
		}
		else
		{
			CST_SetDrawColor(renderer, { 0xff, 0xff, 0xff, 0xff }); // TODO: matches the DEEP_HIGHLIGHT color
			CST_FillRect(renderer, &dimens2);

			// border
			for (int z = 4; z >= 0; z--)
			{
				CST_SetDrawColor(renderer, { 0x66 - z * 10, 0x7c + z * 20, 0x89 + z * 10, 0xFF });
				dimens2.x--;
				dimens2.y--;
				dimens2.w += 2;
				dimens2.h += 2;
				CST_DrawRect(renderer, &dimens2);
			}
		}
	}

	CST_SetDrawColor(renderer, { 0xf4, 0xf4, 0xf4, 0xff });
	CST_FillRect(renderer, &dimensSpace);

	if (typeAction == NULL) {
		CST_FillRect(renderer, &dimensEnter);
		CST_FillRect(renderer, &dimensTab);
	}

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
			int lastRow = curRow;
			curRow += (event->held(DOWN_BUTTON) - event->held(UP_BUTTON));
			index += (event->held(RIGHT_BUTTON) - event->held(LEFT_BUTTON));

			if (curRow < 0) curRow = 0;
			if (index < 0) index = 0;
			if (curRow >= rowCount() + 1) curRow = rowCount(); // +1 for bottom "row" (tab, space, enter)
			if (curRow == rowCount())
			{
				// go to space key if last index is in the middle of row
				if (lastRow < curRow && index > 0 && index < rowLength(lastRow))
				{
 					index = 1;
				}

				// tab key
				if (index < 0) index = 0;

				// enter key
				if (index > 2) index = 2;
			}
			else
			{
				if (index > rowLength(curRow))
					index = rowLength(curRow);

				if (lastRow == rowCount()) {
					switch (index)
					{
						case 0: //tab
							index = 0;
							break;
						case 1: // space
							// go to middle of current row
							index = rowLength(curRow) / 2;
							break;
						case 2: // enter
							// go to end of current row
							index = rowLength(curRow);
							break;
						default:
							break;
					}
				}
			}

			if (event->held(A_BUTTON))
			{
				// space bar and enter key
				if (curRow >= rowCount())
				{
					switch (index)
					{
						case 0:
							just_type('\t');
							break;
						case 1:
							just_type(' ');
							break;
						case 2:
							just_type('\n');
							break;
						default:
							break;
					}
					return true;
				}

				type(curRow, index);
			}

			updateView();
			return true;
		}

		return false;
	}

	int extWidth = width + 305;

	if (event->isTouchDown() && event->touchIn(this->x, this->y, extWidth, height + 200))
	{
		for (int y = 0; y < rowCount(); y++)
			for (int x = 0; x < rowLength(y) + 1; x++)
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

		if (event->touchIn(this->x, this->y, extWidth, height + 200))
		{

			for (int y = 0; y < rowCount(); y++)
				for (int x = 0; x < rowLength(y) + 1; x++)
					if (event->touchIn(this->x + kXPad + x * kXOff + y * yYOff, this->y + kYPad + y * ySpacing, keyWidth, keyWidth))
					{
						ret |= true;
						type(y, x);
					}

			if (event->touchIn(this->x + dPos, this->y + enterHeight, enterWidth, textSize))
			{
				ret |= true;
				just_type('\t');
			}

			if (event->touchIn(this->x + sPos, this->y + dHeight, sWidth, textSize))
			{
				ret |= true;
				just_type(' ');
			}

			if (event->touchIn(this->x + enterPos, this->y + enterHeight, enterWidth, textSize))
			{
				ret |= true;
				just_type('\n');
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
	rows.clear();

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
	// int kYPad = (int)((14 / 400.0) * width);
	int kYOff = (int)((33 / 400.0) * width);

	this->textSize = 0.9375 * keyWidth;

	// delete, space and enter key dimensions
	dPos = (int)((13 / 400.0) * width);
	dHeight = (int)((85 / 135.0) * height) + 145;
	sPos = (int)((150 / 400.0) * width);
	enterPos = dPos + 1000;
	enterHeight = dHeight - 34;

	dWidth = (int)(1.4125 * textSize);
	sWidth = (int)(7.5 * textSize);
	enterWidth = (int)(2.25 * textSize);

	// set up the keys vector based on the current EKeyboard selection
	generateEKeyboard();

	CST_Color gray = { 0x52, 0x52, 0x52, 0xff };

	// go through and draw each of the three rows at the right position
	for (int x = 0; x < rowCount(); x++)
	{
		TextElement* rowText = new TextElement(rows[x]->c_str(), textSize, &gray, true);
		rowText->position(kXPad + x * kXOff, kYPad + x * kYOff);
		this->elements.push_back(rowText);
	}

	// text for space, enter, and symbols
	CST_Color grayish = { 0x55, 0x55, 0x55, 0xff };
	TextElement* spaceText = new TextElement("space", 30, &grayish);
	CST_Rect d4 = { this->x + sPos, this->y + dHeight, sWidth, textSize }; // todo: extract out hardcoded rects like this
	spaceText->position(d4.x + d4.w / 2 - spaceText->width / 2 - 15, 345);
	this->elements.push_back(spaceText);

	if (typeAction == NULL)
	{
		TextElement* enterText = new TextElement("enter", 30, &grayish);
		CST_Rect d3 = { this->x + enterPos, this->y + enterHeight, enterWidth, textSize }; // todo: extract out hardcoded rects like this
		enterText->position(d3.x + d3.w / 2 - enterText->width / 2 - 30, 327);
		this->elements.push_back(enterText);

		TextElement* symText = new TextElement("tab", 30, &grayish);
		CST_Rect d5 = { this->x + dPos, this->y + enterHeight, enterWidth, textSize }; // todo: extract out hardcoded rects like this
		symText->position(d5.x + d5.w / 2 - symText->width / 2 - 30, 327);
		this->elements.push_back(symText);
	}
}

void EKeyboard::type(int y, int x)
{
	const char input = (*(rows[y]))[x * 2];

	if (typeAction != NULL) {
		typeAction(input);
		return;
	}

	string sym(1, input);
	editorView->editor->appendHistory(sym.c_str(), editorView->mainTextField->selectedPos, false);

	auto pos = editorView->mainTextField->selectedPos;
	editorView->editor->type(pos, input);

	editorView->mainTextField->selectedPos++;
	editorView->syncText();
}

void EKeyboard::just_type(const char input)
{
	if (typeAction != NULL) {
		if (input != ' ')
			return;	// don't let them type tab/enter in typeAction (aka filename)
		typeAction(input);
		return;
	}

	auto pos = editorView->mainTextField->selectedPos;

	if (input == '\n')
		editorView->editor->newline(pos);
	else
		editorView->editor->type(pos, input);

	editorView->mainTextField->selectedPos++;
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
		int offset = 47 * (mode - 1) + 0x00a1;
		char chars[47 + (mode == 2)];
		for (int x = 0; x < 47 + (mode == 2); x++)
		{
			chars[x] = offset + x;
		}

		keys = string(chars);
	}

	breaks[3] += (mode == 2); // one more key for bottom row of mode 2

	for (int& end : breaks)
	{
		string* row = new string(keys.substr(count, end));
		for (int x = 1; x < (int)row->size(); x += 2)
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
