#include "state.h"
#include "bot.h"
#include "transp_table.h"
#include <climits>
#include <algorithm>
#include <queue>

using namespace std;

TranspTable transp;
typedef std::pair<int,int> ii;
int playingAs;
// Negamax + Alpha-Beta pruning
// TODO Order nodes!
pair<int, State> negamax(const State& node, int depth, int alpha, int beta, int player)
{
	TranspTableEntry e;
	int originalAlpha = alpha;
	if (transp.lookup(node, e) && e.getDepth() >= depth &&
		node.getLastMove() != pair<ii,ii>(ii(-1,-1), ii(-1,-1)))
	{
		switch(e.getType())
		{
		case EntryType::EXACT:
			return make_pair(e.getScore(), node);
		break;
		case EntryType::LOWER_BOUND:
			alpha = max(alpha, e.getScore());
		break;
		case EntryType::UPPER_BOUND:
			beta = min(beta, e.getScore());
		break;
		}
		if (alpha >= beta)
			return make_pair(e.getScore(), node);
	}
	if (depth == 0 || node.isTerminal())
	{
		// Eval needs to be relative to the side playing, or else we need to add
		// a multiplicative factor here.
		return make_pair( node.eval(), node);
	}

	int bestValue = INT_MIN;
	State bestState;

	vector<State> children = node.getChildrenStates();
	if (children.empty())
		return make_pair( node.eval(), node);
	std::sort(children.begin(), children.end(), State::_evalComparer);
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
	TranspTableEntry newEntry;
	newEntry.setScore(bestValue);
	if (bestValue <= originalAlpha)
	{
		newEntry.setType(EntryType::UPPER_BOUND);
	} else if (bestValue >= beta)
	{
		newEntry.setType(EntryType::LOWER_BOUND);
	} else {
		newEntry.setType(EntryType::EXACT);
	}
	newEntry.setDepth(depth);
	transp.insert(node, newEntry);
	return make_pair(bestValue, bestState);

}

int main()
{
	transp.readFromFile();
	cout << "Defina a profundidade da árvore de busca" << endl;
	int DEPTH;
	cin >> DEPTH;

	cout << "Digite a cor do time..." << endl;
	int c;
	cin >> c;
	ConnectAPI bot(c == 1 ? 50100 : 50200, "Alistinho");

	while (true)
	{

		ServerBoard b = bot.readMsg();
		if (b.end)
		{
			transp.saveToFile();
			continue;
		}
		State k(b.board, b.whiteMoves ? 1 : -1);
		playingAs = (b.whiteMoves ? 1 : -1);

		pair<int, State> p = negamax(k, DEPTH, -State::INF, State::INF, playingAs);

		int r1, c1, r2, c2;

		cout << endl;
		p.second.getLastMove(r1, c1, r2, c2);
		bot.sendMove(r1,c1,r2,c2);
		cout << "Transp table size: " << transp.size() << endl;
	}
	return 0;
}
