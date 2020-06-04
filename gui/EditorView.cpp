#include "EditorView.hpp"
#include "EKeyboard.hpp"
#include "MainDisplay.hpp"

EditorView::EditorView(Editor* editor)
{
	// initialize the text view for the first time, using whatever's in the editor
	mainTextField = new TextInputElement(editor);
	mainTextField->x = 10;
	mainTextField->y = 70;
	this->elements.push_back(mainTextField);

	this->editor = editor;
	this->text = editor->text;

	// create a tool bar, but don't add it to the elements list
	// it will be manually drawn in later (above everything else)
	toolbar = new Toolbar(editor->filename, this);
}

void EditorView::render(Element* parent)
{
	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(parent->renderer, NULL);

	// SDL_SetRenderDrawColor(parent->renderer, 0xee, 0xee, 0xee, 0xff);
	// SDL_Rect rect = {0, 0, 10 + editor->lineNoPlaces * mainTextField->fontWidth, SCREEN_HEIGHT};
	// SDL_RenderFillRect(parent->renderer, &rect);

	super::render(parent);

	toolbar->render(parent);
}

void EditorView::reset_bounds()
{
	int selected_x = mainTextField->selectedPos;

	// loop around in x direction
	selected_x = selected_x < 0 ? 0 : selected_x;
	selected_x = selected_x > text->length() - 1 ? text->length() - 1 : selected_x;

	mainTextField->selectedPos = selected_x;

	int selected_width = mainTextField->selectedWidth;

	// adjust the bounds of the selection
	auto posInLine = mainTextField->selectedPos - editor->curLinePos;
	selected_width = selected_width > editor->curLineLength - posInLine ? editor->curLineLength - posInLine + 1 : selected_width;
	selected_width = selected_width < 1 ? 1 : selected_width;

	mainTextField->selectedWidth = selected_width;

	// printf("main text area %d\n", mainTextField->selectedYPos);
	// always snap the cursor to be on screen and visible (by moving the screen)
	int h = mainTextField->fontHeight;
	float cursor_y = mainTextField->selectedYPos;

	// if this boolean is set, adjust the textfield in the direction of the cursor
	if (keepCursorOnscreen) {
		if (mainTextField->selectedPos == 0)
			mainTextField->y = 0;

		bool blockKeyboardDraws = mainTextField->insertMode &&  !editor->immersiveView;

		if (cursor_y > (blockKeyboardDraws ? 200 : (SCREEN_HEIGHT - 100)))
			mainTextField->y -= h/2;

		if (cursor_y < 50 && mainTextField->y < -1 * h/4)
			mainTextField->y += h/2;
	}
}

bool EditorView::copySelection()
{
	// TODO: handle vertical selections (abstract into Selection class?)

	if (copiedText)
		delete copiedText;
	
	int pos = mainTextField->selectedPos;
  	int width = mainTextField->selectedWidth;

	copiedText = new std::string(text->substr(pos, width));
	((MainDisplay*)RootDisplay::mainDisplay)->copiedText = copiedText;

	return true;
}

bool EditorView::pasteSelection()
{
	if (copiedText == NULL)
		copiedText = ((MainDisplay*)RootDisplay::mainDisplay)->copiedText;
	if (copiedText == NULL)
		return false;
	
	editor->appendHistory(copiedText->c_str(), mainTextField->selectedPos, false);

	for (char& letter : *copiedText)
		editor->type(mainTextField->selectedPos++, letter);

	syncText();
	return true;
}

void EditorView::syncText()
{
	reset_bounds();
	toolbar->setModified(true);
}

void EditorView::moveCursor(InputEvents* e)
{
	int relPos = (mainTextField->selectedPos - editor->curLinePos);

	if (e->pressed(LEFT_BUTTON))
		mainTextField->selectedPos -= 1;
	if (e->pressed(RIGHT_BUTTON))
		mainTextField->selectedPos += 1;
	if (e->pressed(UP_BUTTON))
	{
		int prev_rpos = editor->curLinePos - 1;
		int up_pos = editor->prevLinePos + relPos;
		mainTextField->selectedPos = min(up_pos, prev_rpos);
	}
	if (e->pressed(DOWN_BUTTON))
	{
		int next_lpos = editor->curLinePos + editor->curLineLength;
		int next_rpos = next_lpos + editor->nextLineLength;
		int down_pos = next_lpos + relPos;
		mainTextField->selectedPos = min(next_rpos, down_pos) + 1;
	}
}

bool EditorView::process(InputEvents* e)
{
	bool isPhysicalKey = e->type == SDL_KEYDOWN;
	bool processInput = keyboard == NULL || keyboard->hidden || isPhysicalKey;

	bool prevPassthrough = InputEvents::bypassKeyEvents;
	InputEvents::bypassKeyEvents = false;

	// move the cursoraround the editor
	if (processInput && e->pressed(LEFT_BUTTON | RIGHT_BUTTON | UP_BUTTON | DOWN_BUTTON))
	{
		// relative cursor position from the start
		moveCursor(e);

		// when we use a directional button for movement, start locking the cursor on screen
		keepCursorOnscreen = true;
		InputEvents::bypassKeyEvents = prevPassthrough;

		return true;
	}

	InputEvents::bypassKeyEvents = prevPassthrough;
	keepCursorOnscreen = !(e->isTouch() || e->isScroll() || mainTextField->dragging || mainTextField->elasticCounter != 0);
	// printf("status: %d, Event: %d (%d, %d, %d, %d)\n", keepCursorOnscreen, e->type, e->isTouch(), e->isScroll(), mainTextField->dragging, mainTextField->elasticCounter );

	return toolbar->process(e) || super::process(e);
}
