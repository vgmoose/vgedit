#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "../libs/chesto/src/TextElement.hpp"
#include "EditorView.hpp"

class EditorView;

class Toolbar : public Element
{
public:
	const static int MAX_PATH_LENGTH = 44;

	Toolbar(const char* path, EditorView* editorView);
	void initButtons(EditorView* editorView);
	void setModified(bool modified);
	
	void render(Element* parent);

	TextElement* pathE = NULL;
	TextElement* stats = NULL;
	std::string statusText = "";

	char path[MAX_PATH_LENGTH + 1];
	bool modified = true;
	bool keyboardShowing = false;
};

#endif