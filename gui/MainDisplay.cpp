#include "MainDisplay.hpp"
#include "EditorView.hpp"
#include "FileBrowser.hpp"

MainDisplay::MainDisplay()
{
	RootDisplay::super();

  // backgroundColor = { 0x42, 0x45, 0x48, 0xFF };
}

void MainDisplay::openFile(bool isFolder, std::string* path)
{
	// don't allow a file to be opened if we're already showing an editor
	if (editorView != NULL)
		return;

	if (isFolder)
	{
		browser->update_path(path->c_str());
		browser->y = 0;
		browser->selected = 0;
		browser->listfiles();
	}
	else
	{
		Editor* editor = new Editor(path->c_str());
		editorView = new EditorView(editor);
		this->elements.push_back(editorView);
	}
}

bool MainDisplay::process(InputEvents* event)
{
	// keep processing child elements
	return super::process(event) || showingSplash;
}

void MainDisplay::closeEditor()
{
	elements.erase(elements.begin() + 1); // second element should be the editor (TODO: something smarter)
	delete editorView;
	editorView = NULL;
}