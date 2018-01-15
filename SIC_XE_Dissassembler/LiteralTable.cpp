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

#include "LiteralTable.h"
#include "convert.h"

using namespace std;

LiteralTable::LiteralTable(string filename) {

	ifstream file(filename.c_str(), ios::in);

	// Check that the file is actually there
	if (!file) {
		cout << "LIT_FILE FILE NOT FOUND. PANIC!" << endl;
		file.close();
		exit(EXIT_SUCCESS);
	}

	string currLine;
	getline(file, currLine);

	// Check if symtab or LiteralTable
	bool isSymtab = checkTable(currLine, file);

	// Get to the part of the symbol file that represents the LiteralTable
	while (isSymtab && !file.eof()) {
		getline(file, currLine);
		isSymtab = checkTable(currLine, file);
	}

	if (file.eof()) {
		cout << "no literals" << endl;
		Literal lit("", "", -1, -1);
		table[-1] = lit;
		return;
	}

	// Go past the dashes
	getline(file, currLine);

	fillTable(file);

	temPrint();

	file.close();

}

LiteralTable LiteralTable::open(string filename) {
	return LiteralTable::LiteralTable(filename);
}

string LiteralTable::getLiteral(int addr) {

	unordered_map<int, Literal>::iterator it = table.find(addr);

	if (it == table.end())
		return "";

	Literal temp = table.find(addr)->second;

	return temp.getLiteral();

}

int LiteralTable::getAddress(string lit) {
	for (auto curr : table) {
		if (curr.second.getLiteral() == lit)
			return curr.second.getAddress();
	}
	return -1;
};

bool LiteralTable::hasLiteralAt(int addr) {
	return LiteralTable::getLiteral(addr) != "";
}

int LiteralTable::getLength(int addr) {

	unordered_map<int, Literal>::iterator it = table.find(addr);

	if (it == table.end())
		return -1;

	Literal temp = table.find(addr)->second;

	return temp.getLength();

}

int LiteralTable::size() {
	return this->table.size();
}

void LiteralTable::fillTable(ifstream &file) {

	string currLine;

	while (!file.eof()) {

		getline(file, currLine);

		// if there's a comment or newline skip it!
		if (currLine.size() == 0 || currLine[0] == '.')
			continue;

		string name = removeWhitespace(currLine.substr(NAM_POS, MAX_NAM));
		string literal = removeWhitespace(currLine.substr(LIT_POS, MAX_LIT));
		int length = Convert::stringToInt(currLine.substr(LTH_POS, MAX_LTH));
		int addr = Convert::hexToDecimal(removeWhitespace(currLine.substr(ADR_POS, MAX_ADR)));

		string tempLth = currLine.substr(LTH_POS, MAX_LTH);

		Literal lit(name, literal, length, addr);
		table[addr] = lit;

	}

}

/* 
If the first char of input is 'S' then that means we're looking at
  "Symbol  Value   Flags:"
  Which means its the symtab, otherwise its the LiteralTable
*/
bool LiteralTable::checkTable(string currLine, ifstream& file) {

	if (currLine.substr(0, 4) == "Name" && peekLine(file) == '-')
		return false;

	return true;

}

// Returns the first char of the next line
char LiteralTable::peekLine(ifstream& file) {
	int len = file.tellg();
	string line;

	// Read line
	getline(file, line);

	// Return to position before "Read line".
	file.seekg(len, ios_base::beg);

	return line[0];

}

string LiteralTable::removeWhitespace(string curr) {

	int end = curr.length();
	for (int i = curr.length(); i > 0; i--) {
		if (curr[i] == ' ')
			end = i;
		else
			break;
	}

	int beg = 0;
	for (int i = 0; i < end; i++) {
		if (curr[i] == ' ')
			beg = i;
		else
			break;
	}

	end -= beg;

	return curr.substr(beg, end);

}

void LiteralTable::temPrint() {

	cout << "----LiteralTable----" << endl;
	//	ofstream out ("LiteralTable.txt", ios::out | ios::ate | ios::app);

	cout << "address" << "\t";
	cout << "length" << "\t";
	cout << "literal" << "\t\t";
	cout << "name" << endl;

	for (unordered_map<int, Literal>::iterator it = table.begin(); it != table.end(); it++) {

		Literal lit = it->second;
		cout << Convert::decimalToHex(lit.getAddress()) << "\t";
		cout << lit.getLength() << "\t";
		cout << lit.getLiteral() << "\t\t";
		cout << lit.getName() << endl;

	}

	cout << "----------" << endl;

}