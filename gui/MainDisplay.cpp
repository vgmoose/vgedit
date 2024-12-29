#include "MainDisplay.hpp"
#include "EditorView.hpp"
#include "FileBrowser.hpp"

#if defined(__WIIU__)
#include "../libs/librpxloader/include/rpxloader/rpxloader.h"
#endif

#if defined(SWITCH)
#include <switch.h>
#endif

MainDisplay::MainDisplay()
{
	RootDisplay::super();
	backgroundColor = fromRGB(0x42, 0x45, 0x48);

	RootDisplay::idleCursorPulsing = true;
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
			std::cout << "Launching " << callbackPath << std::endl;
			bool success = false;

			#ifdef __WIIU__
			RPXLoaderStatus ret = RPXLoader_InitLibrary();
			if (ret == RPX_LOADER_RESULT_SUCCESS) {
				auto res = RPXLoader_LaunchHomebrew(callbackPath.c_str());
				success = res == RPX_LOADER_RESULT_SUCCESS;
			}
			#endif

			#ifdef SWITCH
			// use envSetNextLoad to set the path to the app to launch
			if (envHasNextLoad()) {
				auto res = envSetNextLoad(callbackPath.c_str(), editorView->editor->filename); // give it the filename of the file we edited
				success = R_SUCCEEDED(res);
			}
			#endif

			if (!success) {
				std::cout << "Failed to launch " << callbackPath << std::endl;
				exit(1);
			}
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
