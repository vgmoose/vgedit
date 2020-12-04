#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/EKeyboard.hpp"
#include "TextQueryPopup.hpp"

TextQueryPopup::TextQueryPopup(const char* prompt, const char* ctaText, std::function<void(const char*)> onConfirm)
{
	this->width = SCREEN_WIDTH;
	this->height = SCREEN_HEIGHT;

	this->x = 0;
	this->y = 0;
	this->isAbsolute = true;

	// draw a big dim layer around the entire window before drawing this progress bar
	CST_Rect dim = { 0, 0, 1280, 720 };

	CST_SetDrawBlend(this->renderer, true);
	CST_SetDrawColorRGBA(this->renderer, 0x00, 0x00, 0x00, 0xbb);
	CST_FillRect(this->renderer, &dim);

	auto mainDisplay = RootDisplay::mainDisplay;

	auto promptText = new TextElement(prompt, 22);
	queryText = new TextElement("", 28);

	auto keyboard = new EKeyboard([this](char input) {
		query += input;
		queryText->setText(query);
		queryText->update();
	});
	keyboard->preventEnterAndTab = true;
	keyboard->updateSize();
	child(keyboard);

	std::function<void()> cleanUp = [this, mainDisplay](){
		this->removeAll(true);
		mainDisplay->switchSubscreen(NULL);
	};

	auto con = new Container(ROW_LAYOUT);
	bool dark = true;

	child(((new Button("Backspace", B_BUTTON, dark))->setAction([this](){
		if (!query.empty())
			query.pop_back();
		queryText->setText(query);
		queryText->update();
	}))->setPosition(SCREEN_WIDTH - 200, 230));

	child(((new Button("Caps", L_BUTTON, dark))->setAction([keyboard](){
		keyboard->shiftOn = !keyboard->shiftOn;
		keyboard->updateSize();
	}))->setPosition(20, 230));

	con->add(((new Button("Cancel", Y_BUTTON, dark))->setAction(cleanUp)));

	con->add(((new Button(ctaText, X_BUTTON, dark))->setAction([this, onConfirm, cleanUp](){
		onConfirm(query.c_str());
		cleanUp();
	})));

	child(promptText->setPosition(0, 15)->centerHorizontallyIn(this));
	child(con->setPosition(0, 230)->centerHorizontallyIn(this));
	child(queryText->setPosition(0, 100)->centerHorizontallyIn(this));

	hasBackground = true;
	backgroundColor = fromRGB(0x42, 0x45, 0x48);
}

void TextQueryPopup::render(Element* parent)
{
	queryText->centerHorizontallyIn(this);

	// draw a big dim layer around the entire window before drawing
	CST_Rect dim = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	auto renderer = RootDisplay::mainDisplay->renderer;

	CST_Rect dim2 = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 14 };

	CST_SetDrawBlend(renderer, true);
	CST_SetDrawColorRGBA(renderer, 0x11, 0x11, 0x11, 0xff);
	CST_FillRect(renderer, &dim2);

	super::render(this);
}

TextQueryPopup::~TextQueryPopup()
{
	this->wipeAll();
}