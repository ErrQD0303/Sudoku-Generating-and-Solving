#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <conio.h>

class cell {
private:
	int x;
	int y;
public:
	cell(int _x, int _y) : x(_x), y(_y) {}
	bool operator==(const cell& _cell) {
		if (this->x == _cell.x && this->y == _cell.y)
			return true;
		return false;
	}

	bool operator>(const cell& _cell) {
		if (this->x > _cell.x || (this->x == _cell.x && this->y > _cell.y))
			return true;
		return false;
	}

	bool operator<=(const cell& _cell) {
		return !(this > &_cell);
	}

	bool operator<(const cell& _cell) {
		if (this->x < _cell.x || (this->x == _cell.x && this->y < _cell.y))
			return true;
		return false;
	}

	bool operator>= (const cell& _cell) {
		return !(this < &_cell);
	}
	
	int getx() {
		return this->x;
	}

	int gety() {
		return this->y;
	}
};

void readFile(int S[9][9]) {
	std::ifstream ifs("SudokuBoard.txt");
	for (int j = 0; j < 9; ++j)
		for (int i = 0; i < 9; ++i)
			ifs >> S[i][j];
	ifs.close();
}

void writeFile(int S[9][9]) {
	std::ofstream ofs("SudokuResult.txt");
	for (int j = 0; j < 9; ++j) {
		for (int i = 0; i < 9; ++i){
			ofs << S[i][j];
			if (i != 8 || j != 8)
				ofs << " ";
		}
		if (j < 8)
			ofs << std::endl;
	}
	ofs.close();
}

bool check_Valid(int S[9][9], int x, int y, int a) {
	for (int i = 0; i < 9; ++i)
		if (S[i][y] == a)
			return false;
	for (int i = 0; i < 9; ++i)
		if (S[x][i] == a)
			return false;
	int c = (x / 3) * 3;
	int r = (y / 3) * 3;
	for (int i = c; i < c + 3; ++i)
		for (int j = r; j < r + 3; ++j)
			if (S[i][j] == a)
				return false;
	return true;
}

// Using BackTracking Algorithm to solve Sudoku Board
void solve_Sudoku(int S[9][9], int x, int y) {
	if (y == 9)
		if (x == 8)
			throw "Solving Sudoku Complete!";
		else
			solve_Sudoku(S, x + 1, 0);
	else if (S[x][y] != 0)
		solve_Sudoku(S, x, y + 1);
	else
		for (int i = 1; i <= 9; ++i) 
			if (check_Valid(S, x, y, i) == true) {
				S[x][y] = i;
				solve_Sudoku(S, x, y + 1);
				S[x][y] = 0;
			}
}

bool solve_Sudoku1(int S[9][9], int x, int y) {
	if (y == 9)
		if (x == 8)
			return true;
		else {
			if (solve_Sudoku1(S, x + 1, 0) == true)
				return true;
		}
	else if (S[x][y] != 0) {
		if (solve_Sudoku1(S, x, y + 1) == true)
			return true;
	}
	else
		for (int i = 1; i <= 9; ++i)
			if (check_Valid(S, x, y, i) == true) {
				S[x][y] = i;
				if (solve_Sudoku1(S, x, y + 1) == true)
					return true;
				S[x][y] = 0;
			}
	return false;
}

bool generating_Sudoku(int S[9][9], int i, int j, int num) {
	if (j == 3)
		if (i == 2)
			if (num == 9)
				return true;
			else {
				if (generating_Sudoku(S, 0, 0, num + 1) == true)
					return true;
			}
		else {
			if (generating_Sudoku(S, i + 1, 0, num) == true)
				return true;
		}
	else {
		std::vector<cell> v;
		for (int x = 0; x < 3; ++x)
			for (int y = 0; y < 3; ++y) {
				cell _cell(3 * i + x, 3 * j + y);
				v.push_back(_cell);
			}
		for (int x = 0; x < 3; ++x) {
			for (int y = 0; y < 3 * j; ++y) {
				if (S[3 * i + x][y] == num) {
					auto k = v.begin();
					while (k != v.end() && k->getx() != 3 * i + x)
						++k;
					if (k != v.end())
						v.erase(k, k + 3);
					break;
				}
			}
		}
		for (int y = 0; y < 3; ++y)
			for (int x = 0; x < 3 * i; ++x) {
				if (S[x][3 * j + y] == num) {
					for (auto k = v.begin(); k != v.end(); ++k) {
						if (k->gety() == 3*j+y) {
							v.erase(k);
							k = v.begin();
							continue;
						}
					}
					break;
				}
			}
		std::vector<cell>::iterator k;
		do {
			srand(time(NULL));
			int rd;
			if (v.size() > 1)
				rd = rand() % (v.size() - 1);
			else
				rd = 0;
			int count = 0;
			for (k = v.begin(); count != rd; ++k, ++count);
			if (S[k->getx()][k->gety()] != 0) {
				v.erase(k);
				continue;
			}
			S[k->getx()][k->gety()] = num;
			if (generating_Sudoku(S, i, j + 1, num) == true)
				return true;
			S[k->getx()][k->gety()] = 0;
			v.erase(k);
		} while (v.size() != 0);		
	}
	return false;
}

void removeSudokuCell(int S[9][9], std::string str) {
	int num;
	if (str == "Easy")
		num = 28;
	else if (str == "Medium")
		num = 33;
	else if (str == "Hard")
		num = 38;
	else if (str == "Expert")
		num = 43;
	else
		num = 48;
	std::vector<cell> v;
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9;++j) {
			cell _cell(i, j);
			v.push_back(_cell);
		}
	int rd, count;
	while (num != 0) {
		count = 0;
		srand(time(NULL));
		rd = rand() % (v.size() - 1);
		auto k = v.begin();
		while (count != rd) {
			++k, ++count;
		}
		//for (k = v.begin(); count != rd; ++k, ++count);
		S[k->getx()][k->gety()] = 0;
		v.erase(k);
		--num;
	}
}

void printSudokuBoard(int S[9][9]) {
	for (int j = 0; j < 9; ++j) {
		for (int i = 0; i < 9; ++i)
			std::cout << S[i][j] << " ";
		if (j < 8)
			std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::setfill('-') << std::setw(40) << "-" << std::endl;
}

int main() {
	int S[9][9] = { 0 };
	generating_Sudoku(S, 0, 0, 1);
	std::cout << "Generating Sudoku Complete!" << std::endl;;
	printSudokuBoard(S);
	//readFile(S);
	/*try {
		solve_Sudoku(S, 0, 0);
	}
	catch (const char* msg) {
		std::cout << msg << std::endl;
	}*/
	std::cout << "Please Choosing Sudoku Difficulty: (Easy, Medium, Hard, Expert, Evil)\n";
	std::string str;
	getline(std::cin, str, '\n');
	removeSudokuCell(S, str);
	std::cout << "Remove Sudoku Cell!" << std::endl;
	printSudokuBoard(S);
	std::cout << "Press any thing to Print the Sudoku result: ";
	char ch;
	ch = _getch();
	std::cout << std::endl;
	solve_Sudoku1(S, 0, 0);
	std::cout << "Solving Sudoku Complete\n";
	printSudokuBoard(S);
	writeFile(S);
	return 0;
}