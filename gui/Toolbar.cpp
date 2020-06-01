#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "Toolbar.hpp"
#include "MainDisplay.hpp"
#include "EKeyboard.hpp"
#include "TextQueryPopup.hpp"

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

bool Toolbar::commonHistoryLogic(bool isUndo, Editor* editor, TextInputElement* textField)
{
	auto history = editor->undoHistory;
	auto historyPos = editor->historyPos;

	// out of bounds, stop
	if (historyPos < 0 || historyPos >= history.size())
		return false;

	auto event = history.at(historyPos);

	// we need to flip the event's delete boolean if we're doing an undo
	bool isDelete = event.isDelete != isUndo;

	int pos = event.pos;
	std::string* chars = &event.chars;

	if (isDelete) {
		editor->text->erase(pos, chars->length());
	} else {
		editor->text->insert(pos, chars->c_str());
	}

	// move cursor
	textField->selectedPos = pos;

	// for (int x=0; x<history.size(); x++) {
	// 	auto event = history.at(x);
	// 	printf("%d (%d): chars: [%s], pos: %d, action: %d\n", x, x == historyPos, event.chars.c_str(), event.pos, event.isDelete);
	// }

	return true;
}

void Toolbar::stowKeyboard(EKeyboard* keyboard, TextInputElement* textField, EditorView* editorView)
{
	keyboard->hidden = true;
	textField->insertMode = false;
	this->keyboardShowing = false;

	// reload toolbar
	this->initButtons(editorView);
}

void Toolbar::initButtons(EditorView* editorView)
{
	this->removeAll(true);
	displayedPrompt = false;

	pathE = new TextElement(this->path, 20, 0, MONOSPACED);
	pathE->position(10, 13);
	setModified(modified);
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

	con->add((new Button("Exit", SELECT_BUTTON, dark, bsize))->setAction([this, isInsert, keyboard, textField, editorView](){
		if (isInsert) {
			stowKeyboard(keyboard, textField, editorView);
			return;
		}
		if (modified && !displayedPrompt) {
			displayedPrompt = true;
			textField->setStatus("Unsaved changes! Press [Exit] again to confirm");
			return;
		}
		((MainDisplay*)RootDisplay::mainDisplay)->closeEditor();
	}));

	con->add((new Button("Save", START_BUTTON, dark, bsize))->setAction([this, textField, editor](){
		// perform a save
		this->setModified(false);
		bool success = editor->save();
		if (success) {
			textField->setStatus("File saved successfully");
		}
	})),

	con->add((new Button(isInsert ? "Caps" : "Copy", X_BUTTON, dark, bsize))->setAction([isInsert, textField, editorView, keyboard](){
		if (isInsert) {
			keyboard->shiftOn = !keyboard->shiftOn;
			keyboard->updateSize();
			return;
		}
		editorView->copySelection();
		textField->setStatus("Copied selection to clipboard");
	}));

	con->add((new Button(isInsert ? "Stow Keyboard" : "Paste", Y_BUTTON, dark, bsize))->setAction([this, isInsert, textField, keyboard, editorView](){
		if (isInsert) {
			stowKeyboard(keyboard, textField, editorView);
			return;
		}
		if (!editorView->pasteSelection()) {
			textField->setStatus("Nothing on clipboard to paste!");
		}
	}));

	// L and R don't do anything in insert mode (no selections)
	if (!isInsert)
	{
		auto historyPos = editor->historyPos;
		auto undoHistory = editor->undoHistory;
		bot->add((new Button("Undo", ZL_BUTTON, dark, bsize))->setAction([this, editor, textField](){
			auto success = commonHistoryLogic(true, editor, textField);
			editor->historyPos--;
			if (editor->historyPos < 0) editor->historyPos = -1;
			if (!success) textField->setStatus("Nothing to undo!");
		}));

		bot->add((new Button("Redo", ZR_BUTTON, dark, bsize))->setAction([this, editor, textField](){
			editor->historyPos++;
			auto success = commonHistoryLogic(false, editor, textField);
			if (editor->historyPos > editor->undoHistory.size()) editor->historyPos = editor->undoHistory.size();
			if (!success) textField->setStatus("Nothing to redo!");
		}));

		Button deselect("Deslct", L_BUTTON, dark, bsize);
		findButton = (Button*)(new Button("Find...", L_BUTTON, dark, bsize, deselect.width))->setAction([this, textField, editorView, editor](){
			if (textField->selectedWidth == 1) {
				// size 1 uses Find function since we're out of buttons
				auto popup = new TextQueryPopup("Enter search criteria", "Find", [this, textField, editor](const char* query){
					// we're gonna try to find the first occurrence of the case-insensitive string they give us
					int res = editor->text->find(query, textField->selectedPos + 1);
					if (res == std::string::npos) {
						// no match found, retry with no bounds
						res = editor->text->find(query);
						if (res == std::string::npos) {
							// still not found...
							textField->setStatus("No match found in file");
							this->searchQuery.assign(""); // reset saved search
							return;
						}
						textField->setStatus("Search hit bottom, continued at top");
					}
					textField->selectedPos = res;
					this->searchQuery.assign(query); // save query if we had a match
				});
				popup->query = this->searchQuery.c_str();
				popup->queryText->setText(popup->query);
				popup->queryText->update();
				RootDisplay::mainDisplay->switchSubscreen(popup);
				return;
			}
			textField->selectedWidth -= 1;
			if (textField->selectedWidth <= 1) findButton->updateText("Find...");
			editorView->reset_bounds();
		});
		bot->add(findButton);

		bot->add((new Button("Select", R_BUTTON, dark, bsize))->setAction([this, textField, editorView](){
			textField->selectedWidth += 1;
			editorView->reset_bounds();
			if (textField->selectedWidth > 1) findButton->updateText("Deslct");
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

		// if the file is empty, append empty line
		if (editor->text->length() == 0)
			editor->text->append("\n");

		textField->render(editorView);
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