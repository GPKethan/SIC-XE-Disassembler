#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#pragma once
#include <iostream>
#include <unordered_map>

#include "SpecialSymbol.h"
//#include "CtorException.h"

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
		//throw CtorException();
	};

	SymbolTable(string filename);

	static SymbolTable open(string filename);

	string getSymbol(int addr);
	SpecialSymbol getNextSymbol(int addr);
	int getAddress(string symbol);
	bool getFlag(int addr);
	unordered_map<int, SpecialSymbol>::iterator begin();
	unordered_map<int, SpecialSymbol>::iterator end();

	vector<SpecialSymbol> toArray();

	void temPrint();

private:
	bool checkTable(string, ifstream&);
	char peekLine(ifstream&);

	string removeWhitespace(string);

	unordered_map<int, SpecialSymbol> table;
	vector<SpecialSymbol> tableArray;		// Used to get the original ordering of the symbols

};

#endif //SYMBOLTABLE_H_