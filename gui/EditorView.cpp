#include "EditorView.hpp"
#include "EKeyboard.hpp"

EditorView::EditorView(Editor* editor)
{
	// initialize the text view for the first time, using whatever's in the editor
	mainTextField = new TextInputElement(editor);
	mainTextField->x = 10;
	mainTextField->y = 70;
	this->elements.push_back(mainTextField);

	// create a tool bar, but don't add it to the elements list
	// it will be manually drawn in later (above everything else)
	toolbar = new Toolbar(editor->filename, this);

	this->editor = editor;
	this->text = editor->text;
}

void EditorView::render(Element* parent)
{
	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(parent->renderer, NULL);

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
	selected_width = selected_width > editor->curLineLength - selected_x ? editor->curLineLength - selected_x : selected_width;

	mainTextField->selectedWidth = selected_width || 1;

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

	// perform a save
	if (e->released(START_BUTTON))
	{
		toolbar->setModified(false);
		editor->save();
		return true;
	}

	// delete what's under the current selection (not backspace)
	if (e->pressed(B_BUTTON))
	{
		// in insert mode, delete is a "backspace-style" action, and moves the cursor left if it can
		if (mainTextField->insertMode)
		{
			if (mainTextField->selectedPos <= 0)
				return false;
			
			mainTextField->selectedPos--;
		}

		editor->del(mainTextField->selectedPos, mainTextField->selectedWidth);
		syncText();
		return true;
	}

	// bring up the keyboard
	if (e->released(A_BUTTON))
	{
		if (keyboard == NULL)
		{
			keyboard = new EKeyboard(this);
			this->elements.push_back(keyboard);
		}

		keyboard->hidden = false;

		// force selection to be width 1 (more than 1 doesn't make sense in insert mode)
		// (but it does make sense for vertical selections, to type on multiple lines)
		mainTextField->selectedWidth = 1;
		mainTextField->insertMode = true;
		toolbar->keyboardShowing = true;

		reset_bounds();

		return true;
	}

	if (keyboard == NULL || keyboard->hidden)
	{
		// copy and paste on X and Y
		if (e->released(X_BUTTON))
		{
			copySelection();
			return true;
		}
	}
	else
	{
		if (e->released(X_BUTTON))
		{
			// editor->overwriteMode = !editor->overwriteMode;
			// TODO: add overwrite mode back when hex editor is here

			keyboard->shiftOn = !keyboard->shiftOn;
			keyboard->updateSize();
			return true;
		}

		if (e->released(Y_BUTTON))
		{
			keyboard->hidden = true;
			mainTextField->insertMode = false;
			toolbar->keyboardShowing = false;
			return true;
		}
	}

	if (e->released(Y_BUTTON))
	{
		pasteSelection();
		return true;
	}

	if (e->pressed(L_BUTTON | R_BUTTON | ZL_BUTTON | ZR_BUTTON))
	{
		// move the bounds of the selection
		if (e->pressed(L_BUTTON))
			mainTextField->selectedWidth -= 1;
		if (e->pressed(R_BUTTON))
			mainTextField->selectedWidth += 1;
		// if (e->pressed(ZL_BUTTON))
		//   mainTextField->selected_height -= 1;
		// if (e->pressed(ZR_BUTTON))
		//   mainTextField->selected_height += 1;

		reset_bounds();
		return true;
	}

	return super::process(e) || toolbar->process(e);
}
