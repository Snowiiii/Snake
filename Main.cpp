#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

bool over;
const int width = 20;
const int height = 20;
int tailX[width], tailY[height];
int nTail;
int x, y;
int fruitX, fruitY;

int score;
int highscore;

enum Direction {
	STOP = 0, LEFT, RIGHT, UP, DOWN
};

Direction dir;

void placeFruit() {
	fruitX = rand() % width;
	fruitY = rand() % height;
}

void gameover() {
	over = true;
	if (score > highscore)
	{
		highscore = score;
	}
}

void init() {
	over = false;
	dir = STOP;
	score = 0;
	x = width / 2;
	y = height / 2;
	placeFruit();
}

void draw() {
	system("cls");

	for (int i = 0; i < width + 2; i++)	cout << "#";
	cout << endl;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (j <= 0)	cout << "#";
			if (i == y && j == x)
				cout << "O";
			else if (i == fruitY && j == fruitX)
				cout << "F";
			else
			{
				bool print = false;
				for (int k = 0; k < nTail; k++)
				{
					if (tailX[k] == j && tailY[k] == i)
					{
						cout << "o";
						print = true;
					}
				}
				if (!print)	cout << " ";
			}


			if (j == width - 1)	cout << "#";
		}
		cout << endl;
	}

	for (int i = 0; i < width + 2; i++) cout << "#";
	cout << endl;
	cout << "Score:" << score << endl;
	cout << "High Score:" << highscore << endl;
}

void input() {
	// Movement input
	if (_kbhit()) {
		switch (_getch())
		{
		case 'a':
			dir = LEFT;
			break;
		case 'd':
			dir = RIGHT;
			break;
		case 'w':
			dir = UP;
			break;
		case 's':
			dir = DOWN;
			break;
		}
	}
}

void logic() {
	// Process tail

	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;

	for (int i = 1; i < nTail; i++) {
		prev2X = tailX[i];
		prev2Y = tailY[i];
		tailX[i] = prevX;
		tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}

	// Movement render
	switch (dir)
	{
	case LEFT:
		x--;
		break;
	case RIGHT:
		x++;
		break;
	case UP:
		y--;
		break;
	case DOWN:
		y++;
		break;
	}

	// Collide with Border check
	if (x > width || x < 0 || y > height || y < 0)
	{
		gameover();
	}
	// Fruit check
	if (x == fruitX && y == fruitY)
	{
		score++;
		placeFruit();
		nTail++;
	}
}

int main() {
	init();

	while (!over) {
		draw();
		input();
		logic();
		Sleep(10);
	}
	return 0;
}