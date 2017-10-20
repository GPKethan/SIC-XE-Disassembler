#ifndef DISASSEMBLE_H_
#define DISASSEMBLE_H_

#pragma once
#include <stdio.h>
#include <iostream>
#include <string>

#include "CtorException.h"

using namespace std;

class Dissassemble {
public:
	Dissassemble() {
		throw defaultCtorExcept;
	};

	Dissassemble(string inputPath, string outputPath);

	/*
	The only function in this class that should be called (not including ctor's). 
	It is essentially the manager of the class that starts the execution of the necessary functions

	*/
	void disassemble();

private:
	//----Functions----
	void readHeadRecord();
	void readTextRecord();
	void readEndRecord();

	int calculateTargetAddress(string);

	/*
	Sets the Program Counter variable

	Input: string address - The address in HEX
	*/
	void setProgctr(string address);
	
	/*
	Sets the Program Counter variable
	
	Input: string address - The address in DECIMAL
	*/
	void setProgctr(int address);

	//----GlobalVars----
	string inputPath;
	string outputPath;

	bool isIndirect;			// n
	bool isImmedaite;			// i 
	bool isSimpleAddressing;	// maybe not necessary
	bool isIndexAddressing;		// x
	bool isBaseRelative;		// b
	bool isPcRelative;			// p
	bool isExtended;			// maybe not necessary

	int progctr;				// Value is in base_10


};

#endif // DISASSEMBLE_H_