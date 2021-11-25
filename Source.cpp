#include <iostream>
#include <conio.h>
#include <windows.h>
#include <thread>
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

	Point new_point;
	char key = 'd';
	bool flag_key, flag_end = false;
	thread key_in([&key, &flag_end, &flag_key]() { while (!flag_end) { key = _getch(); flag_key = true; } });
	while (true)
	{
		int i = 0;
		while (!flag_key && i < 50) { Sleep(10); ++i; }

		new_point = snake.step(key);
		snake.show(new_point);
		if (snake.over)
		{
			cout << char(7);
			break;
		}
		flag_key = false;
	}

	flag_end = true;
	key_in.join();
	
	bufferConsole.X = 0;	bufferConsole.Y = 0;
	SetConsoleCursorPosition(hWnd, bufferConsole);
	for (int i = 0; i < HEIGHT; i++)	// Clear of field on console
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == 0 || i == HEIGHT - 1)
				Sleep(1);
			cout << ' ';
		}
		cout << endl;
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		delete screen[i];
	}
	delete[] screen;

	bufferConsole.X = 0;	bufferConsole.Y = 0;
	SetConsoleCursorPosition(hWnd, bufferConsole);
	cout << "\x1b[91mGame over!\x1b[0m" << endl << "Scrore: \x1b[92m" << snake.get_score() << "\x1b[0m" << endl;

	return 0;
}