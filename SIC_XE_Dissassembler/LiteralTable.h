#ifndef LITERALTABLE_H_
#define LITERALTABLE_H_

#pragma once
#include <iostream>
#include <unordered_map>
#include <string>

#include "Literal.h"

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
	LiteralTable() { };
	LiteralTable(string filename);
	
	/*
	Returns a LiteralTable Object when given a filename
	*/
	static LiteralTable open(string filename);

	/*
	@param	addr - the address where a literal might be stored at
	@return the literal located at that address, or an empty string 
			 if one doesn't exist
	*/
	string getLiteral(int addr);

	/*
	@param	lit - the literal 
	@return the associated adderss of the literal
	*/
	int getAddress(string lit);

	/*
	@param	addr - the address where a literal might be stored at
	@return true if there is at the specified address, otherwise false
	*/
	bool hasLiteralAt(int addr);

	/*
	@param	addr - the address where a literal might be stored at
	@return the length of the literal, and -1 if there is no literal at that address
	*/
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
#endif //LITERALTABLE_H_