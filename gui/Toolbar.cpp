#include "Toolbar.hpp"
#include "MainDisplay.hpp"
#include "EKeyboard.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/Button.hpp"

Toolbar::Toolbar(const char* path, EditorView* editorView)
{
	if (strlen(path) > MAX_PATH_LENGTH) {
		strcpy(this->path, "...");
		strcpy(this->path + 3, path + strlen(path) - (MAX_PATH_LENGTH - 3));
	} else {
		strcpy(this->path, path);
	}

	this->initButtons(editorView);
}

void Toolbar::initButtons(EditorView* editorView)
{
	this->removeAll(true);	// TOOD: memory leak from old toolbar

	pathE = new TextElement(this->path, 20, 0, MONOSPACED);
	pathE->position(10, 13);
	elements.push_back(pathE);

	stats = new TextElement("ZZ characters", 20, 0, MONOSPACED);
	stats->position(10, SCREEN_HEIGHT - 35);
	elements.push_back(stats);

	CST_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };

	auto editor = editorView->editor;
	auto textField = editorView->mainTextField;

	bool dark = true;
	int bsize = 16;
	
	// member vars
	auto isInsert = textField->insertMode;
	auto keyboard = editorView->keyboard;

	Container* con = new Container(ROW_LAYOUT, 5);
	Container* bot = new Container(ROW_LAYOUT, 5);
	elements.push_back(con);
	elements.push_back(bot);

	con->add((new Button("Exit", SELECT_BUTTON, dark, bsize))->setAction([](){
		((MainDisplay*)RootDisplay::mainDisplay)->closeEditor();
	}));

	con->add((new Button("Save", START_BUTTON, dark, bsize))->setAction([this, editor](){
		// perform a save
		this->setModified(false);
		editor->save();
	})),

	con->add((new Button(isInsert ? "Caps" : "Copy", X_BUTTON, dark, bsize))->setAction([isInsert, editorView, keyboard](){
		if (isInsert) {
			keyboard->shiftOn = !keyboard->shiftOn;
			keyboard->updateSize();
			return;
		}
		editorView->copySelection();
	}));

	con->add((new Button(isInsert ? "Stow Keyboard" : "Paste", Y_BUTTON, dark, bsize))->setAction([this, isInsert, textField, keyboard, editorView](){
		if (isInsert) {
			keyboard->hidden = true;
			textField->insertMode = false;
			this->keyboardShowing = false;

			// reload toolbar
			this->initButtons(editorView);
			return;
		}
		editorView->pasteSelection();
	}));

	// L and R don't do anything in insert mode (no selections)
	if (!isInsert)
	{
		bot->add((new Button("Undo", ZL_BUTTON, dark, bsize))->setAction([textField, editorView](){
			// TODO: add undo
		}));

		bot->add((new Button("Redo", ZR_BUTTON, dark, bsize))->setAction([textField, editorView](){
			// TODO: add redo
		}));

		bot->add((new Button("Find", L_BUTTON, dark, bsize))->setAction([textField, editorView](){
			textField->selectedWidth -= 1;
			editorView->reset_bounds();
		}));

		bot->add((new Button("Select", R_BUTTON, dark, bsize))->setAction([textField, editorView](){
			textField->selectedWidth += 1;
			editorView->reset_bounds();
		}));

		bot->add((new Button("Show Keyboard", A_BUTTON, dark, bsize))->setAction([this, textField, keyboard, editorView](){
			if (keyboard == NULL)
			{
				editorView->keyboard = new EKeyboard(editorView);
				editorView->elements.push_back(editorView->keyboard);
			}

			editorView->keyboard->hidden = false;
			editorView->keepCursorOnscreen = true;

			// force selection to be width 1 (more than 1 doesn't make sense in insert mode)
			// (but it does make sense for vertical selections, to type on multiple lines)
			textField->selectedWidth = 1;
			textField->insertMode = true;
			this->keyboardShowing = true;

			editorView->reset_bounds();

			// re-init toolbar with insert mode options
			this->initButtons(editorView);
		}));

	}

	con->add((new Button(isInsert ? "Backspace" : "Delete", B_BUTTON, dark, bsize))->setAction([textField, editor, editorView](){
		// in insert mode, delete is a "backspace-style" action, and moves the cursor left if it can
		if (textField->insertMode)
		{
			if (textField->selectedPos <= 0)
				return;
			
			textField->selectedPos--;
		}

		editor->del(textField->selectedPos, textField->selectedWidth);
		editorView->syncText();
	}));

	con->position(SCREEN_WIDTH - 10 - con->width, 5);
	bot->position(SCREEN_WIDTH - 10 - bot->width, SCREEN_HEIGHT - bot->height - 5);
}

void Toolbar::setModified(bool modified)
{
	this->modified = modified;

	std::string text;
	text += path;
	text += (modified ? "*" : "");

  pathE->setText(text);
  pathE->update();
}

void Toolbar::render(Element* parent)
{
	CST_Rect topBg = { 0, 0, SCREEN_WIDTH, 50 };

	// draw top bar on top of other things
	CST_SetDrawColor(parent->renderer, { 0x40, 0x40, 0x40, 0xFF });
	CST_FillRect(parent->renderer, &topBg);

	if (!keyboardShowing) {
		CST_Rect botBg = { 0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50 };

		// draw top bar on top of other things
		CST_FillRect(parent->renderer, &botBg);
	}

	super::render(parent);
}