#include "TextQueryPopup.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Constraint.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/EKeyboard.hpp"

using namespace Chesto;

TextQueryPopup::TextQueryPopup(const char* prompt, const char* ctaText, std::function<void(const char*)> onConfirm)
{
	this->width = SCREEN_WIDTH;
	this->height = SCREEN_HEIGHT;

	this->x = 0;
	this->y = 0;
	this->isAbsolute = true;

	auto renderer = (RootDisplay::mainDisplay)->renderer;

	// draw a big dim layer around the entire window before drawing this progress bar
	CST_Rect dim = { 0, 0, 1280, 720 };

	CST_SetDrawBlend(renderer, true);
	CST_SetDrawColorRGBA(renderer, 0x00, 0x00, 0x00, 0xbb);
	CST_FillRect(renderer, &dim);

	auto mainDisplay = RootDisplay::mainDisplay;

	auto promptTextPtr = std::make_unique<TextElement>(prompt, 22);
	auto promptText = promptTextPtr.get();

	auto queryTextPtr = std::make_unique<TextElement>("", 28);
	queryText = queryTextPtr.get();

	auto keyboardPtr = std::make_unique<EKeyboard>([this](char input)
		{
		query += input;
		queryText->setText(query);
		queryText->update(); });
	auto keyboard = keyboardPtr.get();
	keyboard->preventEnterAndTab = true;

#if defined(_3DS) || defined(_3DS_MOCK)
	keyboard->width = SCREEN_WIDTH - 140;
	keyboard->position(40, 360);
#endif

	keyboard->updateSize();
	addNode(std::move(keyboardPtr));

	std::function<void()> cleanUp = [this, mainDisplay]()
	{
		this->removeAll();
		RootDisplay::popScreen();
	};

	auto conPtr = std::make_unique<Container>(ROW_LAYOUT);
	auto con = conPtr.get();
	bool dark = true;

	auto backspaceBtn = std::make_unique<Button>("Backspace", B_BUTTON, dark);
	backspaceBtn->setAction([this]()
		{
		if (!query.empty())
			query.pop_back();
		queryText->setText(query);
		queryText->update(); });
	backspaceBtn->setPosition(SCREEN_WIDTH - 200, 230);
	addNode(std::move(backspaceBtn));

	auto capsBtn = std::make_unique<Button>("Caps", L_BUTTON, dark);
	capsBtn->setAction([keyboard]()
		{
		keyboard->shiftOn = !keyboard->shiftOn;
		keyboard->updateSize(); });
	capsBtn->setPosition(20, 230);
	addNode(std::move(capsBtn));

	con->createNode<Button>("Cancel", Y_BUTTON, dark)->setAction([cleanUp]()
		{ RootDisplay::deferAction(cleanUp); });

	con->createNode<Button>(ctaText, X_BUTTON, dark)->setAction([this, onConfirm, cleanUp]()
		{
		onConfirm(query.c_str());
		RootDisplay::deferAction(cleanUp); });

	promptText->setPosition(0, 15);
	promptText->constrain(ALIGN_CENTER_HORIZONTAL);
	addNode(std::move(promptTextPtr));

	con->setPosition(0, 230);
	con->constrain(ALIGN_CENTER_HORIZONTAL);
	addNode(std::move(conPtr));

	queryText->setPosition(0, 100);
	queryText->constrain(ALIGN_CENTER_HORIZONTAL);
	addNode(std::move(queryTextPtr));

	hasBackground = true;
	backgroundColor = fromRGB(0x42, 0x45, 0x48);
}

void TextQueryPopup::render(Element* parent)
{
	queryText->constrain(ALIGN_CENTER_HORIZONTAL);

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
}