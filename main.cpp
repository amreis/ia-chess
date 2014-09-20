#include "state.h"
#include "bot.h"
#include <climits>
#include <algorithm>

using namespace std;
State sToMove;
int bestTillNow = -1;
#define START_DEPTH 5

int playingAs;
pair<int, State> negamax(const State& node, int depth, int alpha, int beta, int player)
{
    if (depth == 0) // || node.terminal()
    {
        return make_pair( node.eval(), node);
    }

    int bestValue = INT_MIN;
    State bestState;

    vector<State> children = node.getChildrenStates();
    if (playingAs*player > 0)
        reverse(children.begin(), children.end());
    // How do we order?
    //  - Material advantage?
    for (State& s : children)
    {
        s.changeTeam();
        pair<int, State> r = negamax(s, depth-1, -beta, -alpha, -player);
        int val = -r.first;

        if (bestValue < val)
        {
            bestValue = val;
            bestState = s;
        }
        alpha = max(alpha, val);

        if (alpha >= beta)
            break;
    }
    return make_pair(bestValue, bestState);

}

int main()
{
    cout << "Digite a cor do time..." << endl;
    int c;
    cin >> c;
    ConnectAPI bot(c == 1 ? 50100 : 50200, "Alistinho");

    while (true)
    {

        ServerBoard b = bot.readMsg();
        State k(b.board, b.whiteMoves ? 1 : -1);
        playingAs = (b.whiteMoves ? 1 : -1);
        pair<int, State> p = negamax(k, START_DEPTH, INT_MIN, INT_MAX, 1);

        int r1, c1, r2, c2;
        p.second.print();
        cout << endl;
        p.second.getLastMove(r1, c1, r2, c2);
        bot.sendMove(r1,c1,r2,c2);
    }
    return 0;
}
