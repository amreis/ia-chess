#include "state.h"
#include <climits>
#include <cassert>

const int State::dr[4]  = { -2, 2, -1, 1 };
const int State::dc[4]  = { -1, 1, -2, 2 };
const int State::INF = 10000;

void State::setBoard(const char* board)
{
	memcpy(this->board, board, 64);
}

void State::setTeam(int team)
{
	if (team == -1) this->ourTeam = BLACK;
	else this->ourTeam = WHITE;
	this->myFirstRow = ((ourTeam == BLACK) ? 6 : 1);
}

State::State() {

}
// Construct state from a team, and define its first row. (internal only)
State::State(Team team) {
	ourTeam = team;
	myFirstRow = ((ourTeam == BLACK) ? 6 : 1);
	this->lastMove = make_pair( ii(-1,-1), ii(-1,-1) );
}
#define PAWN_SCORE 10
#define ROOK_SCORE 6
#define KNIGHT_SCORE 7
#define ADV_PAWN_SCORE 25
int State::eval() const
{
	int nWhiteP = 0, nWhiteR = 0, nWhiteN = 0;
	int nBlackP = 0, nBlackR = 0, nBlackN = 0;
	int nWhiteAP = 0, nBlackAP = 0;
	bool blackWin = false, whiteWin = false;
	const static int centerDominance[8] = {0, 1, 2, 5, 5, 2, 1, 0};
	const static int advPawnWeight[] = {0,3,5,7,10,13,17};
	int whiteCenter = 0, blackCenter = 0;
	int whiteLP = 0, blackLP = 0;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			char c = board[i*8+j];
			if (c == '.') continue;
			switch(c)
			{
				case 'p':
					blackCenter += centerDominance[j];
					nBlackP++;
					if (i <= 1)
					{
						// nBlackAP++;
						nBlackAP += advPawnWeight[6-i];
						if (i == 0) { blackWin = true; }
					}
					else if (board[(i-1)*8 + j] != '.')
						blackLP++;
					break;
				case 'r':
					blackCenter += centerDominance[j];
					nBlackR++;
					break;
				case 'n':
					blackCenter += centerDominance[j];
					nBlackN++;
					break;
				case 'P':
					whiteCenter += centerDominance[j];
					nWhiteP++;
					if (i >= 6)
					{
						//nWhiteAP++;
						nWhiteAP += advPawnWeight[i-1];
						if (i == 7) { whiteWin = true; }
					}
					else if (board[(i+1)*8 + j] != '.')
						whiteLP++;
					break;
				case 'R':
					whiteCenter += centerDominance[j];
					nWhiteR++;
					break;
				case 'N':
					whiteCenter += centerDominance[j];
					nWhiteN++;
					break;
			}
			if (blackWin || whiteWin) break;
		}
		if (blackWin || whiteWin) break;
	}
	if (nWhiteP == 0) blackWin = true;
	else if (nBlackP == 0) whiteWin = true;

	if (blackWin)
		return ((ourTeam == BLACK) ? INF : -INF);
	else if (whiteWin)
		return ((ourTeam == WHITE) ? INF : -INF);
	/*
	return (PAWN_SCORE*(nWhiteP - nBlackP) +
			ROOK_SCORE*(nWhiteR - nBlackR) +
			KNIGHT_SCORE*(nWhiteN - nBlackN) +
			ADV_PAWN_SCORE*(nWhiteAP - nBlackAP)) * int(ourTeam);
	*/

	return (PAWN_SCORE*(nWhiteP - nBlackP - int(ourTeam)) +
			ROOK_SCORE*(nWhiteR - nBlackR) +
			KNIGHT_SCORE*(nWhiteN - nBlackN) +
			ADV_PAWN_SCORE*(nWhiteAP - nBlackAP) +
			2*(whiteCenter - blackCenter) +
			-5*(whiteLP - blackLP)) * int(ourTeam);
}

int State::getTeam() const
{ return int(this->ourTeam); }
// This constructor creates a state by reading the board that the SERVER sends us.
State::State(const char* board, int team)
{

	for (int i = 7, k = 0; i >= 0; --i, ++k)
	{
		for (int j = 0; j < 8; ++j)
		{
			this->board[i*8+j] = board[k*8+j];
		}
	}
	ourTeam = (team == 1) ? WHITE : BLACK;
	if (ourTeam == BLACK) myFirstRow = 6;
	else myFirstRow = 1;
	this->lastMove = make_pair( ii(-1,-1), ii(-1,-1) );
}

State State::copy() const
{
	State s(ourTeam);
	s.setBoard(this->board);
	return s;
}

void State::changeTeam()
{
	this->ourTeam = ((this->ourTeam == BLACK) ? WHITE : BLACK);
	this->myFirstRow = ((this->ourTeam == BLACK) ? 6 : 1);
}

vector<State> State::getChildrenStates() const
{
	vector<State> moves;
	moves.reserve(40);
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			const int pos = i*8+j;
			if (board[pos] == '.') continue;
			if (islower(board[pos]) && ourTeam == BLACK)
			{
				switch(board[pos])
				{
				case 'p':
					generatePawnMoves(ii(i, j), moves);
					break;
				case 'r':
					generateRookMoves(ii(i, j), moves);
					break;
				case 'n':
					generateKnightMoves(ii(i, j), moves);
					break;
				}
			}
			else if (isupper(board[pos]) && ourTeam == WHITE)
			{
				switch(board[pos])
				{
				case 'P':
					generatePawnMoves(ii(i, j), moves);
					break;
				case 'R':
					generateRookMoves(ii(i, j), moves);
					break;
				case 'N':
					generateKnightMoves(ii(i,j), moves);
					break;
				}
			}
		}
	}

	return moves;
}
// Moves a piece from <f> to <t>. If any position is invalid or there is a friend
// there, returns false since the movement can't be made.
bool State::move(ii f, ii t)
{
	if (!isValidPosition(f) || !isValidPosition(t) || f == t)
		return false;
	if (foundFriend(t))
		return false;

	board[t.first*8 + t.second] = board[f.first*8 + f.second];
	board[f.first*8 + f.second] = '.';
	#ifdef DEBUG
	assert(f != t);
	#endif
	this->lastMove = make_pair(f,t);
	return true;
}

void State::generatePawnMoves(ii pos, vector<State>& moves) const
{
	// One step forward
	ii movingTo = ii(pos.first+(int(ourTeam)), pos.second);
	if (!foundFriend(movingTo) && !foundFoe(movingTo))
	{
		State s = this->copy();
		if (s.move(pos, movingTo))
			moves.push_back(s);
		// Two steps forward
		if (pos.first == myFirstRow)
		{
			movingTo = ii(pos.first + (int(ourTeam) * 2), pos.second);
			if (!foundFriend(movingTo) && !foundFoe(movingTo))
			{
				State s = this->copy();
				if (s.move(pos, movingTo))
					moves.push_back(s);
			}
		}
	}
	// Try to capture to the right
	if (foundFoe(ii(pos.first + (int(ourTeam)), pos.second + 1)))
	{
		State s = this->copy();
		if (s.move(pos, ii(pos.first + (int(ourTeam)), pos.second + 1)))
			moves.push_back(s);

	}
	// Try to capture to the left
	if (foundFoe(ii(pos.first + (int(ourTeam)), pos.second - 1)))
	{
		State s = this->copy();
		if (s.move(pos, ii(pos.first + (int(ourTeam)), pos.second -1)))
			moves.push_back(s);

	}

}

// Function that determines if there is a FRIEND in some position of the board.
bool State::foundFriend (ii pos) const
{
	if (!isValidPosition(pos)) return false;
	if ((islower(board[pos.first*8 + pos.second]) && ourTeam == BLACK)
		|| (isupper(board[pos.first*8+pos.second]) && ourTeam == WHITE))
		return true;
	else
		return false;

}

// Function that determines if there is a FOE in some position of the board.
bool State::foundFoe (ii pos) const
{
	if (!isValidPosition(pos)) return false;
	if ((isupper(board[pos.first*8 + pos.second]) && ourTeam == BLACK)
		|| (islower(board[pos.first*8+pos.second]) && ourTeam == WHITE))
		return true;
	else
		return false;

}
// Function that determines if a position (row,col) is inside the board.
bool State::isValidPosition(ii pos) const
{
	return pos.first >= 0 && pos.first <= 7 && pos.second >= 0 &&
		pos.second <= 7;
}

void State::generateRookMoves(ii pos, vector<State>& moves) const
{
	// Remember that: if we found a foe, we can move to its position by taking it
	// and then we stop there. If we found a friend, we can't move to its position.
	// UP
	for (int i = 1; i < 8; i++)
	{
		if (pos.first + i >= 8) break;
		if (foundFoe(ii((pos.first + i), pos.second)))
		{	//MATA TUTO
			State s = this->copy();
			if (s.move(pos, ii((pos.first + i), pos.second)))
				moves.push_back(s);
			break;
		}
		else if (foundFriend(ii((pos.first + i), pos.second)))
		{
			break;
		}
		else
		{
			State s = this->copy();
			if (s.move(pos, ii((pos.first + i), pos.second)))
				moves.push_back(s);
		}
	}
	// RIGHT

	for (int i = 1; i < 8; ++i)
	{
		if (pos.second + i >= 8) break;
		if (foundFoe(ii(pos.first, pos.second+i)))
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first, pos.second+i)))
				moves.push_back(s);
			break;
		}
		else if (foundFriend(ii(pos.first, pos.second+i)))
			break;
		else
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first, pos.second+i)))
				moves.push_back(s);
		}
	}
	// DOWN
	for (int i = 1; i < 8; i++)
	{
		if (pos.first - i < 0) break;
		if (foundFoe(ii((pos.first - i), pos.second)))
		{	//MATA TUTO
			State s = this->copy();
			if (s.move(pos, ii((pos.first - i), pos.second)))
				moves.push_back(s);

			break;
		}
		else if (foundFriend(ii((pos.first - i), pos.second)))
		{
			break;
		}
		else
		{
			State s = this->copy();
			if (s.move(pos, ii((pos.first - i), pos.second)))
				moves.push_back(s);
		}
	}
	// LEFT
	for (int i = 1; i < 8; ++i)
	{
		if (pos.second - i < 0) break;
		if (foundFoe(ii(pos.first, pos.second - i)))
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first, pos.second-i)))
				moves.push_back(s);
			break;
		}
		else if (foundFriend(ii(pos.first, pos.second-i)))
			break;
		else
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first, pos.second-i)))
				moves.push_back(s);
		}
	}
}

void State::generateKnightMoves(ii pos, vector<State>& moves) const
{
	for (int i = 0; i < 4; ++i)
	{
		// Hehe.
		for (int j = (i & 0xfffffffe); j <= (i | 0x00000001); ++j)
		{
			State s = this->copy();
			int newR = pos.first + dr[i];
			int newC = pos.second + dc[j];
			if (s.move(pos, ii(newR, newC)))
				moves.push_back(s);
		}
	}
}
// Output the last move made in this board to the four variables.
void State::getLastMove(int &fr, int &fc, int &tr, int &tc) const
{
	fr = lastMove.first.first;
	fc = lastMove.first.second;
	tr = lastMove.second.first;
	tc = lastMove.second.second;
}

pair<ii,ii> State::getLastMove() const
{ return this->lastMove; }

void State::print() const
{
	for (int i = 7; i >= 0; --i)
	{
		for (int j = 0; j < 8; ++j)
		{
			std::cout << (board[i*8 + j]);
		}
		std::cout << std::endl;
	}
}

// Function that checks whether a board corresponds to a Game Over situation
bool State::isTerminal() const
{
	// If there is a pawn in any of the vertical extremities of the board
	for (int j = 0; j < 8; ++j)
	{
		// This works since a pawn can't move backwards, so we don't need to
		// check its color.
		if (tolower(this->board[j]) == 'p') return true; // j == [0][j]
		if (tolower(this->board[56+j]) == 'p') return true; // 56 + j == 7*8 +j == [7][j]
	}
	// If there are no pawns at extremities, but there are still pawns in the game,
	// it hasn't ended yet.
	for (int i = 1; i < 7; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (tolower(board[i*8+j]) == 'p') return false;
		}
	}
	// Else, it's game over.
	return true;
}

State& State::operator=(const State& other)
{
	memcpy(this->board, other.getBoard(), 64);
	this->setTeam(other.getTeam());
	this->lastMove = other.getLastMove();
	return *this;
}

bool State::operator<(const State& other) const
{
    return (memcmp(this->getBoard(), other.getBoard(), 64) < 0);
}

bool State::operator==(const State& other) const
{
    return (memcmp(this->getBoard(), other.getBoard(), 64) == 0);
}

const char* State::getBoard() const
{ return this->board; }
