#ifndef SIC_LINEBUILDER_H_
#define SIC_LINEBUILDER_H_

#pragma once

#include <iostream>
#include <string>

#include "SymbolTable.h"
#include "Optab.h"
#include "LiteralTable.h"
#include "Flags.h"
#include "Convert.h"

using namespace std;

class SIC_LineBuilder {
public:
	string mnemonic;
	string opcode;
	string operand;
	string symbol;
	int format;
	int displacement;
	bool isDispNegative;

	SIC_LineBuilder();

	SIC_LineBuilder(string symbol, string opcode, string operand);

	SIC_LineBuilder(SymbolTable &symtab, string toExtract, int startPos, int addr);

	/*
	Does pretty much what the name implies.

	Parameters:	string &currLine - the most recent Text Record
				int index - the position to start reading from currLine
	*/
	void setDisplacement(string &currLine, int index);

	/*
	Does pretty much what the name implies.

	Parameters:	SymbolTable &symtab		- the SymbolTable
				LiteralTable &littab	- the LiteralTable
				Flags &flags			- a Flags object so that we can make sure we're doing the correct thing
				int addr				- the current address we're at
	*/
	void setOperand(SymbolTable &symtab, LiteralTable &littab, Flags &flags, int addr);

	/* 
	   I HAVE LITERALLY NO IDEA IF THIS BELONGS HERE, BUT IT MAKES SENSE TO PUT IT HERE 
	   RIGHT NOW SO THATS WHAT IM GOING TO DO

	   Parameters: SymbolTable &symtab
					int &delta
					int progctr
					int progLength
	*/
	void buildReswLine(SymbolTable &symtab, int &delta, int progctr, int progLength);

};
#endif