#ifndef STATE_H
#define STATE_H

#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cctype>
#include <algorithm>

using std::vector;
using std::pair;
using std::cout;
using std::endl;

typedef pair<int,int> ii;

class State
{
private:
	char board[64];
	enum Team { BLACK = -1, WHITE = 1 } ourTeam;
	int myFirstRow;
	static const int dr[4];
	static const int dc[4];

	pair<ii,ii> lastMove;
	explicit State(Team);
	void setBoard(const char* board);
public:
	static const int INF;

	State();
	explicit State(const char*, int);

	State copy() const;

	void changeTeam();

	vector<State> getChildrenStates() const;
	bool move(ii,ii);

	bool foundFriend(ii) const;
	bool foundFoe(ii) const;

	void generatePawnMoves(ii, vector<State>&) const;
	void generateRookMoves(ii, vector<State>&) const;
	void generateKnightMoves(ii, vector<State>&) const;

	void print() const;

	void setTeam(int);

	int eval() const;

	int getTeam() const;
	void getLastMove(int&,int&,int&,int&) const;
	pair<ii,ii> getLastMove() const;
	const char* getBoard() const;

	bool isTerminal() const;

	State& operator=(const State& other);
    bool operator<(const State& other) const;
    bool operator==(const State& other) const;
};

#endif
