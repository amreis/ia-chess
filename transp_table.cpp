#include "transp_table.h"
#include <cstdio>

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

int TranspTable::size() const
{ return this->_map.size(); }

// TODO TESTTTTTTTTTTTTTTTTTT
void TranspTable::saveToFile() const
{
	FILE *arq;
	arq = fopen("transptable.bin", "wb");

	for (auto it = _map.begin(); it != _map.end(); ++it)
	{
		pair<State, TranspTableEntry> p = *it;
		fwrite(&p.first, sizeof(State), 1, arq);
		fwrite(&p.second, sizeof(TranspTableEntry), 1, arq);
	}

	fclose(arq);
}

void TranspTable::readFromFile()
{
	FILE *arq = fopen("transptable.bin", "rb");
	if (arq == NULL) return;
	while (!feof(arq))
	{
		State x;
		TranspTableEntry y;
		fread(&x, sizeof(State), 1, arq);
		if (feof(arq)) break;
		fread(&y, sizeof(TranspTableEntry), 1, arq);
		this->insert(x,y);
	}
}
