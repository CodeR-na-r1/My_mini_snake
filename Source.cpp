#include <iostream>
#include <windows.h>
#include "Snake.h"

#define WIDTH 120
#define HEIGHT 30

int main()
{
	srand(time(0));

	// setupConsole
	HANDLE hWnd = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD bufferConsole = { WIDTH, HEIGHT };
	SetConsoleScreenBufferSize(hWnd, bufferConsole);

	// create array for game field
	char** screen = new char*[HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		screen[i] = new char[WIDTH];
	}

	// draw a fence
	for (int i = 0; i < WIDTH -1; i++) { screen[0][i] = '#'; }	// begin of field
	screen[0][WIDTH - 1] = '\n';
	for (int i = 0; i < WIDTH -1; i++) { screen[HEIGHT - 1][i] = '#'; }	// end of field
	screen[HEIGHT - 1][WIDTH - 1] = '\0';

	for (int i = 1; i < HEIGHT - 1; i++)	// middle of field
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (j == 0 || j == WIDTH - 2)
				screen[i][j] = '#';
			else if (j == WIDTH - 1)
				screen[i][j] = '\n';
			else
				screen[i][j] = ' ';
		}
	}

	/* building a snake */
	list<Point> snake_body;
	for (int i = 13; i > 2; --i)
	{
		snake_body.push_back(Point(i, 1));
	}

	Snake snake(snake_body, screen, WIDTH, HEIGHT, bufferConsole, hWnd);
	snake.init();	// initialize the snake's body
	snake.show();	// show of field and snake

	int i = 0;
	Point new_point;
	while (i<100)
	{
		Sleep(400);
		new_point = snake.step();
		if (new_point.x == 0 && new_point.y == 0)
		{
			cout << char(7);
			break;
		}
		snake.show(new_point);
		++i;
	}
	
	for (int i = 0; i < HEIGHT; i++)
	{
		delete screen[i];
	}
	delete[] screen;

	getchar();

	return 0;
}