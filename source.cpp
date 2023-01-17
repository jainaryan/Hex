#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <sstream>
#include <random>
#include <map>
#include <numeric>

using namespace std;

class Node {

public:
	Node* neighbours[6];
	int player = -1; //0 for first player and 1 for second player
	int x, y;
	Node(int x, int y) {
		for (int i = 0; i < 6; i++) {
			neighbours[i] = nullptr;
		}
		this->x = x;
		this->y = y;
	}
	void setNeighbour(int index, Node* node) {
		neighbours[index] = node;
	}

	int getPlayer() const {
		return player;
	}

	void setPlayer(int player) {
		this->player = player;
	}

};

struct byX
{
	bool operator () (const Node* lhs, const Node* rhs) const
	{
		return lhs->x < rhs->x;
	}
};

struct byY
{
	bool operator () (const Node* lhs, const Node* rhs) const
	{
		return lhs->y < rhs->y;
	}
};

class Hexboard {
	Node*** board = nullptr;

	int size;

public:
	Hexboard(int n) {
		size = n;
		board = new Node * *[n];
		for (int i = 0; i < n; i++) {
			board[i] = new Node * [n];
			for (int j = 0; j < n; j++) {
				board[i][j] = new Node(i, j);
			}
		}
		int ni, nj;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				ni = i;
				nj = j - 1;

				if ((ni > -1 && ni < n) && (nj > -1 && nj < n)) {
					board[i][j]->setNeighbour(0, board[ni][nj]);
				}

				ni = i + 1;
				nj = j - 1;

				if ((ni > -1 && ni < n) && (nj > -1 && nj < n)) {
					board[i][j]->setNeighbour(1, board[ni][nj]);
				}

				ni = i + 1;
				nj = j;

				if ((ni > -1 && ni < n) && (nj > -1 && nj < n)) {
					board[i][j]->setNeighbour(2, board[ni][nj]);
				}

				ni = i;
				nj = j + 1;

				if ((ni > -1 && ni < n) && (nj > -1 && nj < n)) {
					board[i][j]->setNeighbour(3, board[ni][nj]);
				}

				ni = i - 1;
				nj = j + 1;

				if ((ni > -1 && ni < n) && (nj > -1 && nj < n)) {
					board[i][j]->setNeighbour(4, board[ni][nj]);
				}

				ni = i - 1;
				nj = j;

				if ((ni > -1 && ni < n) && (nj > -1 && nj < n)) {
					board[i][j]->setNeighbour(5, board[ni][nj]);
				}

			}
		}
	}
	set <Node*> getConnectedNodes(Node* node, set <Node*> connected) {
		if (connected.find(node) != connected.end()) {
			return connected;
		}

		connected.insert(node);
		for (int i = 0; i < 6; i++) {
			if (node->neighbours[i] && node->neighbours[i]->player == node->player) {
				connected = getConnectedNodes(node->neighbours[i], connected);

			}
		}
		return connected;
	}

	Hexboard* copy(int n) {
		Hexboard* clone = new Hexboard(n);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				clone->board[i][j]->player = this->board[i][j]->player;
			}
		}
		return clone;
	}

	//returns the first node moved
	Node* randomize(int playerturn) {
		vector<Node*> empty;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (this->board[i][j]->player == -1) {
					empty.push_back(board[i][j]);
				}
			}
		}
		srand(time(NULL));

		//shuffle(empty.begin(), empty.end(), default_random_engine());
		random_shuffle(empty.begin(), empty.end());
		int half = empty.size() / 2;
		for (int i = 0; i < half; i++) {
			empty[i]->player = playerturn;
		}
		int nextplayer = 1 - playerturn;
		for (int i = half; i < empty.size(); i++) {
			empty[i]->player = nextplayer;
		}
		/*int a, b;
		for (int i = 0; i < half; i++) {
			a = empty[i]->x;
			b = empty[i]->y;
			if (isLegal(a, b)) {
				return empty[i];
			}
		}
		*/
		return empty[0];

	}




	bool isOver(int x, int y, bool horizontal) {
		set <Node*> a;
		set <Node*> visited = getConnectedNodes(board[x][y], a);

		if (horizontal) {
			set<Node*, byY> ordered(visited.begin(), visited.end());
			return (*ordered.begin())->y == 0 && (*ordered.rbegin())->y == this->size - 1;
		}

		else {
			set<Node*, byX> ordered(visited.begin(), visited.end());
			return (*ordered.begin())->x == 0 && (*ordered.rbegin())->x == this->size - 1;
		}
	}

	bool isLegal(int x, int y) {
		if ((board[x][y]->player == -1) && (x < size && y < size)) {
			return true;
		}
		else {
			return false;
		}
	}
	bool isWinner(int player) {
		if (player == 1) {
			for (int i = 0; i < size; i++) {
				if (this->board[0][i]->player == 1 && isOver(0, i, false)) {
					return true;
				}
			}
		}
		if (player == 0) {
			for (int i = 0; i < size; i++) {
				if (this->board[i][0]->player == 0 && isOver(i, 0, true)) {
					return true;
				}
			}
		}
		return false;
	}

	std::string toString() const {
		stringstream ostream;

		for (int j = 0; j < size; j++) {
			int space = j * 2;

			for (int k = 0; k < space; k++) {
				ostream << " ";
			}

			for (int i = 0; i < size; i++) {


				Node* node = board[j][i];

				switch (node->getPlayer()) {
				case -1:
					ostream << '.';
					break;
				case 0:
					ostream << 'x';
					break;
				case 1:
					ostream << 'o';
					break;
				default:
					break;
				}
				if (i < size - 1) {
					ostream << " - ";
				}
				else {
					ostream << endl;
				}

			}
			for (int k = 0; k < space + 1; k++) {
				ostream << " ";
			}

			if (j < size - 1) {
				for (int i = 0; i < size - 1; i++) {
					ostream << "\\ / ";
				}
				ostream << "\\" << endl;
			}
			else {
				ostream << endl;
			}

		}

		return ostream.str();
	}


	int move(int player, int x, int y) {
		if (isLegal(x, y)) {
			board[x][y]->setPlayer(player);
			if (isOver(x, y, player == 0)) {
				return 1; // winning case
			}
			else {
				return 0;//resume
			}
		}
		else {
			return -1;//illegal move
		}
	}

	pair<int, int> findNextMove(int player) {
		vector <pair<int, int>> moves;
		while (moves.size() == 0) {
			for (int i = 0; i < 10000; i++) {

				Hexboard* newboard = this->copy(size);
				Node* first = newboard->randomize(player);
				// if win then store move in a vector, in the vector whichever move has highest frequency is the next move
				// we can check the index first member of board in each case its a win and use that as our move
				if (newboard->isWinner(1)) {
					moves.push_back(pair<int, int>(first->x, first->y));
				}
				delete newboard;
			}
		}


		auto accumulator = [](map <pair<int, int>, int> collector, pair<int, int> b)
		{
			if (collector.find(b) != collector.end()) {
				collector.insert({ b, 1 });
			}
			else {
				collector[b] = collector[b] + 1;
			}
			return collector;
		};

		map<pair<int, int>, int> freq = accumulate(moves.begin(), moves.end(), map<pair<int, int>, int>(), accumulator);
		if (freq.size() == 0) {
			cout << "Unable to get move\n";
		}
		int max = 0;
		pair<int, int> maxfreq;

		for (auto f : freq) {
			if (f.second > max) {
				max = f.second;
				maxfreq = f.first;
			}
		}
		return maxfreq;

	}

	int check(int user, int x, int y) {
		int test = move(user, x, y);
		if (test == 0) {

		}
	}

};




int main(int argc, char** argv) {
	int size;

	if (argc > 1) {
		size = stoi(argv[1]);
	}

	Hexboard board(size);
	//for (int i )
	int flag = 1;
	int x, y;
	int gamestate;
	std::cout << board.toString();
	while (flag == 1) {
		cout << "enter move: ";
		cin >> x >> y;
		int check = board.move(0, x, y);
		//check the user move cases
		if (check == -1) {
			cout << "illegal move" << endl;
			cout << "reenter move: ";
			cin >> x >> y;
			check = board.move(0, x, y);
		}
		//case for user win
		if (check == 1) {
			cout << endl;
			cout << "Congratulations! You won" << endl;
			flag = 0;
			break;
		}
		gamestate = board.move(0, x, y);
		cout.clear();
		cout.seekp(0);

		std::cout << board.toString();
		pair <int, int> move = board.findNextMove(1);
		//cout << move.first << " " << move.second << endl;
		gamestate = board.move(1, move.first, move.second);

		//cout << "enter move: ";
		//cin >> x >> y;
		//gamestate = board.move(1, x, y);
		std::cout << board.toString();
		cout.clear();
		cout.seekp(0);
		if (gamestate == 1) {
			cout << endl;
			cout << "Computer Won! Well played" << endl;
			flag = 0;
		}
	}
	cout << "Thank you for playing!";

	return 0;


}