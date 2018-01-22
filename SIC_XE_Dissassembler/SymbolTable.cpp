#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <ctype.h>

#include "SymbolTable.h"
#include "Convert.h"

typedef unordered_map<int, SpecialSymbol>::iterator u_map_iter;

using namespace std;

// Populates the SymbolTable with the stuff from the .sym file
SymbolTable::SymbolTable(string filename) {

	ifstream file(filename.c_str(), ios::in);

	// Check that the file is actually there
	if (!file) {
		cout << "SYM_FILE DOESN'T EXIST; PANIC!" << endl;
		file.close();
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
		SpecialSymbol sym(symbol, value, flag, tableArray.size());

		/*
		This was added in for something, but I dont remember why
		maybe if an absolute symbol has the same value as a relative???
		if (table[value].getValue() == value)
			value *= -1;
		*/

		tableArray.push_back(sym);
		table[value] = sym;

	}

	//temPrint();

	file.close();

};

SymbolTable SymbolTable::open(string filename) {
	return SymbolTable(filename);
};

string SymbolTable::getSymbol(int addr) {

	u_map_iter it = table.find(addr);
	if (it == table.end())
		return "";
	
	SpecialSymbol temp = table.find(addr)->second;
	return temp.getSymbol();

};

SpecialSymbol SymbolTable::getNextSymbol(int addr) {

	// Get current symbol
	SpecialSymbol curr = table.find(addr)->second;
	int currIndex = curr.getIndexInArray();

	if (currIndex + 1 >= tableArray.size())
		return SpecialSymbol(to_string(INT_MIN), INT_MIN, false, -1);

	return tableArray[currIndex + 1];

};

int SymbolTable::getAddress(string symbol) {
	for (auto curr : tableArray) {
		if (curr.getSymbol() == symbol)
			return curr.getValue();
	}
	return -1;
};

bool SymbolTable::getFlag(int addr) {

	u_map_iter it = table.find(addr);
	if (it == table.end())
		return "";

	SpecialSymbol temp = table.find(addr)->second;
	return temp.getIsAbsolute();

};

bool SymbolTable::hasSymbolAt(int addr) {
	return getSymbol(addr) != "";
};

u_map_iter SymbolTable::begin() {
	return table.begin();
};

u_map_iter SymbolTable::end() {
	return table.end();
};

vector<SpecialSymbol> SymbolTable::toArray() {
	return tableArray;
};

// If the first char of input is 'S' then that means we're looking at
//  "Symbol  Value   Flags:"
//  Which means its the SymbolTable, otherwise its the littab
bool SymbolTable::checkTable(string currLine, ifstream& file) {

	if ((currLine.substr(0, 4) == "Name" && peekLine(file) == '-') ||
		currLine[0] == ' ' || currLine.size() == 0) {
		return false;
	}

	return true;
	
};

// return the first char of the next line
char SymbolTable::peekLine(ifstream& file) {

	int len = file.tellg();
	string line;

	// Read line
	getline(file, line);

	// Return to position before "Read line".
	file.seekg(len, ios_base::beg);

	return line[0];

};

string SymbolTable::removeWhitespace(string curr) {

	int end = 0;
	for (int i = (curr.size() - 1); i > 0; i--) {
		if (curr[i] != ' ')
			break;
		end = i;
	}

	return curr.substr(0, end);

};

void SymbolTable::temPrint() {

	cout << "----SymbolTable----" << endl;
	//	ofstream out ("SymbolTable.sic", ios::out | ios::ate | ios::app);
	vector<SpecialSymbol>::iterator it = tableArray.begin();
	for (; it != tableArray.end(); it++) {
	
		string value = Convert::decimalToHex(it->getValue());
		SpecialSymbol sym = *it;
		string abs = "R";
		if (sym.getIsAbsolute())
			abs = "A";

		cout << value << "\t=>" << setw(10) << sym.getSymbol() << setw(10) << abs << endl;

	}

};