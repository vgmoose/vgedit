#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/EKeyboard.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "EditorView.hpp"

using namespace Chesto;

class EditorView;

class Toolbar : public Element
{
public:
	const static int MAX_PATH_LENGTH = 44;

	Toolbar(const char* path, EditorView* editorView);
	void initButtons(EditorView* editorView);
	void setModified(bool modified);

	bool process(InputEvents* event);
	void render(Element* parent);
	bool commonHistoryLogic(bool isUndo, Editor* editor, TextInputElement* textField);
	void stowKeyboard(EKeyboard* keyboard, TextInputElement* textField, EditorView* editorView);

	TextElement* pathE = NULL;
	TextElement* stats = NULL;

	std::string statusText = "";
	std::string searchQuery = "";

	Button* findButton = NULL;

	char path[MAX_PATH_LENGTH + 1];
	bool modified = false;
	bool keyboardShowing = false;
	bool displayedPrompt = false;

	bool needsRebuild = false;
};

#endif