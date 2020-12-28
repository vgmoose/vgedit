#include <string>
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/DrawUtils.hpp"
#include "../edit/Editor.hpp"

#ifndef TEXTINPUTELEMENT_H
#define TEXTINPUTELEMENT_H

class TextInputElement : public ListElement
{
public:
	TextInputElement(Editor* editor);
	void updateText(const char* text);

	bool process(InputEvents* event);
	void render(Element* parent);

	void drawLineNo(int actualLineNo, int lineXPos, int actualLineYPos);
	void setStatus(const char* status);

	int selectedPos = 0;
	int selectedWidth = 1;

	int selectedXPos = 0;
	int selectedYPos = 0;

	int lastSelectedXPos = -1;
	int lastSelectedYPos = -1;

	// variables to track the last touch for cursor positioning
	int lastTouchX = -1;
	int lastTouchY = -1;

	// true if we want to display a custom status for a bit
	bool customStatus = false;

	int bonusWidthInSelection = 0;

	// set to true after the first render, to help prevent strange touch-racing inputs
	bool hasRendered = false;

	int COLS = 95;
	int lineSpacing = 2;

	int fontHeight;
	int fontWidth;

	bool insertMode = false;

	Editor* editor;
	CST_Font* font = NULL;
	CST_Font* lineFont = NULL;
};

#endif