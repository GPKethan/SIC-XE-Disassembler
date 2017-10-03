#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <exception>

#include "Literal.h"
#include "CtorException.h"

// Some definitions to make life easier
#define MAX_NAM 8
#define MAX_LIT 8
#define MAX_LTH 6
#define MAX_ADR 6
#define NAM_POS 0
#define LIT_POS 8
#define LTH_POS 17
#define ADR_POS 24

using namespace std;

class LiteralTable {
public:
	// Constructor
	LiteralTable() {
		throw defaultCtorExcept;
	};

	LiteralTable(string filename);

	string getLiteral(int addr);
	int getLength(int addr);
	int size();

private:
	void fillTable(ifstream &file);
	bool checkTable(string, ifstream&);
	char peekLine(ifstream&);
	void temPrint();

	// Removes whitespace from both the front and back
	string removeWhitespace(string);

	unordered_map<int, Literal> table;

};