#pragma once
#include <iostream>
#include <unordered_map>
#include <exception>

#include "Symbol.h"
#include "CtorException.h"

// Some Definitions to make life easier
#define MAX_SYM 8
#define MAX_VAL 6
#define MAX_FLG 1
#define SYM_POS 0
#define VAL_POS 8
#define FLG_POS 16

using namespace std;

class SymbolTable {
public:
	SymbolTable() {
		throw defaultCtorExcept;
	};

	SymbolTable(string filename);

	string getSymbol(int addr);
	bool getFlag(int addr);
	unordered_map<int, Symbol>::iterator begin();
	unordered_map<int, Symbol>::iterator end();

private:
	bool checkTable(string, ifstream&);
	char peekLine(ifstream&);
	void temPrint();

	string removeWhitespace(string);

	unordered_map<int, Symbol> table;

};