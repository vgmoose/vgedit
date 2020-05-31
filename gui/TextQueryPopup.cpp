#include "../libs/chesto/src/Button.hpp"
#include "TextQueryPopup.hpp"
#include "EKeyboard.hpp"

TextQueryPopup::TextQueryPopup(const char* prompt, const char* ctaText, std::function<void(const char*)> onConfirm)
{
	this->width = 220;
	this->height = 200;

	// draw a big dim layer around the entire window before drawing this progress bar
	CST_Rect dim = { 0, 0, 1280, 720 };

	CST_SetDrawBlend(this->renderer, true);
	CST_SetDrawColorRGBA(this->renderer, 0x00, 0x00, 0x00, 0xbb);
	CST_FillRect(this->renderer, &dim);

	auto mainDisplay = RootDisplay::mainDisplay;

	auto promptText = new TextElement(prompt, 20);
	auto queryText = new TextElement("<folder name>", 20);

	auto keyboard = new EKeyboard(NULL);
	keyboard->typeAction = [this, queryText](char input) {
		query += input;
		queryText->setText(query);
		queryText->update();
	};
	child(keyboard);

	child(((new Button("Backspace", B_BUTTON))->setAction([this, queryText](){
		if (!query.empty())
			query.pop_back();
		queryText->setText(query);
		queryText->update();
	}))->setPosition(20, 10));

	std::function<void()> cleanUp = [this, mainDisplay](){
		this->removeAll();
		mainDisplay->switchSubscreen(NULL);
	};

	child(((new Button(ctaText, X_BUTTON))->setAction([this, onConfirm, cleanUp](){
		onConfirm(query.c_str());
		cleanUp();
	}))->setPosition(160, 110));

	child(((new Button("Cancel", Y_BUTTON))->setAction(cleanUp))->setPosition(50, 110));

	child(queryText->setPosition(300, 50));
}

void TextQueryPopup::render(Element* parent)
{
	// draw a big dim layer around the entire window before drawing
	CST_Rect dim = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	auto renderer = RootDisplay::mainDisplay->renderer;

	CST_SetDrawBlend(renderer, true);
	CST_SetDrawColorRGBA(renderer, 0x00, 0x00, 0x00, 0xbb);
	CST_FillRect(renderer, &dim);

	super::render(this);
}

TextQueryPopup::~TextQueryPopup()
{
	this->wipeAll();
}