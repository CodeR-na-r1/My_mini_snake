#pragma once

#include <list>

using namespace std;

struct Point
{
	int x, y;

	Point()
	{
		x = 0;
		y = 0;
	}

	Point(int _x, int _y) : x(_x), y(_y) { }

	Point(const Point& _point)
	{
		this->x = _point.x;
		this->y = _point.y;
	}

	bool operator==(const Point& other) const
	{
		return this->x == other.x && this->y == other.y;
	}
};

class Snake
{
public:
	Snake(const list<Point>& _points, char** _screen, unsigned _width, unsigned _height, COORD& _bufferConsole, HANDLE& _hWnd)
	{
		for (auto it = _points.begin(); it != _points.end(); it++)
		{
			this->points.push_back(*it);
		}

		this->screen = _screen;
		this->width = _width;
		this->height = _height;
		this->bufferConsole = _bufferConsole;
		this->hWnd = _hWnd;
		this->is_fruit = false;
		this->is_special_fruit = false;
		this->over = false;
	}

	~Snake()
	{
		this->points.clear();
		this->screen = nullptr;
	}

	void init()
	{
		for (auto it = points.begin(); it != points.end(); it++)
		{
			this->screen[(*it).y][(*it).x] = '*';
		}

		return;
	}

	Point auto_step()
	{
		Point head(points.front().x, points.front().y);
		Point point;

		if (trand == 0 || rand() & 1)
			trand = rand() % 4;

		int count(0);
		do
		{
			if (count >= 4) { point.x = 0; point.y = 0; break; }
			point.x = head.x; point.y = head.y;

			point = this->__decode_trand(point, trand);
			if (point.x < 1 || point.x >= width || point.y < 1 || point.y >= height || screen[point.y][point.x] == '*')
			{
				trand = (++trand) % 3;
				++count;
				continue;
			}
			else
				break;
		} while (true);

		return point;
	}

	Point step(char key)
	{
		Point res(points.front().x, points.front().y);

		if (int(key) == 119 || int(key) == -26) { --res.y; }
		else if (int(key) == 100 || int(key) == -94) { ++res.x; }
		else if (int(key) == 115 || int(key) == -21) { ++res.y; }
		else if (int(key) == 97 || int(key) == -28) { --res.x; }
		else if (int(key) == 10) { over = true; }

		if (res.x < 1 || res.x >= (width - 2) || res.y < 1 || res.y >= (height - 1) || screen[res.y][res.x] == '*')
		{
			over = true;
		}

		return res;
	}

	void show()
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				std::cout << screen[i][j];
			}
		}

		return;
	}

	void show(Point new_coord_head)
	{
		// Снимаем окрас прошлой головы
		bufferConsole.X = points.front().x;	bufferConsole.Y = points.front().y;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		cout << '*';

		// Рисуем голову на то место, куда сходил пользователь
		this->points.push_front(new_coord_head);
		bufferConsole.X = points.front().x;	bufferConsole.Y = points.front().y;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		this->screen[new_coord_head.y][new_coord_head.x] = '*';
		if ((rand() % 3) & 1) { cout << "\x1b[93m"; }
		else { cout << "\x1b[91m"; }
		cout << '*' << "\x1b[0m";

		if (ordinary_fruit == new_coord_head || !is_fruit)	// Если сьели обычный фрукт, +1 к размеру змеи
		{
			is_fruit = false;
			++this->points_for_fruit;
		}

		if (special_fruit == new_coord_head)	// Если сьели особый фрукт, +5 к размеру змеи
		{
			this->points_for_fruit += 5;
			is_special_fruit = false;
		}

		if (!this->points_for_fruit)
		{
			// Стираем хвост
			bufferConsole.X = points.back().x;	bufferConsole.Y = points.back().y;
			SetConsoleCursorPosition(hWnd, bufferConsole);
			this->screen[points.back().y][points.back().x] = ' ';
			cout << ' ';
			this->points.pop_back();
		}
		else	// Если к размеру змеи причислены еще не все очки, то добавляем одно из них === хвост не трогаем
		{
			--this->points_for_fruit;
			// update score
			bufferConsole.X = width / 2 - 1;	bufferConsole.Y = 0;
			SetConsoleCursorPosition(hWnd, bufferConsole);
			cout << "\x1b[92m" << " " << points.size() << " " << "\x1b[0m";
		}

		if (!is_fruit)	// если фрукт сьеден, генерируем новый
		{
			ordinary_fruit = this->create_fruit();
			if (over) { return; }
			screen[ordinary_fruit.y][ordinary_fruit.x] = '#';
			bufferConsole.X = ordinary_fruit.x;	bufferConsole.Y = ordinary_fruit.y;
			SetConsoleCursorPosition(hWnd, bufferConsole);
			cout << "\x1b[96m" << '#' << "\x1b[0m";
			is_fruit = true;

			if (!is_special_fruit && rand() & 0x5)	// С некоторой вероятностью, генерируем особый фрукт
			{
				special_fruit = this->create_fruit();
				if (over) { return; }
				screen[special_fruit.y][special_fruit.x] = '#';
				bufferConsole.X = special_fruit.x;	bufferConsole.Y = special_fruit.y;
				SetConsoleCursorPosition(hWnd, bufferConsole);
				cout << "\x1b[91m" << '#' << "\x1b[0m";
				is_special_fruit = true;
			}
		}

		return;
	}

	Point create_fruit()
	{
		Point fruit;
		fruit.y = 1 + (rand() % (height - 2));	// Неберем элементы, где стоит забор
		fruit.x = 1 + (rand() % (width - 3));	// Неберем элементы, где стоит забор и символ '\n'

		int count = 0;
		while (screen[fruit.y][fruit.x] == '*')
		{
			++fruit.x;

			if (fruit.x >= width - 2)
			{
				fruit.x = 0;
				++fruit.y;
			}

			if (fruit.y >= height - 1)
				fruit.y = 0;

			if (count < width * height)
				++count;
			else
			{
				over = true;
				break;
			}
		}

		return fruit;
	}

	int get_score()
	{
		return points.size();
	}

	bool is_fruit;
	bool is_special_fruit;
	bool over;

private:
	list<Point> points;
	Point ordinary_fruit, special_fruit;
	char** screen;
	unsigned width;
	unsigned height;
	short points_for_fruit;
	int trand;
	COORD bufferConsole;
	HANDLE hWnd;

	Point& __decode_trand(Point& point, int& trand)
	{
		if (trand == 0) { --point.y; return point; }
		if (trand == 1) { ++point.x; return point; }
		if (trand == 2) { ++point.y; return point; }
		if (trand == 3) { --point.x; return point; }

		return point;
	}
};