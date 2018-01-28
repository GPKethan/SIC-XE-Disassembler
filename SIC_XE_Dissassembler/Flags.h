#ifndef FLAGS_H_
#define FLAGS_H_

#pragma once
#include <iostream>
#include <string>

#include "Convert.h"

#define INDIRECT_OFFSET 1
#define INDEXED_OFFSET 2
#define DISPLACEMENT_OFFSET 3

using namespace std;

class Flags {
public:
	Flags();

	Flags(string currLine);

	static Flags getInstance();

	/*
	Set the N, I, X, B, P, and E flags

	Parameter:	int index - the current place in currLine
	Return:		true if everything went okay, otherwise false
	*/
	bool setFlags(string &currLine, int index);

	void resetFlags();

	bool getIsIndirect();
	bool getIsImmediate();
	bool getIsSimpleAddressing();
	bool getIsIndexAddressing();
	bool getIsBaseRelative();
	bool getIsPcRelative();
	bool getIsExtended();

private:
	bool isIndirect;			// n
	bool isImmediate;			// i 
	bool isSimpleAddressing;	// maybe not necessary
	bool isIndexAddressing;		// x
	bool isBaseRelative;		// b
	bool isPcRelative;			// p
	bool isExtended;			// is format 4

};
#endif // FLAGS_H_