#include "../gui/EditorView.hpp"
#include "../gui/FileBrowser.hpp"
#include "../gui/MainDisplay.hpp"
#include "../libs/chesto/src/Element.hpp"

#include "../libs/json/single_include/nlohmann/json.hpp"
#include <algorithm>


#if defined(__WIIU__)
#include <sysapp/launch.h>
#endif

int main(int argc, char* argv[])
{
	// initialize main title screen
	MainDisplay* display = new MainDisplay();

	// the main input handler
	auto events = display->events;
	events->rapidFireRate = 6; // 2x faster directional key repeat

	std::string startPath = START_PATH;

	// USAGE: vgedit [file to edit path] [callback path]
	if (argc > 1) {
		// get the file path from the command line
		startPath = argv[1];

		// if there's a callback path, store it for later
		if (argc > 2) {
			display->callbackPath = argv[2];
		}
	}

	// check if the args.json file exists
	std::ifstream argsFile("args.json");
	if (argsFile.good()) {
		// parse the args.json file
		std::string argsJson((std::istreambuf_iterator<char>(argsFile)), std::istreambuf_iterator<char>());
		auto args = nlohmann::json::parse(argsJson);

		// check if the file path is specified
		if (args.contains("filename")) {
			startPath = args["filename"];

			// if there's a callback path, store it for later
			if (args.contains("callback")) {
				display->callbackPath = args["callback"];
			}

			// since we got the path, we can delete the args.json file
			std::remove("args.json");
		}
	}

	// setup the quit callback (used by FileBrowser or EditorView in single file mode)
	auto quitaction = [display]() {
		display->exitRequested = true;
		display->isRunning = false;
	};

	events->quitaction = quitaction;

	// if the start path isn't a directory,use single file mode
	if (startPath != START_PATH && !std::filesystem::is_directory(startPath))
	{
		// if the file doesn't exist, create it TODO: entire path?
		if (!std::filesystem::exists(startPath)) {
			std::ofstream file(startPath);
			file.close();
		}

		display->openFile(false, &startPath);

	} else {
		// otherwise, open a file browser to the start path
		FileBrowser* fileBrowser = new FileBrowser(startPath.c_str());
		display->browser = fileBrowser;
		display->elements.push_back(fileBrowser);
	}

	display->mainLoop();

	return 0;
}
