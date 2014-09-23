#ifndef TRANSP_TABLE_H
#define TRANSP_TABLE_H
#include "state.h"
#include <map>

enum EntryType { LOWER_BOUND, UPPER_BOUND, EXACT };

using std::map;
using std::make_pair;
class TranspTableEntry
{
private:
	EntryType type;
	int depth;
	int score;
public:
	explicit TranspTableEntry();
	explicit TranspTableEntry(EntryType, int, int);

	EntryType getType() const;
	int getDepth() const;
	int getScore() const;

	void setType(EntryType t);
	void setDepth(int d);
	void setScore(int s);
};

class TranspTable
{
private:
	map<State, TranspTableEntry> _map;
public:
	explicit TranspTable();

	bool lookup(const State& s, TranspTableEntry& out);
	bool insert(const State& s, const TranspTableEntry& entry);

};




#endif
