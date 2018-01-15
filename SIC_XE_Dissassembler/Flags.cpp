#include "stdafx.h"
#include <iostream>
#include <string>

#include "Flags.h"

using namespace std;

Flags::Flags() {
	isIndirect = false;
	isImmediate = false;
	isSimpleAddressing = false;
	isIndexAddressing = false;
	isBaseRelative = false;
	isPcRelative = false;
	isExtended = false;
};

Flags::Flags(string currLine) {
	setFlags(currLine, 0);
};

Flags Flags::instantiate() {
	return Flags();
};

/*
Set the N, I, X, B, P, and E flags

Parameter:	int index - the current place in currLine
Return:		true if everything went okay, otherwise false
*/
bool Flags::setFlags(string &currLine, int index) {

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

/*
RESet the N, I, X, B, P, and E flags
*/
void Flags::resetFlags() {

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

bool Flags::getIsIndirect() {
	return isIndirect;
};

bool Flags::getIsImmediate() {
	return isImmediate;
};

bool Flags::getIsSimpleAddressing() {
	return isSimpleAddressing;
};

bool Flags::getIsIndexAddressing() {
	return isIndexAddressing;
};

bool Flags::getIsBaseRelative() {
	return isBaseRelative;
};

bool Flags::getIsPcRelative() {
	return isPcRelative;
};

bool Flags::getIsExtended() {
	return isExtended;
};