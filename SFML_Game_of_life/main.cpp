#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime> 

const int SquareSize = 10;
const int Padding = 2;

const int SizeX = 130;
const int SizeY = 85;

const int WindowWidth = SizeX * SquareSize + (SizeX - 1) * Padding;
const int WindowWHeight = SizeY * SquareSize + (SizeY - 1) * Padding;

int DelayTime = 50;

bool map[SizeX][SizeY] = { false };
bool tmp_map[SizeX][SizeY];

sf::RectangleShape rectangles[SizeX][SizeY];

enum State {ADD, REMOVE, IDLE} state;

void DrawScene(sf::RenderWindow& w) {
	w.clear();
	for (int x = 0; x < SizeX; x++) {
		for (int y = 0; y < SizeY; y++) {
			if (map[x][y] == true) {
				w.draw(rectangles[x][y]);
			}
		}

	}
	w.display();
}

void SetObject(sf::RenderWindow& w, State s) {
	sf::Vector2i pos = sf::Mouse::getPosition(w);
	int x = pos.x / (SquareSize + Padding);
	int y = pos.y / (SquareSize + Padding);

	if (s == ADD)
		map[x][y] = tmp_map[x][y] = true;
	else if (s == REMOVE)
		map[x][y] = tmp_map[x][y] = false;

	DrawScene(w);
}

void Init() {	
	for (int x = 0; x < SizeX; x++) {
		for (int y = 0; y < SizeY; y++) {
			rectangles[x][y].setSize(sf::Vector2f(SquareSize, SquareSize));
			rectangles[x][y].setPosition(x * (SquareSize + Padding), y * (SquareSize + Padding));
		}
	}
	state = IDLE;
}

int CountNeighbours(int x, int y) {
	int cnt = 0;
	if (x > 0)
		if (map[x - 1][y]) cnt++;

	if (x < (SizeX-1))
		if (map[x + 1][y]) cnt++;

	if (y > 0) 
		if (map[x][y-1]) cnt++;

	if (y < (SizeY-1))
		if (map[x][y + 1]) cnt++;

	if (x > 0 && y > 0) 
		if (map[x - 1][y - 1]) cnt++;

	if (x < (SizeX - 1) && (y < (SizeY - 1)))
		if (map[x + 1][y + 1]) cnt++;

	if (x > 0 && (y < (SizeY - 1)))
		if (map[x - 1][y + 1]) cnt++;

	if (x < (SizeX - 1) && y > 0)
		if (map[x + 1][y - 1]) cnt++;

	return cnt;
}

void Simulate() {	
	for (int x = 0; x < SizeX; x++) {
		for (int y = 0; y < SizeY; y++) {
			switch (CountNeighbours(x, y)) {
			case 0:
			case 1:
				tmp_map[x][y] = false; // dies - underpopulation
				break;
			case 2:
				break;
			case 3:
				tmp_map[x][y] = true; // is born
				break;
			default:
				tmp_map[x][y] = false; // dies - overpopulation
				break;
			}
		}
	}
	std::memcpy(map, tmp_map, sizeof(map[0][0]) * SizeX * SizeY);
}

void Clear(sf::RenderWindow& w) {
	for (int x = 0; x < SizeX; x++) {
		for (int y = 0; y < SizeY; y++) {
			map[x][y] = tmp_map[x][y] = false;
		}
	}
	DrawScene(w);
	DelayTime = 50;
}

void Random(sf::RenderWindow& w) {
	bool prev = false;
	srand(time(NULL));
	for (int x = 0; x < SizeX; x++) {
		for (int y = 0; y < SizeY; y++) {
			if (prev) {				
				map[x][y] = (rand() % 100) > 35;
			} else {
				map[x][y] = (rand() % 100) > 90;
			}
			prev = map[x][y];
		}
	}
	DrawScene(w);
}
void GenerateHelpWindow(sf::RenderWindow& w) {
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		std::cerr << "Can't load font file" << std::endl;
	}
	sf::Text helpText;
	helpText.setFont(font);
	helpText.setCharacterSize(16);
	helpText.setColor(sf::Color::White);
	helpText.setStyle(sf::Text::Bold);
	helpText.setPosition(10, 10);

	helpText.setString("Space - start / stop\nS - single step\nC - Clear\nR - random\nUp - Speed up\nDown - Slow down\nH - Help\Esc - Quit");

	w.create(sf::VideoMode(200, 200), "Help", sf::Style::Close);
	w.clear();
	w.draw(helpText);
	w.display();
}

int main()
{ 
	sf::RenderWindow windowMain(sf::VideoMode(WindowWidth, WindowWHeight), "Game of Life", sf::Style::Close);
	sf::RenderWindow windowHelp;

	Init();
	windowMain.clear();
	windowMain.display();

	bool start = false;
	bool singleStep = false;
	bool helpWindowShow = false;

	while (windowMain.isOpen()) {		
		sf::Event event;
		while (windowMain.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				windowMain.close();
			}	
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					windowMain.close();
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Space) {
					if (start == false) 
						start = true;
					else 
						start = false;
				}
				else if (event.key.code == sf::Keyboard::S) {
					singleStep = true;
				}
				else if (event.key.code == sf::Keyboard::C) {
					start = false;
					Clear(windowMain);
				}
				else if (event.key.code == sf::Keyboard::R) {
					Random(windowMain);
				}
				else if (event.key.code == sf::Keyboard::Up) {
					if (DelayTime >= 10) {
						DelayTime -= 5;
					}				
				}
				else if (event.key.code == sf::Keyboard::Down) {
					DelayTime += 5;
				}
				else if (event.key.code == sf::Keyboard::H) {
					if (helpWindowShow == false) {
						helpWindowShow = true;		
						GenerateHelpWindow(windowHelp);
					} else {
						helpWindowShow = false;		
						windowHelp.close();
					}

					
				}
			}
			if (start == false) {
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						state = ADD;
						SetObject(windowMain, ADD);
					}
					if (event.mouseButton.button == sf::Mouse::Right)
					{
						SetObject(windowMain, REMOVE);
						state = REMOVE;
					}
				}
				if (event.type == sf::Event::MouseMoved) {
					if (state == ADD) {
						SetObject(windowMain, ADD);
					}
					else if (state == REMOVE) {
						SetObject(windowMain, REMOVE);
					}
				}
				if (event.type == sf::Event::MouseButtonReleased) {
					if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)
					{
						state = IDLE;
					}
				}
			}
			
		}

		if (helpWindowShow) {
			while (windowHelp.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					helpWindowShow = false;
					windowHelp.close();
				}
				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Escape) {
						helpWindowShow = false;
						windowHelp.close();
					}
				}
			}			
		}

		if (start || singleStep) {

			// Delay
			sf::Clock clk;
			sf::Time t1;
			clk.restart();
			do {
				t1 = clk.getElapsedTime();
			} while (t1.asMilliseconds() < DelayTime);

			Simulate();
			DrawScene(windowMain);
			singleStep = false;
		}		
	}

	return 0;
}