#include "EditorView.hpp"
#include "EKeyboard.hpp"

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
	selected_width = selected_width < 1 ? 1 : selected_width;
	selected_width = selected_width > editor->curLineLength - selected_x + 2 ? editor->curLineLength - selected_x + 2 : selected_width;

	mainTextField->selectedWidth = selected_width;

	// always snap the cursor to be on screen and visible (by moving the screen)
	// int h = mainTextField->fontHeight + 2;
	// float cursor_y = (h * mainTextField->selected_y - 50) * -1;

	// if (cursor_y > mainTextField->y + 50)
	// 	mainTextField->y += h;

	// if (cursor_y < mainTextField->y - 550)
	// 	mainTextField->y -= h;

	// // if it's still offscreen, and we're showing the keyboard
	// if (mainTextField->insertMode)
	// 	mainTextField->y = cursor_y;
}

bool EditorView::copySelection()
{
	// TODO: handle vertical selections (abstract into Selection class?)

	if (copiedText)
		delete copiedText;

	int pos = mainTextField->selectedPos;
  int width = mainTextField->selectedWidth;

	copiedText = new std::string(text->substr(pos, width));
	return true;
}

bool EditorView::pasteSelection()
{
	if (copiedText == NULL)
		return false;

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

bool EditorView::process(InputEvents* e)
{
	// move the cursoraround the editor
	if ((keyboard == NULL || keyboard->hidden) && e->pressed(LEFT_BUTTON | RIGHT_BUTTON | UP_BUTTON | DOWN_BUTTON))
	{
		// relative cursor position from the start
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

		reset_bounds();
		return true;
	}

	return toolbar->process(e) || super::process(e);
}
