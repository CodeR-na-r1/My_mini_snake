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

	Point step()
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
		// Снимаем окрас 'старой' головы
		bufferConsole.X = points.front().x;	bufferConsole.Y = points.front().y;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		cout << '*';

		// Добавляем новую голову
		this->points.push_front(new_coord_head);
		bufferConsole.X = points.front().x;	bufferConsole.Y = points.front().y;
		SetConsoleCursorPosition(hWnd, bufferConsole);
		this->screen[new_coord_head.y][new_coord_head.x] = '*';
		if ((rand() % 3) & 1) { cout << "\x1b[93m"; } else { cout << "\x1b[91m"; }
		cout << '*' << "\x1b[0m";

		if (fruit == new_coord_head || !is_fruit)
			this->is_fruit = false;

		if (this->is_fruit)
		{
			// Удаляем хвост
			bufferConsole.X = points.back().x;	bufferConsole.Y = points.back().y;
			SetConsoleCursorPosition(hWnd, bufferConsole);
			this->screen[points.back().y][points.back().x] = ' ';
			cout << ' ';
			this->points.pop_back();
		}
		else	// Если фрукт сьели, то хвост не трогаем и генерируем новый фрукт
		{
			this->create_fruit();
			if (over) { return; }
			screen[fruit.y][fruit.x] = '#';
			bufferConsole.X = fruit.x;	bufferConsole.Y = fruit.y;
			SetConsoleCursorPosition(hWnd, bufferConsole);
			cout << "\x1b[93m" << '#' << "\x1b[0m";
		}
		
		return;
	}

	void create_fruit()
	{
		fruit.y = 1 + rand() % height;
		fruit.x = 1 + rand() % width;

		int count = 0;
		while (screen[fruit.y][fruit.x] =='*')
		{
			++fruit.x;

			if (fruit.x >= width)
			{
				fruit.x = 0;
				++fruit.y;
			}

			if (fruit.y >= height)
				fruit.y = 0;

			if (count < width * height)
				++count;
			else
			{
				over = true;
				break;
			}
		}

		is_fruit = true;

		return;
	}

	bool is_fruit;

private:
	list<Point> points;
	Point fruit;
	char** screen;
	unsigned width;
	unsigned height;
	int trand;
	bool over;
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