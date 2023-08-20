#include "MainDisplay.hpp"
#include "EditorView.hpp"
#include "FileBrowser.hpp"

MainDisplay::MainDisplay()
{
	RootDisplay::super();
	backgroundColor = fromRGB(0x42, 0x45, 0x48);
}

void MainDisplay::openFile(bool isFolder, std::string* path)
{
	// don't allow a file to be opened if we're already showing an editor
	if (editorView != NULL)
		return;

	if (isFolder && browser != NULL)
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
	return RootDisplay::process(event);
}

void MainDisplay::closeEditor()
{
	if (browser == NULL)
	{
		// The file browser was never created, so let's leave the whole app
		// (single file mode)

		if (callbackPath != "") {
			// TODO: on switch/wiiu set the next nro/wuhb to launch
			std::cout << "Launching " << callbackPath << std::endl;
		}

		exit(0);
		return;
	}

	// otherwise, we'll go back to the file browser (this is _probably_ safe)
	elements.erase(elements.begin() + 1); // second element should be the editor (TODO: something smarter)
	editorView->wipeAll(); // destroy subelements
	delete editorView;
	editorView = NULL;
}
