#include "transp_table.h"

TranspTableEntry::TranspTableEntry()
	: type(EntryType::EXACT), depth(0), score(0)
{

}

TranspTableEntry::TranspTableEntry(EntryType t, int d, int s)
{
	this->score = s;
	this->type = t;
	this->depth = d;
}

int TranspTableEntry::getScore() const
{ return this->score; }

EntryType TranspTableEntry::getType() const
{ return this->type; }

int TranspTableEntry::getDepth() const
{ return this->depth; }

void TranspTableEntry::setScore(int s)
{ this->score = s; }

void TranspTableEntry::setType(EntryType t)
{ this->type = t; }

void TranspTableEntry::setDepth(int d)
{ this->depth = d; }

TranspTable::TranspTable()
{ this->_map.clear(); }

bool TranspTable::lookup(const State& s, TranspTableEntry& out)
{
	map<State,TranspTableEntry>::iterator where;
	if ((where = _map.find(s)) != _map.end())
	{
		out = where->second;
		return true;
	}
	return false;
}

bool TranspTable::insert(const State& state, const TranspTableEntry& entry)
{
	return this->_map.insert(make_pair(state, entry)).second;
}
