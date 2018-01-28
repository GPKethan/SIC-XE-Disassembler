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

#define RECORD_ADDR_POS		1
#define RECORD_ADDR_LTH		6

#define TEXT_REC_START_POS	9

#define PROG_LTH_OFFSET		14

#define INDIRECT_OFFSET		1
#define INDEXED_OFFSET		2
#define DISPLACEMENT_OFFSET 3

#define REC_TYPE_FLAG_POS	0

#define MODIFICATION		'M'
#define END					'E'

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
	void readRecords();

private:
	//----GlobalVars----
	SymbolTable symtab;
	LiteralTable littab;

	Flags flags;

	int baseRegisterVal;		// Value is in base_10

	int progctr;				// Value is in base_10
	int progLength;

	string progName;

	IOHandler iohandler;
	string currLine;			// Used to hold the current line/record for input purposes

	static const unordered_map<int, string> registerTable;

	//----Functions----
	void readHeadRecord();
	void readTextRecords();
	void readSingleTextRecord();
	void readEndRecord();

	/*
	Tells us if the current line is a modification or end record.

	@param	lineToCheck - the line we're inspecting to see if its a mod/end record
	@return	true if the current line is a mod/end record, false otherwise
	*/
	bool isModOrEndRecord(string &lineToCheck);

	/*
	Updates the Program Counter variable, by adding the offest to it.

	Parameter:	int offset - The format of the current opcode
	*/
	void updateProgctr(int offset);

	/*
	PLEASE NOTE: I'm positive this method should be in LineBuilder, HOWEVER if I put 
				 this method in LineBuilder then I have to put formatThreeAndFour() 
				 in there as well. The problem with this is that formatThreeAndFour() 
				 is dependent on many functions and variables that are only in this class. 
				 SO, I can't move it anywhere until I figure something else out

	Handles cases when the opcode is of format 1.

	Parameter:	SIC_LineBuilder &line - the line we're creating
	*/
	void handleFormatOne(SIC_LineBuilder &line);

	/*
	PLEASE NOTE: I'm positive this method should be in LineBuilder, HOWEVER if I put 
				 this method in LineBuilder then I have to put formatThreeAndFour() 
				 in there as well. The problem with this is that formatThreeAndFour() 
				 is dependent on many functions and variables that are only in this class. 
				 SO, I can't move it anywhere until I figure something else out

	Handles cases when the opcode is of format 2.

	Parameter:	SIC_LineBuilder &line - the line we're creating
				int index - the position we're at in currLine
	*/
	void handleFormatTwo(SIC_LineBuilder &line, int index);

	/*
	PLEASE NOTE: I'm positive this method should be in LineBuilder, HOWEVER if I put 
				 this method in LineBuilder then I have to put formatThreeAndFour() 
				 in there as well. The problem with this is that formatThreeAndFour() 
				 is dependent on many functions and variables that are only in this class. 
				 SO, I can't move it anywhere until I figure something else out
	
	Handles cases when the opcode is of format 3/4.

	Parameter:	SIC_LineBuilder &line - the line we're creating
				int index - the position we're at in currLine
	Return:		false if the flag was NOT set, otherwise true
	*/
	bool handleFormatThreeAndFour(SIC_LineBuilder &line, int index);

	/*
	Checks if the current info if "nonsense". If it is, then we know that we're
	  dealing with a WORD or BYTE Directive.

	Parameter:	line	- the line we're creating
	Parameter:	flagSet - tells us if the flag object got set
	*/
	bool isWordByteDirective(SIC_LineBuilder &line);

	/*
	Calculates what the line's operand is.

	Parameter:	SIC_LineBuilder &line - the line we're creating
	Return:		The target address
	*/
	int calculateTargetAddress(SIC_LineBuilder &line);

	/*
	Handles the word and byte assembler directives.

	Parameter:	SIC_LineBuilder &line - the line we're creating
				int index - the position we're at in currLine
	*/
	void handleWordByteDirectives(SIC_LineBuilder &line, int index);

	/*
	Handles the RESW && RESB assembler directives.
	*/
	void handleReservationDirectives();
	

	// Temp thing for stuff
	void stopper();

};

#endif // DISASSEMBLE_H_