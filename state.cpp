#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cctype>
using std::vector;
using std::pair;
typedef pair<int, int> ii;
class State
{
private:
	char board[64];
	enum Team { BLACK = -1, WHITE = 1 } ourTeam;
	int myFirstRow;
	
	static int const dr[4];
	static int const dc[4];
	void setBoard(const char* board)
	{
		memcpy(this->board, board, 64);
	}
public:
	State(Team team) {
		ourTeam = team;
		myFirstRow = ourTeam == BLACK ? 6 : 1;
	};
	State(const char* board, int team)
	{

		for (int i = 7, k = 0; i >= 0; --i, ++k)
		{
			for (int j = 0; j < 8; ++j)
			{
				this->board[i*8+j] = board[k*8+j];
			}
		}
		ourTeam = team == 0 ? WHITE : BLACK;
		if (ourTeam == BLACK) myFirstRow = 6;
		else myFirstRow = 1;
	}
	
	bool remove_piece_at(ii pos)
	{
		if (pos.first < 0 || pos.first >= 8)
			return false;
		board[pos.first*8+pos.second] = '.';
	}
	
	State copy()
	{
		State s(ourTeam);
		s.setBoard(this->board);
		return s;
	}
	
	void changeTeam()
	{
		this->ourTeam = this->ourTeam == BLACK ? WHITE : BLACK;
	}
	
	vector<State> getChildrenStates()
	{
		vector<State> moves;
		moves.reserve(25);
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
						//generatePawnMoves(ii(pos/8, pos%8), moves);
						break;
					case 'r':
						//generateRookMoves(ii(pos/8, pos%8), moves);
						break;
					case 'n':
						generateKnightMoves(ii(pos/8, pos%8), moves);
						break;
					}
				}
				else if (isupper(board[pos]) && ourTeam == WHITE)
				{
					switch(board[pos])
					{
					case 'P':
						generatePawnMoves(ii(pos/8, pos%8), moves);
						break;
					case 'R':
						generateRookMoves(ii(pos/8, pos%8), moves);
						break;
					case 'N':
						
						break;
					}
				}
			}
		}
		return moves;
	}
	
	bool move(ii f, ii t)
	{
		if (f.first < 0 || f.first >= 8 || t.first < 0 || t.second >= 8)
			return false;
		if (foundFriend(t))
			return false;
		
		board[t.first*8 + t.second] = board[f.first*8 + f.second];
		board[f.first*8 + f.second] ='.';
		return true;
	}
	
	void generatePawnMoves(ii pos, vector<State>& moves)
	{
		if (pos.first == myFirstRow)
		{
			if (!foundFoe(ii(pos.first + (int(ourTeam)*2), pos.second)))
			{
				State s = this->copy();
				if (s.move(pos, ii(pos.first + (int(ourTeam)*2), pos.second)))
					moves.push_back(s);
			}
		}
		
		if (!foundFoe(ii(pos.first + (int(ourTeam)), pos.second)))
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first + (int(ourTeam)), pos.second)))
				moves.push_back(s);
		}
		
		
		if (foundFoe(ii(pos.first + (int(ourTeam)), pos.second + 1)))
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first + (int(ourTeam)), pos.second + 1)))
				moves.push_back(s);			

		}
		
		if (foundFoe(ii(pos.first + (int(ourTeam)), pos.second - 1)))
		{
			State s = this->copy();
			if (s.move(pos, ii(pos.first + (int(ourTeam)), pos.second -1)))
				moves.push_back(s);

		}
	}


	bool foundFriend (ii pos)
	{
		if ((islower(board[pos.first*8 + pos.second]) && ourTeam == BLACK) 
			|| (isupper(board[pos.first*8+pos.second]) && ourTeam == WHITE))
			return true;
		else
			return false;
			
	}
	
	bool foundFoe (ii pos)
	{
		if ((isupper(board[pos.first*8 + pos.second]) && ourTeam == BLACK) 
			|| (islower(board[pos.first*8+pos.second]) && ourTeam == WHITE))
			return true;
		else
			return false;
			
	}
	

	
	void generateRookMoves(ii pos, vector<State>& moves)
	{

		
		for (int i = 1; i < 8; i++)
		{
			if (pos.first + i >= 8) break;
			if (foundFoe(ii((pos.first + i), pos.second)))
			{	//MATA TUTO
				State s = this->copy();
				if (s.move(pos, ii((pos.first - i), pos.second)))
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
	}
	
	void generateKnightMoves(ii pos, vector<State>& moves)
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
	void print()
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

};

const int State::dr[4]  = { -2, 2, -1, 1 };
const int State::dc[4]  = { -1, 1, -2, 2 };
int main()
{
	State s("....................p.........n......r.........P................", 1);
	vector<State> l = s.getChildrenStates();
	for (vector<State>::iterator it = l.begin(); it != l.end(); ++it)
	{
		it->print();
		std::cout << std::endl;
	}
}
