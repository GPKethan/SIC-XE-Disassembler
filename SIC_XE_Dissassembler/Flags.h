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
	Flags() {
		isIndirect = false;
		isImmediate = false;
		isSimpleAddressing = false;
		isIndexAddressing = false;
		isBaseRelative = false;
		isPcRelative = false;
		isExtended = false;
	};

	Flags(string currLine) {
		setFlags(currLine, 0);
	};

	static Flags instantiate() {
		return Flags();
	};

	/*
	Set the N, I, X, B, P, and E flags

	Parameter:	int index - the current place in currLine
	Return:		true if everything went okay, otherwise false
	*/
	bool setFlags(string &currLine, int index) {

		if (index + INDIRECT_OFFSET >= currLine.size() || index + INDEXED_OFFSET >= currLine.size())
			return false;

		// the iNdirect and Immediate flags
		string nAndI = Convert::hexToBinary(currLine[index + INDIRECT_OFFSET]);
		isIndirect = (nAndI[2] == '1') ? true : false;
		isImmediate = (nAndI[3] == '1') ? true : false;
		isSimpleAddressing = isIndirect && isImmediate;

		// the indeXed, Base relative, Pc relative, and Extended flags
		string xBPE = Convert::hexToBinary(currLine[index + INDEXED_OFFSET]);
		isIndexAddressing = (xBPE[0] == '1') ? true : false;
		isBaseRelative = (xBPE[1] == '1') ? true : false;
		isPcRelative = (xBPE[2] == '1') ? true : false;
		isExtended = (xBPE[3] == '1') ? true : false;

		return true;

	};

	void resetFlags() {

		// the iNdirect and Immediate flags
		isIndirect = false;
		isImmediate = false;
		isSimpleAddressing = true;

		// the indeXed, Base relative, Pc relative, and Extended flags
		isIndexAddressing = false;
		isBaseRelative = false;
		isPcRelative = false;
		isExtended = false;

	};

	bool getIsIndirect() {
		return isIndirect;
	};

	bool getIsImmediate() {
		return isImmediate;
	};

	bool getIsSimpleAddressing() {
		return isSimpleAddressing;
	};

	bool getIsIndexAddressing() {
		return isIndexAddressing;
	};

	bool getIsBaseRelative() {
		return isBaseRelative;
	};

	bool getIsPcRelative() {
		return isPcRelative;
	};

	bool getIsExtended() {
		return isExtended;
	};

private:
	bool isIndirect;			// n
	bool isImmediate;			// i 
	bool isSimpleAddressing;	// maybe not necessary
	bool isIndexAddressing;		// x
	bool isBaseRelative;		// b
	bool isPcRelative;			// p
	bool isExtended;			// is format 4?

};
#endif // FLAGS_H_