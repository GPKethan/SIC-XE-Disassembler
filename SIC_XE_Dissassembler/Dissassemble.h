#ifndef DISASSEMBLE_H_
#define DISASSEMBLE_H_

#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>

#include "SymbolTable.h"
#include "LiteralTable.h"
#include "Optab.h"

#define TEXT_REC_ADDR_POS 1
#define TEXT_REC_START_POS 9
#define INDIRECT_OFFSET 1
#define INDEXED_OFFSET 2
#define DISPLACEMENT_OFFSET 3

using namespace std;

class Dissassemble {
public:
	Dissassemble() {
		//throw CtorException();
	};

	Dissassemble(string inputPath, string outputPath, string symbolFile);

	/*
	The only function in this class that should be called (not including ctor's). 
	It is the manager of the class that starts the execution of the necessary functions
	*/
	void disassemble();

private:
	//----Functions----
	void readHeadRecord();
	void readTextRecord();
	void readEndRecord();

	void stopper();

	int calculateTargetAddress(int, bool, int);
	int calculateTargetAddress(string, bool, int);

	/*
	Updates the Program Counter variable, by adding the offest to it.

	Parameter:	int offset - The format of the current opcode
	*/
	void updateProgctr(int offset);

	/*
	Sets the Program Counter variable to the value specified.

	Parameter:	int currFormat - The format of the current opcode
	*/
	void setProgctr(int val);
	
	/*
	Handles cases when the opcode is of format 1.

	Parameter:	string opcode
	*/
	void formatOne(string &opcode);

	/*
	Handles cases when the opcode is of format 2.

	Parameter:	string opcode
				string operand - holds the operand (if there is one)
				int index - the current place in currLine
	*/
	void formatTwo(string &opcode, string &operand, int index);

	/*
	Handles cases when the opcode is of format 3.

	Parameter:	string opcode
				string operand - holds the operand (if there is one)
				int index - the current place in currLine
				int targetAddr - the targetAddress; only here because its needed in other methods
	*/
	void formatThree(string &opcode, string &operand, int index);

	/*
	Handles cases when the opcode is of format 4.

	Parameter:	string opcode
				string operand - holds the operand (if there is one)
				int index - the current place in currLine
				int targetAddr - the targetAddress; only here because its needed in other methods
	*/
	void formatFour(string &opcode, string &operand, int index);

	/*
	Checks if the current info if "nonsense". If it is, then we know that we're
	  dealing with a WORD or BYTE Directive.

	Parameter:	string symbol
				string mnemonic - the current mnemonic (the hex one)
				string operand
				int format
	*/
	bool isWordOrByteDirective(string &symbol, string &mnemonic, string &operand, int &format);

	/*
	Handles the word and byte assembler directives.

	Parameter:	string symbol
				string directive - either WORD or BYTE
				string operand
				int index - the current place in currLine
	*/
	void wordByte(string &symbol, string &directive, string &operand, int index);

	/*
	Handles the RESW && RESB assembler directives.
	*/
	void reswResb();

	// I don't belong here!
	string peekNextLine();

	/*
	Uses the current operand to figure out the displacement.
	I did this because I didn't want to have to keep track of ANOTHER variable.

	Parameter:	string operand - the current operand we're dealing with
	*/
	int getDisplacement(string &operand, int &format);

	bool isOperandNumber(string &operand);

	void writeOut(string symbol, string opcode, string operand, int currFormat);

	int writeOutLtorg();
	
	//for testing only
	void tempWriteOut(string symbol, string opcode, string operand, int currFormat);
	int tempWriteOutLtorg();

	/*
	Set the N, I, X, B, P, and E flags
	Parameter:	int index - the current place in currLine
	*/
	void setFlags(int index);

	void resetFlags();

	//----GlobalVars----
	SymbolTable symtab;
	LiteralTable littab;

	// These flags could maybe be their own class/struct,
	//   that way you wouldn't need them to be global variables.
	// If these flags are taken out then so should the writeOut methods
	bool isIndirect;			// n
	bool isImmediate;			// i 
	bool isSimpleAddressing;	// maybe not necessary
	bool isIndexAddressing;		// x
	bool isBaseRelative;		// b
	bool isPcRelative;			// p
	bool isExtended;			// maybe not necessary

	int base;					// Value is in base_10
	//int index;				// Value is in base_10

	int progctr;				// Value is in base_10
	int progLength;

	ifstream inFile;
	ofstream outFile;
	string currLine;			// Used to hold the current line for input purposes

	static const unordered_map<int, string> registerTable;

};

#endif // DISASSEMBLE_H_