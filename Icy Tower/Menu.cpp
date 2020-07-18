#include "Menu.h"


Menu::Menu(float width, float height, int Num, string ChoicesStr[])
{
	//w = width;
	//	h = height;
	if (!font.loadFromFile("KGHAPPYSolid.ttf"))
	{
		// handle error
	}
	this->Num = Num;
	NumOfChoices = Num;
	Choices = new Text[NumOfChoices];


	NumOfChoices = Num;
	Choices = new Text[NumOfChoices];

	for (int i = 0; i < NumOfChoices; i++)
	{
		Choices[i].setFont(font);
		Choices[i].setFillColor(i == 0 ? Color(138, 43, 226) : Color(255, 140, 0));
		Choices[i].setString(ChoicesStr[i]);
		Choices[i].setCharacterSize(40);
		Choices[i].setOutlineThickness(3);
		Choices[i].setPosition(sf::Vector2f(width / 11, height / (15 - 3) *(i + 6)));
	}
}


Menu::~Menu()
{
}

void Menu::draw(sf::RenderWindow &window, Texture& _back_texter)
{
	RectangleShape back_;
	back_.setSize(Vector2f(600, 600));
	back_.setTexture(&_back_texter);
	window.draw(back_);
	for (int i = 0; i < Num; i++)
	{
		window.draw(Choices[i]);
	}
}

void Menu::MoveUp()
{
	if (selectedItemIndex - 1 >= 0)
	{
		Choices[selectedItemIndex].setFillColor(sf::Color(255, 140, 0));
		selectedItemIndex--;
		Choices[selectedItemIndex].setFillColor(sf::Color(138, 43, 226));
	}
}

void Menu::MoveDown()
{
	if (selectedItemIndex + 1 < Num)
	{
		Choices[selectedItemIndex].setFillColor(sf::Color(255, 140, 0));
		selectedItemIndex++;
		Choices[selectedItemIndex].setFillColor(sf::Color(138, 43, 226));
	}
}