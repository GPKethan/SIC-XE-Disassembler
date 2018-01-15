#include "stdafx.h"
#include "SIC_LineBuilder.h"

using namespace std;

SIC_LineBuilder::SIC_LineBuilder() {
	this->mnemonic = "";
	this->opcode = "";
	this->format = -1;
	this->operand = "";
	this->symbol = symbol;
	this->displacement = -1;
	this->isDispNegative = false;
};

SIC_LineBuilder::SIC_LineBuilder(string symbol, string opcode, string operand) : SIC_LineBuilder() {
	this->opcode = opcode;
	this->operand = operand;
	this->symbol = symbol;
};

SIC_LineBuilder::SIC_LineBuilder(SymbolTable &symtab, string toExtract, int startPos, int addr) : SIC_LineBuilder() {
	this->mnemonic = Optab::getMnemonic(toExtract[startPos], toExtract[startPos + 1]);
	this->opcode = Optab::getOpcode(mnemonic);
	this->format = Optab::getFormat(mnemonic);
	this->symbol = symtab.getSymbol(addr);
};

/*
Does pretty much what the name implies.

Parameters:	string &currLine - the most recent Text Record
int index - the position to start reading from currLine
*/
void SIC_LineBuilder::setDisplacement(string &currLine, int index) {

	if (this->format == 4) {
		displacement = Convert::hexToDecimal(currLine.substr(index + DISPLACEMENT_OFFSET, 5));
		return;
	}

	string hexDisp = currLine.substr(index + DISPLACEMENT_OFFSET, 3);
	if (hexDisp[0] != 'F')
		displacement = Convert::hexToDecimal(hexDisp);
	else {
		displacement = Convert::negativeHexToPositiveDecimal(hexDisp);
		isDispNegative = true;
	}

};

/*
Does pretty much what the name implies.

Parameters:	SymbolTable &symtab		- the SymbolTable
LiteralTable &littab	- the LiteralTable
Flags &flags			- a Flags object so that we can make sure we're doing the correct thing
int addr				- the current address we're at
*/
void SIC_LineBuilder::setOperand(SymbolTable &symtab, LiteralTable &littab, Flags &flags, int addr) {

	bool isRelativeToSomething = true;
	if (this->format == 3)
		isRelativeToSomething = flags.getIsBaseRelative() != flags.getIsPcRelative();

	if (symtab.hasSymbolAt(addr) && isRelativeToSomething)
		this->operand = symtab.getSymbol(addr);
	else if (littab.hasLiteralAt(addr) && isRelativeToSomething)
		this->operand = littab.getLiteral(addr);
	else
		this->operand = to_string(addr);

};

/*
I HAVE LITERALLY NO IDEA IF THIS BELONGS HERE, BUT IT MAKES SENSE TO PUT IT HERE
RIGHT NOW SO THATS WHAT IM GOING TO DO

Parameters: SymbolTable &symtab
int &delta
int progctr
int progLength
*/
void SIC_LineBuilder::buildReswLine(SymbolTable &symtab, int &delta, int progctr, int progLength) {

	int nextSymbolAddr = symtab.getNextSymbol(progctr).getValue();
	nextSymbolAddr = (nextSymbolAddr == INT_MIN) ? progLength : nextSymbolAddr;
	delta = nextSymbolAddr - progctr;

	this->symbol = symtab.getSymbol(progctr);
	this->opcode = "RESB";
	this->operand = to_string(delta);
	if (symtab.getFlag(progctr))
		this->opcode = "EQU";
	else if (delta % 3 == 0) {
		this->opcode = "RESW";
		this->operand = to_string(delta / 3);
	}

};