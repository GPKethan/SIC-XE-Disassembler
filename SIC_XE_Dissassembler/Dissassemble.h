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
#include "Flags.h"
#include "IOHandler.h"
#include "SIC_LineBuilder.h"

#define RECORD_ADDR_POS 1
#define PROG_LTH_OFFSET 14
#define TEXT_REC_START_POS 9
#define INDIRECT_OFFSET 1
#define INDEXED_OFFSET 2
#define DISPLACEMENT_OFFSET 3

using namespace std;

class Dissassemble {
public:
	Dissassemble() { };

	Dissassemble(string inputPath, string outputPath, string symbolFile);

	~Dissassemble();

	/*
	The only function in this class that should be called (not including ctor's). 
	It is the manager of the class that starts the execution of the necessary functions
	*/
	void disassemble();

private:
	//----GlobalVars----
	SymbolTable symtab;
	LiteralTable littab;

	Flags flags;

	int base;					// Value is in base_10

	int progctr;				// Value is in base_10
	int progLength;

	string progName;

	IOHandler iohandler;
	string currLine;			// Used to hold the current line for input purposes

	static const unordered_map<int, string> registerTable;

	//----Functions----
	void readHeadRecord();
	void readTextRecord();
	void readEndRecord();

	void stopper();

	int calculateTargetAddress(SIC_LineBuilder &line);
	/*
	Updates the Program Counter variable, by adding the offest to it.

	Parameter:	int offset - The format of the current opcode
	*/
	void updateProgctr(int offset);
	
	/*
	Handles cases when the opcode is of format 1.

	Parameter:	string opcode
	*/
	void formatOne(SIC_LineBuilder &line);

	/*
	Handles cases when the opcode is of format 2.

	Parameter:	string opcode
				string operand - holds the operand (if there is one)
				int index - the current place in currLine
	*/
	void formatTwo(SIC_LineBuilder &line, int index);

	/*
	Normally I would have format 3 and 4 seperated (in fact that's how it originally was), 
	  BUT after refactoring the two methods were SO similar I decided to just combine them.
	Handles cases when the opcode is of format 3/4.

	Parameter:	line - 
	*/
	bool formatThreeAndFour(SIC_LineBuilder &line, int index);


	/*
	Checks if the current info if "nonsense". If it is, then we know that we're
	  dealing with a WORD or BYTE Directive.

	Parameter:	string symbol
				string mnemonic - the current mnemonic (the hex one)
				string operand
				int format
	*/
	bool isWordDirective(SIC_LineBuilder &line);

	/*
	Handles the word and byte assembler directives.

	Parameter:	string symbol
				string directive - either WORD or BYTE
				string operand
				int index - the current place in currLine
	*/
	void wordByte(SIC_LineBuilder &line, int index);

	/*
	Handles the RESW && RESB assembler directives.
	*/
	void reswResb();

};

#endif // DISASSEMBLE_H_