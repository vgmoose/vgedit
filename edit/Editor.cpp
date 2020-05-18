#include "Editor.hpp"

Editor::Editor(const char* filename)
{
	this->filename = filename;

	if (filename == NULL)
		filename = "test.txt";

	this->open(filename);
}

int Editor::open(const char* filename)
{
	// open input stream for file
	ifstream input(filename);

	if (input.tellg() == -1)
	{
		// quit if file is missing
		cout << "target file does not exist" << endl;
		return -1;
	}

	// TODO: check and prompt for hex files

	internal_load_from_stream(&input);

	input.close();

	// file loaded successfully
	return 0;
}

void Editor::internal_load_from_stream(ifstream* input)
{
	// read into our string representation
	std::stringstream buffer;
	buffer << input->rdbuf();
	text = new std::string(buffer.str());

	// prevent empty files
	if (text->empty())
	{
		// new line at the end 
		// https://stackoverflow.com/a/31426524
		text->append("\n");
	}
}

bool Editor::save()
{
	ofstream output;
	output.open(filename);

	if (text == NULL) return false;

	for (auto& letter : *text)
		output << letter;

	output.close();
	return true;
}

bool Editor::type(int pos, const char input)
{
	// TODO: handle vertical selections

	// if "overwrite" is on, delete what's under us first, given it's not the end of the line
	if (overwriteMode && pos < curLineLength - 1)
		text->erase(pos, 1);

	text->insert(pos, 1, input);
	return true;
}

bool Editor::del(int pos, int size)
{
	// TODO: take in a Selection instead and work accordingly
	text->erase(pos, size);

	return true;
}

bool Editor::newline(int pos)
{
	type(pos, '\n');

	return true;
}
