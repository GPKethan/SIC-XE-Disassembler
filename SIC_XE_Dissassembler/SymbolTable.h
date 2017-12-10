#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#pragma once
#include <iostream>
#include <unordered_map>
#include <stdexcept>

#include "SpecialSymbol.h"

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
	SymbolTable() { };
	SymbolTable(string filename);
	static SymbolTable open(string filename);

	string getSymbol(int addr);
	bool hasSymbolAt(int addr);
	int getAddress(string symbol);
	bool getFlag(int addr);

	SpecialSymbol getNextSymbol(int addr);
	
	unordered_map<int, SpecialSymbol>::iterator begin();
	unordered_map<int, SpecialSymbol>::iterator end();

	vector<SpecialSymbol> toArray();

	void temPrint();

private:
	unordered_map<int, SpecialSymbol> table;
	vector<SpecialSymbol> tableArray;

	bool checkTable(string, ifstream&);
	char peekLine(ifstream&);
	string removeWhitespace(string);

};

#endif //SYMBOLTABLE_H_