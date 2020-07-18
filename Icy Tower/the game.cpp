#include <SFML/Graphics.hpp>
#include<SFML\Audio.hpp>
#include <iostream>
#include <time.h>
#include <thread>
#include <string.h>
#include <fstream>
#include <sstream>
#include "Menu.h"
using namespace std;
using namespace sf;
Vector2f Posetion(190, 500);
float x = 0;
float y = 0;
int bonus = 0;
sf::Vector2f pos(320, 500);
sf::Vector2f vel(0, 0);   // player velocity (per frame)
sf::Vector2f gravity(0, .5f);   // gravity (per frame)
const float maxfall = 5;   // max fall velocity
const float runacc = .25f;  // run acceleratio
const float maxrun = 2.5f;   // max run velocity
const float jumpacc = -1;   // jump acceleration
const unsigned int jumpframes = 10;   // number of frames to accelerate in
unsigned int jumpcounter = 0;    // counts the number of frames where you can still accelerate
Vector2i bodysize(47, 55);
Sprite sbackground, sleftborder, srightborder, hurryup, trara;     //for background , charcter ,left border& right border
Texture icy_man;  //for charcter
bool jump = false;
int scoree = 0;
int score = 0;
bool min5 = false;
bool EDGE = false;
Vector2f sd(640, 600);       //for window
RectangleShape con;
Clock last;
bool side_menu = false;
int compo;
int highscore1 = 0;
int highscoree;
String t1;
Texture tbackground, tleftborder, trightborder, tmountanes;
Texture floortex, tstep2, tstep3;
string Choices[] = { "Play", "How to play", "Developer", "Exit" };
struct location
{
	int x, y, w, h;
}floorlocation[1000];
//PlayerAnimation  class 
class Animation
{
private:
	Vector2u ImageCount; //noumber of rows and columns of image
	Vector2u CurrentImage; //the index of the accessed image
	float TotalTime, SwitchTime; //switch time:is the time taken to switch between two textures
public:
	IntRect uv_Rect; //we use uv_rect to crop one texture properly
	Animation(Texture* character, Vector2u ImageCount, float SwitchTime)
	{
		this->ImageCount = ImageCount; //we use keyword "this" to differentiate between the private variable and the parameter
		this->SwitchTime = SwitchTime; //we use keyword "this" to differentiate between the private variable and the parameter
		TotalTime = 0; //we initialize the total time of PlayerAnimation  to zero
		CurrentImage.x = 0; //we access to the start point of the texture
		uv_Rect.width = character->getSize().x / float(ImageCount.x); //calculate the width of a single texture
		uv_Rect.height = character->getSize().y / float(ImageCount.y); //calculate the height of a single texture
	}
	void update(int row, float deltaTime, bool face_right) //It is the function used to update the PlayerAnimation  of the character in each frame
	{
		CurrentImage.y = row; //we initialize the current image on the y-axis to the index of the row to get the suitable texture for each specific movement
		TotalTime += deltaTime; //we add the value of delta time to the current value of total time
		if (TotalTime >= SwitchTime)
		{
			TotalTime -= SwitchTime; //we subtract the value of switch time from the Total time  
			CurrentImage.x++; //we access to the next texture after the switch is done
			if (CurrentImage.x >= ImageCount.x) //when we reach the last point of the row , we return back to the start point 
			{
				CurrentImage.x = 0;
			}
		}
		if (face_right) //if the user presses left key,the image will be flipped
		{
			uv_Rect.left = CurrentImage.x * uv_Rect.width;
			uv_Rect.width = abs(uv_Rect.width);
		}
		else
		{
			uv_Rect.left = (CurrentImage.x + 1) * abs(uv_Rect.width);
			uv_Rect.width = -abs(uv_Rect.width);
		}
		uv_Rect.top = CurrentImage.y * uv_Rect.height;
	}
};
//Player class
class Player
{
private:
	Animation  PlayerAnimation; //Creating object
	unsigned int row; //there is no index in negative
	bool face_right;
	float speed;
public:
	//bool left = false, right = false;  //right and left here represent the event of pressing right and left keys
	Player(Texture*character, Vector2u ImageCount, float SwitchTime, float speed) : //Note:we add the speed
		PlayerAnimation(character, ImageCount, SwitchTime) //initializer list for object's PlayerAnimation 
	{
		this->speed = speed;
		face_right = true;
		body.setSize(Vector2f(bodysize.x, bodysize.y));

		body.setTexture(character);
	}
	void update(float deltaTime, RenderWindow& window, int w, int x) //We used a window here to handle events 
	{
		// inputs
		bool left = false;
		bool right = false;
		bool jump = false;
		Event event;
		Vector2f movement(0, 0);
		//Moving
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			movement.x -= speed * deltaTime; //to let the speed increase every frame in the left direction
			left = true;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			Posetion.x += 10;
			movement.x += speed * deltaTime; //to let the speed increase every frame in the right direction
			right = true;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Space))) //jumping without pressing any other key
		{
			jump = true;
			row = 3;

		}
		else if (Keyboard::isKeyPressed(Keyboard::Space)) //jumping with pressing left or right key
		{
			jump = true;
			row = 2;
			if (movement.x > 0)
				face_right = true;
			else
				face_right = false;
		}
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::KeyReleased)
			{
				if (event.key.code == Keyboard::Left)
					left = false;
				if (event.key.code == Keyboard::Right)
					right = false;
				if (event.key.code == Keyboard::Space)
					jump = false;
			}
		}
		if (movement.x == 0)  //we access to row 0 when the character isn't moving
		{
			row = 0; //ideal position of the character
		}
		else
		{
			row = 1; //we access to row 1 when the character is moving
			if (movement.x > 0)
				face_right = true; //the character is moving rightwards
			else
				face_right = false; //the character is moving leftwards
		}
		// logic update start
		// first, apply velocities
		pos += vel;
		// determine whether the player is on the ground
		const bool onground = pos.y >= 500;
		// now update the velocity by...
		// ...updating gravity
		vel += gravity;
		// ...capping gravity
		if (vel.y > maxfall)
			vel.y = maxfall;
		if (left) // running to the left
		{
			vel.x -= runacc;
		}
		else if (right)// running to the right
		{
			vel.x += runacc;
		}
		else // not running anymore; slowing down each frame
		{
			vel.x *= 0.3;
		}
		// jumping
		if (jump)
		{
			if (left)
			{
				face_right = false;
				row = 2;
			}
			if (right)
			{
				face_right = true;
				row = 2;
			}
			if (right == false && left == false)
			{
				row = 3;
			}
			if (onground)
			{ // on the ground
				vel.y += jumpacc;
				jumpcounter = jumpframes;
			}
			else if (jumpcounter > 0) { // first few frames in the air
				vel.y += jumpacc;
				jumpcounter--;
			}
		}
		else // jump key released, stop acceleration
		{
			jumpcounter = 0;
		}
		// check for collision with the left border
		if (pos.x < 71)
		{
			face_right = true;
			vel.x = -0.3*vel.x;
			pos.x = 71;
		}
		else if (pos.x > 530)
		{
			face_right = false;
			vel.x = -0.4*vel.x;
			pos.x = 530;
		}
		// logic update end
		// update the position
		body.setPosition(pos);

		if ((vel.x >= 10 || vel.x <= -10) && jumpcounter >= 1)
		{
			vel.y -= 1;
			//while_rotating.play();

			compo = last.getElapsedTime().asMilliseconds()*last.getElapsedTime().asMilliseconds() / 10000;
			bonus += compo;
			body.rotate(18);
			row = 5;

		}
		else if ((vel.x >= 7 && vel.x<10 || vel.x <= -7 && vel.x>-10) && jumpcounter >= 1)
		{
			vel.y -= 1;

			//high_jump.play();
			//while_rotating.pause();
			//while_rotating.setVolume(0);
		}
		else
		{
			body.setRotation(0);

		}
		PlayerAnimation.update(row, deltaTime, face_right);//calling update function from PlayerAnimation  object
		body.setTextureRect(PlayerAnimation.uv_Rect);
		//  for collision with steps
		if (vel.y >= 0)
		{
			for (int i = 0; i < 1000; i++)
			{
				if ((pos.x + 0.5 *bodysize.x > floorlocation[i].x)
					&& (pos.x - bodysize.x < floorlocation[i].x + floorlocation[i].w)
					&& (pos.y + 0.5 *bodysize.y > floorlocation[i].y)
					&& (pos.y + 0.5*bodysize.y < floorlocation[i].y + floorlocation[i].h))
				{

					pos.x = pos.x;
					vel.y = 0;
					pos.y = floorlocation[i].y - 20;
					scoree = i * 10;
					last.restart();

					if (Keyboard::isKeyPressed(Keyboard::Space))
					{
						vel.y += jumpacc * 4;
						jumpcounter = jumpframes;

						if (jumpcounter > 0)  // first few frames in the air
						{
							vel.y += jumpacc;
							jumpcounter--;
						}
					}
				}
			}
		}
	}
	Vector2f GetPosition()
	{
		return body.getPosition();
	}
	RectangleShape body;
};
void Play();
void MainMenu();
void Option();
void PlayMenu();
void HowToPlay();
void PoseMenu();
void GameOver();
void Soud();
void Developers();
int main()
{
	MainMenu();
}
void MainMenu()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML WORK!");
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu menu(window.getSize().x, window.getSize().y, Num, Choices);
	Texture bbb;
	bbb.loadFromFile("images/menu back grund 3D.png");
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					menu.MoveUp();
					break;
				case sf::Keyboard::Down:
					menu.MoveDown();
					break;
				case sf::Keyboard::Return:
					switch (menu.GetPressedItem())
					{
					case 0:
					{
						window.close();
						PlayMenu();
						break;
					}
					case 1:
					{
						window.close();
						HowToPlay();
						break;
					}
					case 2:
					{
						window.close();
						Developers();
						break;
					}
					case 3:
					{
						window.close();
						break;
					}
					}
					break;
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		menu.draw(window, bbb);
		window.display();
	}
}
void Play()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "icy tower Game");
	srand(time(0));
	View view(FloatRect(0, 0, sd.x, sd.y)); //to move screen
	View view2(FloatRect(0, 0, sd.x, sd.y)); //to fixed things
	window.setView(view);
	// all the texture	
	Texture tstep0, ColckTextuer, tbackground2, thurryup, tmountanes, tgameover;    //for the first four steps
	tstep0.loadFromFile("images/step00.png");     //load the fourth steps
	ColckTextuer.loadFromFile("images/final awee.png");   //load the cclock texter 
	tbackground2.loadFromFile("images/back_grond night.png");   //load background
	thurryup.loadFromFile("images/hurry up.png");  //to load the word hurry up
	tgameover.loadFromFile("images/game-background_23-2148080814.png");
	tbackground.setRepeated(true);   //for repeating background
	tbackground2.setRepeated(true);   //for repeating background
	trightborder.setRepeated(true); //for repeated right border
	tleftborder.setRepeated(true);//for repeated left border
	//texture for charcter
	Vector2u T_size = icy_man.getSize();
	Player player1(&icy_man, Vector2u(4, 6), 0.4, 300); //creating an object from player class 
	float deltaTime = 0; //initialization,delta time is a variable which makes the program runs on different devices with the same speed
	//std::fstream file;
	//std::string highS;
	//all the sprites and rectangel shape
	RectangleShape floor, step2, step3, ColckRectangl;
	floor.setTexture(&floortex);
	step2.setTexture(&tstep2);
	step3.setTexture(&tstep3);
	//clock PlayerAnimation 
	Animation  ColckAnimationOpject(&ColckTextuer, Vector2u(12, 1), 2.5f);
	ColckRectangl.setTexture(&ColckTextuer);
	ColckRectangl.setSize(Vector2f(60, 60));
	ColckRectangl.setPosition(0, 0);
	con.setSize(Vector2f(1, 1));
	RectangleShape hur, vie, vie2, vie3;
	vie.setSize(Vector2f(640, .01));
	vie2.setSize(Vector2f(640, 0.01));
	vie3.setSize(Vector2f(640, 0.01));
	hur.setSize(Vector2f(640, 0.01));
	vie.setPosition(0, 30);
	vie2.setPosition(0, 660);
	vie3.setPosition(0, 680);
	hur.setPosition(300, 250);
	//sprites
	Sprite mimo5(tmountanes), sbackground2, sgameove;
	mimo5.setPosition(0, 600 - 330);
	//step2.setFillColor(Color::Red);
	//sbackground.setColor(Color::Black);
	sbackground2.setTexture(tbackground2);   //to load background in sprite
	sbackground.setTexture(tbackground);   //to load background in sprite
	srightborder.setTexture(trightborder); //load right border in sprite
	sleftborder.setTexture(tleftborder); //load left border in sprite
	sgameove.setTexture(tgameover);
	sleftborder.setPosition(0, 482 - 127); //Make its starting point at the bottom left of the screen
	srightborder.setPosition(640 - 74, 482 - 127);//Make its starting point at the bottom right of the screen
	sgameove.setPosition(100, 200);
	//all fonts
	Font font1;
	font1.loadFromFile("MATURASC.ttf");
	//all texts
	Text text1, text2, text3, text4, text5, text6, text8;
	//text1
	text1.setFont(font1);  // to draw word "hurry up"
	text1.setString("hurry up");  //to load the word hurry up
	text1.setCharacterSize(100); // in pixels, not points!
	text1.setFillColor(sf::Color::Yellow); //to set the color of the word hurry up
	//text1.setStyle(sf::Text::Bold | sf::Text::Underlined);  //set the tybe of the word hurry up
	//text2
	text2.setFont(font1);
	text2.setCharacterSize(30); // in pixels, not points!
	text2.setFillColor(sf::Color::Black); //to set the color of the word hurry up
	//text2.setStyle(sf::Text::Bold | sf::Text::Underlined);  //set the tybe of the word hurry up
	//text3
	text3.setFont(font1);
	text3.setCharacterSize(50); // in pixels, not points!
	text3.setFillColor(sf::Color::Blue); //to set the color of the word hurry up
	//text3.setStyle(sf::Text::Bold | sf::Text::Underlined);
	text3.setPosition(10, 530);
	text3.setString("Score : ");
	//text4
	text4.setFont(font1);
	text4.setCharacterSize(50); // in pixels, not points!
	text4.setFillColor(sf::Color::Blue); //to set the color of the word hurry up
	//text4.setStyle(sf::Text::Bold | sf::Text::Underlined);
	text4.setPosition(200, 530);
	text5.setFont(font1);  // to draw word "hurry up"
	text5.setString("Alright");  //to load the word hurry up
	text5.setCharacterSize(100); // in pixels, not points!
	text5.setFillColor(sf::Color::Yellow); //to set the color of the word hurry up
	text6.setFont(font1);  // to draw word "hurry up"
	text6.setString("sweet");  //to load the word hurry up
	text6.setCharacterSize(100); // in pixels, not points!
	text6.setFillColor(sf::Color::Yellow); //to set the color of the word hurry up
	text8.setFont(font1);  // to draw word "hurry up"
	text8.setString("GAME OVER");  //to load the word hurry up
	text8.setCharacterSize(60); // in pixels, not points!
	text8.setFillColor(sf::Color::Red); //to set the color of the word hurry u
	text8.setPosition(50, 100);
	hurryup.setTexture(thurryup);
	//all times and clocks
	Clock clock;
	Clock c, clock3, z; //Clock2
	//setting positions of each step randomly
	for (int i = 0; i < 1000; i++)
	{
		floorlocation[i].h = 37;
		if (i << 200)
		{
			floorlocation[i].w = rand() % 218;
			if (floorlocation[i].w <= 145)
				floorlocation[i].w = 145;
			//else if (floorlocation[i].w>130 && floorlocation[i].w <= 145)
			//	floorlocation[i].w = 145;
			else if (floorlocation[i].w > 145 && floorlocation[i].w <= 160)
				floorlocation[i].w = 160;
			else if (floorlocation[i].w > 160 && floorlocation[i].w <= 185)
				floorlocation[i].w = 185;
			else
				floorlocation[i].w = 218;
		}
		if (i > 200 && i < 300)
		{
			if (floorlocation[i].w <= 110)
				floorlocation[i].w = 110;
			else if (floorlocation[i].w>110 && floorlocation[i].w <= 130)
				floorlocation[i].w = 130;
			else
				floorlocation[i].w = 160;
		}
		if (i > 300)
			floorlocation[i].w = 130;
		if (i == 0 || (i % 50 == 0))
			floorlocation[i].w = 640;
		floorlocation[i].x = rand() % 550;
		if (i == 0)
		{
			floorlocation[i].x = 0;
			floorlocation[i].y = 550;
		}
		else
			floorlocation[i].y = floorlocation[i - 1].y - 100;
	}
	// handil the position of steps
	for (int i = 1; i < 1000; i++)
	{
		if (floorlocation[i].x < 75)
			floorlocation[i].x = 75;
		if ((floorlocation[i].x + floorlocation[i].w)> 564)
			floorlocation[i].x = (564 - floorlocation[i].w);
		if (i % 50 == 0)
			floorlocation[i].x = 0;
	}
	FloatRect fBounds(0.f, 0.f, sd.x, sd.y); // arbitrary > view height
	IntRect iBounds(fBounds);        //for repeating background
	int vy = -2;   //velocity of view of y axis
	int y = 1;   //for repeating background
	int h = 100000;   //variable for repeating backgrond upward
	//Creating the window
	//Setting the framerate limit to 60 FPS
	window.setFramerateLimit(80);
	//set fonts 
	//for numbers of steps
	//for score
	int num = 0;
	string t2;
	int o = 0;
	int a = 0;
	//*******************************************************
	//sound
	//Setting Sound Buffers
	sf::SoundBuffer w_jump;
	if (w_jump.loadFromFile("Data/while jumping male.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer k;
	if (k.loadFromFile("Data/Ok.wav") == false)
	{
		//code here
	}

	sf::SoundBuffer highScore;
	if (highScore.loadFromFile("Data/High Score.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer cool;
	if (cool.loadFromFile("Data/Yo.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer lose;
	if (lose.loadFromFile("Data/Game Over.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer hiJump;
	if (hiJump.loadFromFile("Data/high jump.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer heyGood;
	if (heyGood.loadFromFile("Data/hey good.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer edge;
	if (edge.loadFromFile("Data/edge.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer hurry_up;
	if (hurry_up.loadFromFile("Data/hurry up.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer whatsup;
	if (whatsup.loadFromFile("Data/what's up.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer hittingGround;
	if (hittingGround.loadFromFile("Data/hitting the ground.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer sweet;
	if (sweet.loadFromFile("Data/sweet.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer thatsGreat;
	if (thatsGreat.loadFromFile("Data/great.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer tryAgain;
	if (tryAgain.loadFromFile("Data/Try Again.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer crazy;
	if (crazy.loadFromFile("Data/crazy.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer IT_menu;
	if (IT_menu.loadFromFile("Data/Icy Tower Menu.wav") == false)
	{
		//code here
	}
	//Setting sounds' objects
	sf::Sound ok;
	ok.setBuffer(k);
	sf::Sound celebrate;
	celebrate.setBuffer(highScore);
	sf::Sound Yo;
	Yo.setBuffer(cool);
	Yo.play();
	sf::Sound while_jumping;
	while_jumping.setBuffer(w_jump);
	//while_jumping.play();
	sf::Sound game_over;
	game_over.setBuffer(lose);
	sf::Sound high_jump;
	high_jump.setBuffer(hiJump);
	sf::Sound good;
	good.setBuffer(heyGood);
	sf::Sound onEdge;
	onEdge.setBuffer(edge);
	sf::Sound h_up;
	h_up.setBuffer(hurry_up);
	sf::Sound wassap;
	wassap.setBuffer(whatsup);
	sf::Sound hit_g;
	hit_g.setBuffer(hittingGround);
	hit_g.play();
	sf::Sound sw;
	sw.setBuffer(sweet);
	sw.play();
	sf::Sound great;
	great.setBuffer(thatsGreat);
	great.play();
	/*sf::Sound sel;
	sel.setBuffer(select);*/
	sf::Sound t_again;
	t_again.setBuffer(tryAgain);
	sf::Sound while_rotating;
	while_rotating.setBuffer(crazy);
	sf::Sound menu;
	menu.setBuffer(IT_menu);
	menu.setLoop(true);
	while (window.isOpen())
	{
		Event all;

		window.setView(view);
		{
			player1.body.setOrigin(bodysize.x / 2, bodysize.y / 2);
			if (EDGE)
			{
				onEdge.play();
			}
			if (player1.body.getGlobalBounds().intersects(srightborder.getGlobalBounds()))
			{
				vel.x = -.5*vel.x;
				pos.x = 530;
			}

			if (player1.body.getGlobalBounds().intersects(sleftborder.getGlobalBounds()))
			{
				vel.x = -.5*vel.x;
				pos.x = 71 + 25;
			}
			if ((vel.x >= 10 || vel.x <= -10) && jumpcounter >= 1)
			{
				while_rotating.play();
			}

			if (Keyboard::isKeyPressed(Keyboard::Space) && vel.y<7 && vel.y>-7)
				while_jumping.play();

		}
		Event Pose;
		while (window.pollEvent(Pose))
		{
			if (Pose.type == Event::Closed)
			{
				window.close();

			}
			if (Pose.type == Event::KeyReleased)
			{
				if (Pose.key.code == Keyboard::Escape)
					PoseMenu();

			}
		}
		y += 10 * sd.y;
		deltaTime = c.restart().asSeconds(); //clock restarts time at each frame
		sbackground.setPosition(fBounds.left, fBounds.top - 1000.f - h); //for repeating background upward
		sbackground2.setPosition(fBounds.left, fBounds.top - 1000.f - h); //for repeating background upward
		sleftborder.setPosition(fBounds.left, fBounds.top - 1000.f - h); //for repeating left border upward
		srightborder.setPosition(fBounds.left + (640 - 74), fBounds.top - 1000.f - h); //for repeating right border upward
		sbackground.setTextureRect(IntRect(0, 0, sd.x, y));  //repeat background
		sbackground2.setTextureRect(IntRect(0, 0, sd.x, y));  //repeat background
		sleftborder.setTextureRect(IntRect(0, 0, 68, y));    //repeat left border
		srightborder.setTextureRect(IntRect(0, 0, 74, y));   //repeat right border
		sf::Event event;  //the event
		// increase the value of repeating
		// check all the window's events that were triggered since the last iteration of the loop
		if (player1.body.getPosition().y < 10)
		{
			ColckAnimationOpject.update(0, deltaTime, true);   // updat the clock PlayerAnimation  by calling update function from clock PlayerAnimation  object
			ColckRectangl.setTextureRect(ColckAnimationOpject.uv_Rect);    //to crup the tectuer of the clock
		}
		player1.update(deltaTime, window, floorlocation[a].w, floorlocation[a].x); // updat the character PlayerAnimation  by calling update function from player1 object
		menu.play();
		t1 = to_string(scoree + bonus);
		text4.setString(t1);
		//set view 1
		window.setView(view);
		window.draw(sbackground);
		window.draw(mimo5);
		for (int i = 0; i < 1000; i++)
		{
			floor.setSize(Vector2f(floorlocation[i].w, floorlocation[i].h));
			step2.setSize(Vector2f(floorlocation[i].w, floorlocation[i].h));
			step3.setSize(Vector2f(floorlocation[i].w, floorlocation[i].h));
			floor.setPosition(floorlocation[i].x, floorlocation[i].y);
			step2.setPosition(floorlocation[i].x, floorlocation[i].y);
			step3.setPosition(floorlocation[i].x, floorlocation[i].y);


			if (i < 100)
				window.draw(step2);
			else if (i >= 100 && i < 200)
				window.draw(step2);
			else
				window.draw(step2);
		}

		window.draw(vie);
		window.draw(vie2);
		window.draw(vie3);
		window.draw(player1.body); //draw charcter
		window.draw(sleftborder);  //draw the left border
		window.draw(srightborder);  //draw the right border
		if (player1.body.getPosition().y < 10)
		{

			view.move(0, vy);
			vie.move(0, vy);
			vie2.move(0, vy);
			vie3.move(0, vy);

			if (clock.getElapsedTime().asSeconds() >= 30)  //increasse velocity of view every 30 seconds
			{
				vy--;
				clock.restart();
				h_up.play();
			}
		}

		if (clock3.getElapsedTime().asSeconds() >= 35)
			clock3.restart();

		if (pos.y > 10)
		{
			clock.restart();
			clock3.restart();
		}

		for (int i = 0; i < 1000; i++)
		{
			if (i % 10 == 0)
			{
				num = i;
				t2 = to_string(num);
				text2.setString(t2);
				text2.setPosition(floorlocation[i].x + 0.5*floorlocation[i].w - 20, floorlocation[i].y);
				window.draw(text2);
			}
		}
		window.setView(view2);

		window.draw(vie3);
		if (clock3.getElapsedTime().asSeconds() >= 30)
		{
			text1.setPosition(100, 400);
			window.draw(text1);

		}
		if (vy <= -6)
		{
			clock.restart();
			clock3.getElapsedTime();

		}
		for (int i = 1; i < 1000; i++)
		{
			if (i % 50 == 0)
			{

				if (pos.y <= floorlocation[i].y&&pos.y >= floorlocation[i + 10].y)
				{
					text5.setPosition(100, 300);
					window.draw(text5);
					ok.play();
				}
			}

		}
		if (compo >= 2 && compo < 4)
		{
			text6.setPosition(100, 250);
			window.draw(text6);
			sw.play();
			compo = 0;
		}
		else if (compo >= 4 && compo < 7)
			great.play();
		if (player1.body.getGlobalBounds().intersects(vie2.getGlobalBounds()))
		{
			cout << "game over" << endl;
			game_over.play();
			//	newscore(window);
			window.close();
			GameOver();
		}
		window.draw(text4);
		window.draw(text3);

		window.draw(ColckRectangl);   //drowing the clock texter

		if (player1.body.getGlobalBounds().intersects(vie.getGlobalBounds()))
		{
			view.move(0, vel.y);
			vie.move(0, vel.y);
			vie2.move(0, vel.y);
			vie3.move(0, vel.y);
		}

		window.display();
	}
}
void PlayMenu()
{

	sf::RenderWindow window1(sf::VideoMode(600, 600), "SFML WORK!");
	string Choices[] = { "Theme 1", "Theme 2 ", "Theme 3", "Back" };
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu option(window1.getSize().x, window1.getSize().y, Num, Choices);

	Texture bbb;
	bbb.loadFromFile("images/menu back grund 3D.png");
	while (window1.isOpen())
	{

		sf::Event event;

		while (window1.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					option.MoveUp();
					break;

				case sf::Keyboard::Down:
					option.MoveDown();
					break;
				case sf::Keyboard::M:
					break;
				case sf::Keyboard::Return:
					switch (option.GetPressedItem())
					{
					case 0:
					{
						window1.close();
						icy_man.loadFromFile("images/oregenal.png"); //load the charcter
						tbackground.loadFromFile("images/blue gameBack.png");   //load background
						trightborder.loadFromFile("images/red wall.png");// load right border
						tleftborder.loadFromFile("images/red wall.png");//load left border
						floortex.loadFromFile("images/platform.png");    //load the first step
						tstep2.loadFromFile("images/platform.png");     //load the second steps
						tstep3.loadFromFile("images/step3.png");     //load the third steps
						tmountanes.loadFromFile("images/mimo5.png");
						Play();
						break;
					}
					case 1:
					{
						window1.close();
						icy_man.loadFromFile("images/cape character.png"); //load the charcter
						tbackground.loadFromFile("images/back grond.1.png");   //load background
						trightborder.loadFromFile("images/right border1.png");// load right border
						tleftborder.loadFromFile("images/right border1.png");//load left border
						floortex.loadFromFile("images/platform.png");    //load the first step
						tstep2.loadFromFile("images/platform.png");     //load the second steps
						tstep3.loadFromFile("images/step3.png");     //load the third steps
						;

						Play();
						break;
					}
					case 2:
					{
						window1.close();
						icy_man.loadFromFile("images/black boy.png"); //load the charcter
						tbackground.loadFromFile("images/back_grond sunset.png");   //load background
						trightborder.loadFromFile("images/right birder3.png");// load right border
						tleftborder.loadFromFile("images/left border2.png");//load left border
						floortex.loadFromFile("images/pool.png");    //load the first step
						tstep2.loadFromFile("images/pool.png");     //load the second steps
						tstep3.loadFromFile("images/pool.png");     //load the third steps
						Play();
						break;
					}
					case 3:
					{
						window1.close();
						MainMenu();
					}
					}

					break;
				}
				break;
			case sf::Event::Closed:
				window1.close();
				break;
			}
		}
		window1.clear();
		option.draw(window1, bbb);
		window1.display();
	}


}
void HowToPlay()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML WORK!");
	string Choices[] = { "Back" };
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu menu(window.getSize().x, window.getSize().y, Num, Choices);
	Texture bbb;
	bbb.loadFromFile("images/back grond.final.png");
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{

			switch (event.type)
			{

			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					menu.MoveUp();
					break;

				case sf::Keyboard::Down:
					menu.MoveDown();
					break;
				case sf::Keyboard::Return:
					switch (menu.GetPressedItem())
					{
					case 0:
					{
						window.close();
						MainMenu();
						break;
					}
					}
					break;
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		menu.draw(window, bbb);
		window.display();
	}
	window.close();
}
void Option()
{
	sf::RenderWindow window1(sf::VideoMode(600, 600), "SFML WORK!");
	string Choices[] = { "back" };
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu option(window1.getSize().x, window1.getSize().y, Num, Choices);
	Texture bbb;
	bbb.loadFromFile("images/menu back grund 3D.png");
	while (window1.isOpen())
	{
		sf::Event event;

		while (window1.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					option.MoveUp();
					break;

				case sf::Keyboard::Down:
					option.MoveDown();
					break;

				case sf::Keyboard::Return:
					switch (option.GetPressedItem())
					{
					case 0:
					{
						window1.close();
						MainMenu();
						break;
					}
					}
					break;
				}
				break;
			case sf::Event::Closed:
				window1.close();
			}
		}

		window1.clear();

		option.draw(window1, bbb);

		window1.display();
	}
}
void PoseMenu()
{
	sf::RenderWindow window1(sf::VideoMode(600, 600), "SFML WORK!");
	sf::SoundBuffer IT_menu;
	if (IT_menu.loadFromFile("Data/Icy Tower Menu.wav") == false)
	{
		//code here
	}
	sf::SoundBuffer whatsup;
	if (whatsup.loadFromFile("Data/what's up.wav") == false)
	{
		//code here
	}
	sf::Sound wassap;
	wassap.setBuffer(whatsup);
	wassap.play();
	string Choices[] = { "contenue", "Restwrt", "Main Menu" };
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu option(window1.getSize().x, window1.getSize().y, Num, Choices);
	Texture bbb;
	bbb.loadFromFile("images/menu back grund 3D.png");
	while (window1.isOpen())
	{
		sf::Event event;

		while (window1.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					option.MoveUp();
					break;

				case sf::Keyboard::Down:
					option.MoveDown();
					break;

				case sf::Keyboard::Return:
					switch (option.GetPressedItem())
					{
					case 0:
					{
						window1.close();
						break;
					}
					case 1:
					{
						window1.close();
						PlayMenu();
						break;
					}
					case 2:
					{
						window1.close();
						MainMenu();
						break;
					}
					}
					break;
				}
				break;
			case sf::Event::Closed:
				window1.close();
			}
		}

		window1.clear();

		option.draw(window1, bbb);

		window1.display();
	}

}
void GameOver()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML WORK!");

	sf::SoundBuffer tryAgain;

	if (tryAgain.loadFromFile("Data/Try Again.wav") == false)
	{
		//code here
	}
	sf::Sound t_again;
	t_again.setBuffer(tryAgain);
	string Choices[] = { "Main Menu" };
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu option(50, 900, Num, Choices);
	Texture bbb;
	bbb.loadFromFile("images/gameover_biue _back_ground.png");
	std::fstream file;
	std::string highS;
	Text text9, text10, text8, text11, text12;
	Font font1;
	font1.loadFromFile("MATURASC.ttf");
	text9.setFont(font1);  // to draw word "hurry up"
	text9.setCharacterSize(60); // in pixels, not points!
	text9.setFillColor(sf::Color::Red); //to set the color of the word hurry u
	text9.setPosition(50, 200);

	text10.setFont(font1);  // to draw word "hurry up"
	text10.setCharacterSize(60); // in pixels, not points!
	text10.setFillColor(sf::Color::Red); //to set the color of the word hurry u
	text10.setPosition(50, 300);

	text11.setFont(font1);  // to draw word "hurry up"
	text11.setCharacterSize(60); // in pixels, not points!
	text11.setFillColor(sf::Color::Red); //to set the color of the word hurry u
	text11.setPosition(400, 200);

	text12.setFont(font1);  // to draw word "hurry up"
	text12.setCharacterSize(60); // in pixels, not points!
	text12.setFillColor(sf::Color::Red); //to set the color of the word hurry u
	text12.setPosition(400, 300);
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					option.MoveUp();
					break;

				case sf::Keyboard::Down:
					option.MoveDown();
					break;

				case sf::Keyboard::Return:
					switch (option.GetPressedItem())
					{

					case 0:
					{
						window.close();
						t_again.play();
						MainMenu();

						break;

					}
					}
					break;
				}
				break;
			case sf::Event::Closed:
				window.close();
			}
			file.open("hadi.txt", std::ios::in);
			std::getline(file, highS);
			file.close();
			highscoree = std::stoi(highS);
			if ((scoree + bonus) > highscoree)
			{

				highscoree = (scoree + bonus);
				file.open("hadi.txt", std::ios::out);
				file << highscoree << endl;

				file.close();
			}
			cout << highscoree;
			text9.setString("score : ");

			text10.setString("high score : ");

			text11.setString(t1);

			text12.setString(highS);

		}

		window.clear();
		option.draw(window, bbb);
		window.draw(text11);
		window.draw(text12);
		window.draw(text10);
		window.draw(text9);
		window.draw(text8);


		window.display();
	}
}
void Developers()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML WORK!");
	string Choices[] = { "Back" };
	int Num = sizeof(Choices) / sizeof(Choices[0]);
	Menu menu(window.getSize().x, 10, Num, Choices);
	Texture bbb;
	bbb.loadFromFile("images/dev.png");
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{

			switch (event.type)
			{

			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					menu.MoveUp();
					break;

				case sf::Keyboard::Down:
					menu.MoveDown();
					break;
				case sf::Keyboard::Return:
					switch (menu.GetPressedItem())
					{
					case 0:
					{
						window.close();
						MainMenu();
						break;
					}
					}
					break;
				}
				break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		window.clear();
		menu.draw(window, bbb);
		window.display();
	}
	window.close();
}