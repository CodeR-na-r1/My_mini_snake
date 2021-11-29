#include <iostream>
#include <conio.h>
#include <windows.h>
#include <thread>
#include <clocale>
#include "Snake.h"

#define WIDTH 120
#define HEIGHT 30

int main()
{
	setlocale(LC_ALL, "ru");
	srand(time(0));

	// setupConsole
	HANDLE hWnd = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD bufferConsole = { WIDTH, HEIGHT };	// Фиксированное окно консоли
	SetConsoleScreenBufferSize(hWnd, bufferConsole);	// Применяем

	CONSOLE_CURSOR_INFO curs = { 0 };
	curs.dwSize = sizeof(curs);
	curs.bVisible = FALSE;	// Скрыть курсор консоли
	SetConsoleCursorInfo(hWnd, &curs);	// Применяем

	DWORD dwMode = 0;
	GetConsoleMode(hWnd, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;	// Для корректного отборжаниея виртуальных ‎‎последовательностей консоли
	dwMode |= DISABLE_NEWLINE_AUTO_RETURN;	// Для корректной смены позиционирования каретки консоли
	SetConsoleMode(hWnd, dwMode);	// Применяем

	// create array for game field
	char** screen = new char* [HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		screen[i] = new char[WIDTH];
	}

	// draw a fence
	// [][WIDTH - 1] = '\n'
	// [][WIDTH - 2] = '#'
	// [][0] = '#'
	for (int i = 0; i < WIDTH - 1; i++) { screen[0][i] = '#'; }	// begin of field
	screen[0][WIDTH - 1] = '\n';
	for (int i = 0; i < WIDTH - 1; i++) { screen[HEIGHT - 1][i] = '#'; }	// end of field
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

	Point new_point;	// Точка, куда сдвинется змея после хода пользователя
	char key = 'd';	// Клавиша хода пользователя
	bool flag_key, flag_end = false;	// Флаг, что пользователь нажал на клавишу, и флаг завершения игры.
	thread key_in([&key, &flag_end, &flag_key]() { while (!flag_end) { key = _getch(); flag_key = true; } });	// Поток, чекающий ввод пользователя
	while (true)
	{
		int i = 0;
		while (!flag_key && i < 50) { Sleep(10); ++i; }	// Ожидание основным потоком хода пользователя по таймеру (около +-500 миллисекунд), при вводе пользователем хода, цикл прерывается

		new_point = snake.step(key);	// Выисляем точку, где окаэеться голова и проверяем, что ход не проигрышный
		snake.show(new_point);	// Сдвиг змеи, и прорисовка нового фрукта, если он был съеден
		if (snake.over)	// Если ход проигрышный, звуковой сигнал и выход из цикла
		{
			cout << char(7);
			break;
		}
		flag_key = false;	//	Сброс флага (отвечающего за наличие ввода клавиши пользователем)
	}

	/* Стираем поле: верхняя и нижняя границы поля */
	for (int i = 0; i < WIDTH; i++)
	{
		bufferConsole.X = i;	bufferConsole.Y = 0;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		cout << ' ';
		Sleep(1);
		bufferConsole.X = WIDTH - 1 - i;	bufferConsole.Y = HEIGHT - 1;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		cout << ' ';
	}

	/* Стираем поле: левая и правая границы поля */
	for (int i = 0; i < HEIGHT; i++)
	{
		bufferConsole.X = WIDTH - 2;	bufferConsole.Y = i;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		cout << ' ';
		Sleep(1);
		bufferConsole.X = 0;	bufferConsole.Y = HEIGHT - 1 - i;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		cout << ' ';
	}

	/* Стираем поле: середина поля */
	bufferConsole.X = 0;	bufferConsole.Y = 1;
	SetConsoleCursorPosition(hWnd, bufferConsole);
	for (int i = 1; i < HEIGHT - 1; i++)
	{
		for (int j = 0; j < WIDTH - 2; j++)
		{
			cout << ' ';
		}
		cout << endl;
	}

	/* Чистим память */
	for (int i = 0; i < HEIGHT; i++)
	{
		delete screen[i];
	}
	delete[] screen;

	/* Вывод результатов */
	bufferConsole.X = 0;	bufferConsole.Y = 0;
	SetConsoleCursorPosition(hWnd, bufferConsole);
	cout << "\x1b[91mGame over!\x1b[0m" << endl << "Scrore: \x1b[92m" << snake.get_score() << "\x1b[0m" << endl;

	cout << "for exit - press any key" << endl;
	flag_end = true;
	key_in.join();	// Ввод значения для выхода и за одно поток считывает значение и завершает свою работу

	return 0;
}