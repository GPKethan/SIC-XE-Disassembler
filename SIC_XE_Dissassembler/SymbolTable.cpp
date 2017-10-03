#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <ctype.h>

#include "SymbolTable.h"
#include "Convert.h"

typedef unordered_map<int, Symbol>::iterator u_map_iter;

using namespace std;

// Populates the SymbolTable with the stuff from the .sym file
SymbolTable::SymbolTable(string filename) {

	ifstream file(filename.c_str(), ios::in);

	// Check that the file is actually there
	if (!file) {
		cout << "SymbolTable NOT FOUND" << endl;
		exit(EXIT_SUCCESS);
	}

	string currLine;

	// need to skip the first 2 lines
	getline(file, currLine);

	// Check if SymbolTable or LiteralTable
	bool isSymbolTable = checkTable(currLine, file);

	// Skip past the dashes
	getline(file, currLine);

	while (!file.eof() && isSymbolTable) {

		getline(file, currLine);
		isSymbolTable = checkTable(currLine, file);
		if (currLine.size() == 0 || currLine[0] == '.')
			continue;
		else if (currLine.size() == 1 && (currLine[0] == '\n' || currLine[0] == '\r'))
			continue;
		else if (!isSymbolTable)
			break;

		string symbol = removeWhitespace(currLine.substr(SYM_POS, MAX_SYM));
		int value = Convert::hexToDecimal(currLine.substr(VAL_POS, MAX_VAL));
		bool flag = (currLine[FLG_POS] == 'A') ? 1 : 0;
		Symbol sym(symbol, value, flag);

		/*
		This was added in for something, but I dont remember why
		if (table[value].getValue() == value)
			value *= -1;
		*/

		table[value] = sym;

	}

	temPrint();

}

string SymbolTable::getSymbol(int addr) {

	u_map_iter it = table.find(addr);
	if (it == table.end())
		return "";

	Symbol temp = table.find(addr)->second;
	return temp.getSymbol();

}

bool SymbolTable::getFlag(int addr) {

	u_map_iter it = table.find(addr);
	if (it == table.end())
		return "";

	Symbol temp = table.find(addr)->second;
	return temp.getIsAbsolute();

}

u_map_iter SymbolTable::begin() {
	return table.begin();
}

u_map_iter SymbolTable::end() {
	return table.end();
}

// If the first char of input is 'S' then that means we're looking at
//  "Symbol  Value   Flags:"
//  Which means its the SymbolTable, otherwise its the littab
bool SymbolTable::checkTable(string currLine, ifstream& file) {

	// If we're supposed to account for weird SymbolTable input this will need to be fixed; specifically
	//  the last part after ||
	if ((currLine.substr(0, 4) == "Name" && peekLine(file) == '-') ||
		currLine[0] == ' ' || currLine.size() == 0) {
		return false;
	}

	return true;

}

// return the first char of the next line
char SymbolTable::peekLine(ifstream& file) {

	int len = file.tellg();
	string line;

	// Read line
	getline(file, line);

	// Return to position before "Read line".
	file.seekg(len, ios_base::beg);

	return line[0];

}

string SymbolTable::removeWhitespace(string curr) {

	int end = 0;
	for (int i = (curr.size() - 1); i > 0; i--) {
		if (curr[i] != ' ')
			break;
		end = i;
	}

	return curr.substr(0, end);

}


void SymbolTable::temPrint() {

	cout << "----SymbolTable----" << endl;
	//	ofstream out ("SymbolTable.sic", ios::out | ios::ate | ios::app);

	for (u_map_iter it = table.begin(); it != table.end(); it++) {

		int value = it->first;
		Symbol sym = it->second;

		cout << value << " => " << sym.getSymbol() << endl;

	}

}