#include "Toolbar.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/EKeyboard.hpp"
#include "MainDisplay.hpp"
#include "TextQueryPopup.hpp"

using namespace Chesto;

Toolbar::Toolbar(const char* path, EditorView* editorView)
{
	if (strlen(path) > MAX_PATH_LENGTH)
	{
		strcpy(this->path, "...");
		strcpy(this->path + 3, path + strlen(path) - (MAX_PATH_LENGTH - 3));
	}
	else
	{
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
	std::string& chars = event.chars;

	if (isDelete)
	{
		editor->text->erase(pos, chars.length());
	}
	else
	{
		editor->text->insert(pos, chars.c_str());
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

	RootDisplay::deferAction([this]()
		{ this->needsRebuild = true; });
}

void Toolbar::initButtons(EditorView* editorView)
{
	this->removeAll();
	displayedPrompt = false;

	auto pathEPtr = std::make_unique<TextElement>(this->path, 20, nullptr, MONOSPACED);
	pathEPtr->position(10, 13);
	pathE = pathEPtr.get();
	setModified(modified);
	addNode(std::move(pathEPtr));

	auto statsPtr = std::make_unique<TextElement>("ZZ characters", 20, nullptr, MONOSPACED);
	statsPtr->position(10, SCREEN_HEIGHT - 35);
	stats = statsPtr.get();
	addNode(std::move(statsPtr));

	CST_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };

	auto editor = editorView->editor;
	auto textField = editorView->mainTextField;

	bool dark = true;
	int bsize = 16;

	// member vars
	auto isInsert = textField->insertMode;
	auto keyboard = editorView->keyboard;

	auto conPtr = std::make_unique<Container>(ROW_LAYOUT, 5);
	auto botPtr = std::make_unique<Container>(ROW_LAYOUT, 5);
	auto con = conPtr.get();
	auto bot = botPtr.get();
	addNode(std::move(conPtr));
	addNode(std::move(botPtr));

	con->createNode<Button>("Exit", SELECT_BUTTON, dark, bsize)->setAction([this, isInsert, keyboard, textField, editorView]()
		{
		if (isInsert) {
			stowKeyboard(keyboard, textField, editorView);
			return;
		}
		if (modified && !displayedPrompt) {
			displayedPrompt = true;
			textField->setStatus("Unsaved changes! Press [Exit] again to confirm");
			return;
		}
		RootDisplay::deferAction([]() {
			((MainDisplay*)RootDisplay::mainDisplay)->closeEditor();
		}); });

	con->createNode<Button>("Save", START_BUTTON, dark, bsize)->setAction([this, textField, editor]()
		{
		// perform a save
		this->setModified(false);
		bool success = editor->save();
		if (success) {
			textField->setStatus("File saved successfully");
		} });

	con->createNode<Button>(isInsert ? "Caps" : "Copy", X_BUTTON, dark, bsize)->setAction([isInsert, textField, editorView, keyboard]()
		{
		if (isInsert) {
			keyboard->shiftOn = !keyboard->shiftOn;
			keyboard->updateSize();
			return;
		}
		editorView->copySelection();
		textField->setStatus("Copied selection to clipboard"); });

	con->createNode<Button>(isInsert ? "Stow Keyboard" : "Paste", Y_BUTTON, dark, bsize)->setAction([this, isInsert, textField, keyboard, editorView]()
		{
		if (isInsert) {
			stowKeyboard(keyboard, textField, editorView);
			return;
		}
		if (!editorView->pasteSelection()) {
			textField->setStatus("Nothing on clipboard to paste!");
		} });

	// L and R don't do anything in insert mode (no selections)
	if (!isInsert)
	{
		auto historyPos = editor->historyPos;
		auto undoHistory = editor->undoHistory;
		bot->createNode<Button>("Undo", ZL_BUTTON, dark, bsize)->setAction([this, editor, textField]()
			{
			auto success = commonHistoryLogic(true, editor, textField);
			editor->historyPos--;
			if (editor->historyPos < 0) editor->historyPos = -1;
			if (!success) textField->setStatus("Nothing to undo!"); });

		bot->createNode<Button>("Redo", ZR_BUTTON, dark, bsize)->setAction([this, editor, textField]()
			{
			editor->historyPos++;
			auto success = commonHistoryLogic(false, editor, textField);
			if (editor->historyPos > editor->undoHistory.size()) editor->historyPos = editor->undoHistory.size();
			if (!success) textField->setStatus("Nothing to redo!"); });

		Button deselect("Deslct", L_BUTTON, dark, bsize);
		auto findButtonPtr = std::make_unique<Button>("Find...", L_BUTTON, dark, bsize, deselect.width);
		findButton = findButtonPtr.get();
		findButton->setAction([this, textField, editorView, editor]()
			{
			if (textField->selectedWidth == 1) {
				// size 1 uses Find function since we're out of buttons
				auto popup = std::make_unique<TextQueryPopup>("Enter search criteria", "Find", [this, textField, editor](const char* query){
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
				RootDisplay::pushScreen(std::move(popup));
				return;
			}
			textField->selectedWidth -= 1;
			if (textField->selectedWidth <= 1) findButton->updateText("Find...");
			editorView->reset_bounds(); });
		bot->addNode(std::move(findButtonPtr));

		bot->createNode<Button>("Select", R_BUTTON, dark, bsize)->setAction([this, textField, editorView]()
			{
			textField->selectedWidth += 1;
			editorView->reset_bounds();
			if (textField->selectedWidth > 1) findButton->updateText("Deslct"); });

		bot->createNode<Button>("Show Keyboard", A_BUTTON, dark, bsize)->setAction([this, textField, keyboard, editorView]()
			{
			if (keyboard == NULL)
			{
				auto keyboardPtr = std::make_unique<EKeyboard>([editorView](char input) {
					// main callback action that enters text into the editor
					std::string sym(1, input);
					editorView->editor->appendHistory(sym.c_str(), editorView->mainTextField->selectedPos, false);

					auto pos = editorView->mainTextField->selectedPos;
					if (input == '\n')
						editorView->editor->newline(pos);
					else
						editorView->editor->type(pos, input);

					editorView->mainTextField->selectedPos++;
					editorView->syncText();
				});

				editorView->keyboard = keyboardPtr.get();
				editorView->addNode(std::move(keyboardPtr));
			}

			editorView->keyboard->hidden = false;
			editorView->keepCursorOnscreen = true;

#if defined(_3DS) || defined(_3DS_MOCK)
			editorView->keyboard->width = SCREEN_WIDTH - 140;
			editorView->keyboard->position(40, 360);
			editorView->keyboard->updateSize();
#endif

			// force selection to be width 1 (more than 1 doesn't make sense in insert mode)
			// (but it does make sense for vertical selections, to type on multiple lines)
			textField->selectedWidth = 1;
			textField->insertMode = true;
			this->keyboardShowing = true;

			editorView->reset_bounds();

			RootDisplay::deferAction([this]() {
				this->needsRebuild = true;
			}); });
	}

	con->createNode<Button>(isInsert ? "Backspace" : "Delete", B_BUTTON, dark, bsize)->setAction([textField, editor, editorView]()
		{
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

		textField->render(editorView); });

	con->position(SCREEN_WIDTH - 10 - con->width, 5);
	bot->position(SCREEN_WIDTH - 10 - bot->width, SCREEN_HEIGHT - bot->height - 5);
}

bool Toolbar::process(InputEvents* event)
{
	// Check and manually rebuild if flagged TODO: just use defers? Ideally we shouldn't have to override this
	if (needsRebuild && parent)
	{
		needsRebuild = false;
		auto editorView = (EditorView*)parent;
		initButtons(editorView);
		return true;
	}

	bool ret = Element::process(event);
	return ret;
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

	auto renderer = RootDisplay::renderer;

	// draw top bar on top of other things
	CST_SetDrawColor(renderer, { 0x40, 0x40, 0x40, 0xFF });
	CST_FillRect(renderer, &topBg);

	if (!keyboardShowing)
	{
		CST_Rect botBg = { 0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50 };

		// draw top bar on top of other things
		CST_FillRect(renderer, &botBg);
	}

	super::render(parent);
}