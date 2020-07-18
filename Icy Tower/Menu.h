#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>

#define MAX_NUMBER_OF_ITEMS 4
using namespace std;
using namespace sf;
class Menu
{
public:
	Menu(float width, float height, int Num, string ChoicesStr[]);
	~Menu();

	void draw(sf::RenderWindow &window, Texture &_back_texter);
	void MoveUp();
	void MoveDown();
	int GetPressedItem() { return selectedItemIndex; }
	sf::Text menu_text[MAX_NUMBER_OF_ITEMS];
private:
	int selectedItemIndex;
	sf::Font font;
	int       NumOfChoices;
	sf::Text *Choices;
	int Num;
	float w;
	float h;
};
