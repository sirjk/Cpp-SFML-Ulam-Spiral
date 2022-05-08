#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <sstream>
#include "Tilemap.h"

using namespace sf;

bool isNumberPrime(int);

int main() {

	//two variables (int rows, int columns) would be necessary if we had tilemap of irregular size
	//but making ulams spiral we want a square so its just one variable
	int SIZE = 316;

	//after declaring Clock object the time is being measured
	float dt = 0.f;
	Clock dtClock;

	//all window stuff
	RenderWindow window(VideoMode(1920, 1080), "Prime Numbers");
	window.setFramerateLimit(60);
	View view;
	view.setSize(1920.f, 1080.f);
	view.setCenter(16*SIZE/2, 16*SIZE/2); //window.getSize().x / 2.f, window.getSize().y / 2.f
	float viewSpeed = 15.f;

	//all text stuff
	Text text1;
	Font font;
	font.loadFromFile("sansation.ttf");
	text1.setCharacterSize(65);
	text1.setFillColor(Color::Green);
	text1.setFont(font);
	text1.setPosition(20.f, 20.f);

	Text text2;
	font.loadFromFile("sansation.ttf");
	text2.setCharacterSize(25);
	text2.setFillColor(Color::Green);
	text2.setFont(font);
	text2.setPosition(20.f, window.getSize().y - 200);

	//all mouse stuff
	Vector2i mousePosWindow;
	Vector2f mousePosView;

	//with such large data as hundreads of thousands we need to allocate it on the heap
	//because stack memory is to small

	//level is array representation of textures location,
	//values which the array stores are specific tiles of texture in the tileset file
	int** level = (int**)malloc(SIZE * sizeof(int*));
	for (int i = 0; i < SIZE; i++)
		level[i] = (int*)malloc(SIZE * sizeof(int));

	//as default it is set up to white tiles (0)
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			level[i][j] = 0;
		}
	}

	//this array will store following numbers in spiral order starting from the center
	int** valMap = (int**)malloc(SIZE * sizeof(int*));
	for (int i = 0; i < SIZE; i++)
		valMap[i] = (int*)malloc(SIZE * sizeof(int));

	//while moving in spiral-like order throughout the square
	//we have odd numberes of moves for up and right direction
	//and even numbers of moves for left and down direction
	int movesRight=1, movesUp=1, movesLeft=2, movesDown=2;

	//store current position for level and valMap
	int xPos, yPos;

	//we set them up to the center
	//and it depends on if the size of square is even or odd
	if (SIZE % 2 == 0) {
		xPos = SIZE / 2 - 1;
		yPos = SIZE / 2;
	}
	else {
		xPos = SIZE / 2;
		yPos = SIZE / 2;
	}

	int values = 1;

	//this loop is responsible for setting up the level (depending on if the current number is prime or not)
	//and valMap
	while (movesRight <= SIZE && movesUp <= SIZE) {

		for (int i = 0; i < movesRight; i++) {
			if (values == 1) {
				level[yPos][xPos] = 1;
			}
			if (isNumberPrime(values)) {
				level[yPos][xPos] = 2;	
			}
			valMap[yPos][xPos] = values;
			xPos += 1;
			values++;
			if (values > SIZE * SIZE)
				break;
		}
		if (movesUp == SIZE)
			break;
		for (int i = 0; i < movesUp; i++) {
			if (isNumberPrime(values)) {
				level[yPos][xPos] = 2;	
			}
			valMap[yPos][xPos] = values;
			yPos -= 1;
			values++;
			if (values > SIZE * SIZE)
				break;
		}
		for (int i = 0; i < movesLeft; i++) {
			if (isNumberPrime(values)) {
				level[yPos][xPos] = 2;	
			}

			valMap[yPos][xPos] = values;
			xPos -= 1;
			values++;
			if (values > SIZE * SIZE)
				break;
		}
		for (int i = 0; i < movesDown; i++) {
			if (isNumberPrime(values)) {
				level[yPos][xPos] = 2;	
			}
			valMap[yPos][xPos] = values;
			yPos += 1;
			values++;
			if (values > SIZE * SIZE)
				break;
		}
		movesRight += 2;
		movesUp += 2;
		movesLeft += 2;
		movesDown += 2;
	}
	
	//factors requiers calibration,
	//setting their values affects moving comfort
	float zoomFactor1;
	float zoomFactor2 = 0;
	float gridFactor = 0;

	//for grid
	bool g = false;

	//creating Tilemap object
	Tilemap map;

	//checking if it was loaded properly
	//load function gets tileset file name, size of these tiles, 2d array with tile representation
	//and width and height of the level map
	if (!map.load("tileset16.png", Vector2u(16, 16), level, SIZE, SIZE))
			return -1;

	while (window.isOpen()) {

		zoomFactor1 = 0;

		dt = dtClock.restart().asMilliseconds();
		mousePosWindow = Mouse::getPosition(window);

		window.setView(view);

		mousePosView = window.mapPixelToCoords(mousePosWindow);

		window.setView(window.getDefaultView());
		
		//string to display,
		//in this case it will be the number on which the cursor is pointing
		//and manual
		std::stringstream number;
		std::stringstream manual;

		if (mousePosView.x <= SIZE * 16 && mousePosView.y <= SIZE * 16 && mousePosView.x >=0 && mousePosView.y >= 0) {
			number << valMap[(int)mousePosView.y / 16][(int)mousePosView.x / 16] << "\n";
			if (!isNumberPrime(valMap[(int)mousePosView.y / 16][(int)mousePosView.x / 16])) {
				text1.setFillColor(Color::Red);
			}
			else
				text1.setFillColor(Color::Green);
			text1.setString(number.str());
		}
		else {
			number << "";
			text1.setString(number.str());
		}

		manual << "navigate with arrows" << "\n"
			<< "Q - zoom in" << "\n"
			<< "E - zoom out" << "\n"
			<< "G - grid" << "\n"
			<< "C - center" << "\n";
		text2.setString(manual.str());

		Event event;
		while (window.pollEvent(event)) {

			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				// update the view to the new size of the window
				FloatRect view(0.f, 0.f, event.size.width, event.size.height);
				window.setView(sf::View(view));
			}
			//zooming with mouse wheel
			//not changing moves speed for the moment
			if (event.type == Event::MouseWheelMoved) {
				if (event.mouseWheel.delta > 0) {
					view.zoom(0.5f);
				}
				else if (event.mouseWheel.delta < 0) {
					view.zoom(1.5f);
				}
			}
		}
		//commented below are instructions which doesnt
		//change the speed of movements depending on zoom

		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			//view.move(-viewSpeed, 0.f);
			view.move(-viewSpeed - zoomFactor2, 0.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			//view.move(viewSpeed, 0.f);
			view.move(viewSpeed + zoomFactor2, 0.f);
		}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			//view.move(0.f, -viewSpeed);
			view.move(0.f, -viewSpeed - zoomFactor2);
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			//view.move(0.f, viewSpeed);
			view.move(0.f, viewSpeed + zoomFactor2);
		}
		if (Keyboard::isKeyPressed(Keyboard::Q)) {  //zoom in
			zoomFactor1 += 0.004;
			if(zoomFactor1 > 0) {
				view.zoom(0.99 + zoomFactor1);
				zoomFactor1 -= 0.004;
			}
			if(viewSpeed + zoomFactor2 > 0.5)
				zoomFactor2 -= 0.055;
		}
		if (Keyboard::isKeyPressed(Keyboard::E)) {  //zoom out
			zoomFactor1 -= 0.004;
			if (zoomFactor1 < 0) {
				view.zoom(1.01 - zoomFactor1);
				zoomFactor1 += 0.004;
			}
			//if(viewSpeed - zoomFactor2 < 19.5)
				zoomFactor2 += 0.055;
		}
		if (Keyboard::isKeyPressed(Keyboard::G)) {
			gridFactor += dt;
			if (gridFactor >= 90) {
				g = !g;
				if (g) {
					if (!map.load("grid_tileset16.png", Vector2u(16, 16), level, SIZE, SIZE))
						return -1;
				}
				else {
					if (!map.load("tileset16.png", Vector2u(16, 16), level, SIZE, SIZE))
						return -1;
				}
				gridFactor = 0;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::C)) {
			view.setCenter(16*SIZE/2, 16*SIZE/2);
		}

		window.clear();
		window.setView(view);

		window.draw(map);
		window.setView(window.getDefaultView());

		window.draw(text1);
		window.draw(text2);
		window.display();
	}

	//freeing the allocated memory
	for (int i = 0; i < SIZE; i++) {
		 free(level[i]);
	}
	for (int i = 0; i < SIZE; i++) {
		free(valMap[i]);
	}
	free(valMap);
	free(level);

	return 0;
}

bool isNumberPrime(int num) {
	if (num == 1)
		return false;
	int dividers = 0;
	for (int i = 1; i <= num; i++) {
		if (num % i == 0)
			dividers++;
		if (dividers > 2)
			return false;
	}
	return true;
}