#include "state.h"
#include <climits>
#include <cassert>
const int State::dr[4]  = { -2, 2, -1, 1 };
const int State::dc[4]  = { -1, 1, -2, 2 };

void State::setBoard(const char* board)
{
    memcpy(this->board, board, 64);
}

void State::setTeam(int team)
{
    if (team == -1) this->ourTeam = BLACK;
    else this->ourTeam = WHITE;
}

State::State() { 
    
}

State::State(Team team) {
    ourTeam = team;
    myFirstRow = ((ourTeam == BLACK) ? 6 : 1);
    this->lastMove = make_pair( ii(-1,-1), ii(-1,-1) );
}
#define PAWN_SCORE 10
#define ROOK_SCORE 8
#define KNIGHT_SCORE 4
int State::eval() const
{
    int nMyPawns = 0, nMyRooks = 0, nMyKnights = 0;
    int nOtherPawns = 0, nOtherRooks = 0, nOtherKnights = 0;
    int myAdvancedPawns = 0;
    int otherAdvancedPawns = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            char c = board[i*8 + j];
            if (c == '.') continue;
            if (ourTeam == BLACK)
            {
                assert(myFirstRow == 6);
                switch(c)
                {
                case 'p':
                    if (abs(i - myFirstRow) >= 5)
                    {
                        myAdvancedPawns++;
                        if (abs(i-myFirstRow) == 6)
                            return INT_MAX;
                    }
                    nMyPawns++;
                    break;
                case 'r':
                    nMyRooks++;
                    break;
                case 'n':
                    nMyKnights++;
                    break;
                case 'P':
                    if (abs(i-myFirstRow) <= 3)
                    {
                        otherAdvancedPawns++;
                        if (abs(i-myFirstRow) == 0) return INT_MIN;
                    }
                    nOtherPawns++;
                    break;
                case 'R':
                    nOtherRooks++;
                    break;
                case 'N':
                    nOtherKnights++;
                }
            }
            else if (ourTeam == WHITE)
            {
                assert(myFirstRow == 1);
                switch(c){
                case 'p':
                    if (abs(i-myFirstRow) <= 3)
                    {
                        otherAdvancedPawns++;
                        if (abs(i-myFirstRow) == 0) return INT_MAX;
                    }
                    nOtherPawns++;
                    break;
                case 'r':
                    nOtherRooks++;
                    break;
                case 'n':
                    nOtherKnights++;
                    break;
                case 'P':
                    nMyPawns++;
                    if (abs(i - myFirstRow) >= 5)
                    {
                        myAdvancedPawns++;
                        if (abs(i-myFirstRow) == 6)
                            return INT_MIN;
                    }
                    break;
                case 'R':
                    nMyRooks++;
                    break;
                case 'N':
                    nMyKnights++;
                }
            }
        }
    }
    extern int playingAs;
    return (10*(nMyPawns - nOtherPawns) +
            8*(nMyRooks - nOtherRooks) +
            5*(nMyKnights - nOtherKnights) +
            20*(myAdvancedPawns - otherAdvancedPawns));
}

int State::getTeam() const
{ return int(this->ourTeam); }

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
	
bool State::remove_piece_at(ii pos)
{
    if (pos.first < 0 || pos.first >= 8)
        return false;
    board[pos.first*8+pos.second] = '.';
    return true;
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
    
    //#if __cplusplus > 199711L
    //std::for_each(moves.begin(), moves.end(), [this] () { this->changeTeam(); });
    //#else
    
    //#endif
    return moves;
}

bool State::move(ii f, ii t) 
{
    if (f.first < 0 || f.first >= 8 || f.second < 0 || f.second >= 8
        || t.first < 0 || t.first >= 8 || t.second < 0 || t.second >= 8)
        return false;
    if (foundFriend(t))
        return false;
    
    board[t.first*8 + t.second] = board[f.first*8 + f.second];
    board[f.first*8 + f.second] ='.';
    this->lastMove = make_pair(f,t);
    return true;
}

void State::generatePawnMoves(ii pos, vector<State>& moves) const
{
    
    
    if (!foundFriend(ii(pos.first+(int(ourTeam)), pos.second)) && !foundFoe(ii(pos.first + (int(ourTeam)), pos.second)))
    {
        State s = this->copy();
        if (s.move(pos, ii(pos.first + (int(ourTeam)), pos.second)))
            moves.push_back(s);
        if (pos.first == myFirstRow)
        {
            if (!foundFriend(ii(pos.first+(int(ourTeam)*2),pos.second)) && !foundFoe(ii(pos.first + (int(ourTeam)*2), pos.second)))
            {
                State s = this->copy();
                if (s.move(pos, ii(pos.first + (int(ourTeam)*2), pos.second)))
                    moves.push_back(s);
            }
        }
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


bool State::foundFriend (ii pos) const
{
    if ((islower(board[pos.first*8 + pos.second]) && ourTeam == BLACK) 
        || (isupper(board[pos.first*8+pos.second]) && ourTeam == WHITE))
        return true;
    else
        return false;
        
}

bool State::foundFoe (ii pos) const
{
    if ((isupper(board[pos.first*8 + pos.second]) && ourTeam == BLACK) 
        || (islower(board[pos.first*8+pos.second]) && ourTeam == WHITE))
        return true;
    else
        return false;
        
}



void State::generateRookMoves(ii pos, vector<State>& moves) const
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

void State::getLastMove(int &fr, int &fc, int &tr, int &tc) const
{
    fr = lastMove.first.first;
    fc = lastMove.first.second;
    tr = lastMove.second.first;
    tc = lastMove.second.second;
}

pair<ii,ii> State::getLastMove() const  
{ return this->lastMove; }
void State::print()
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

