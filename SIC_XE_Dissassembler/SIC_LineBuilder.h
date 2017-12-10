#ifndef SIC_LINEBUILDER_H_
#define SIC_LINEBUILDER_H_

#pragma once
#include <iostream>
#include <string>

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

	SIC_LineBuilder() {
		this->mnemonic = "";
		this->opcode = "";
		this->format = -1;
		this->operand = "";
		this->symbol = symbol;
		this->displacement = -1;
		this->isDispNegative = false;
	};

	SIC_LineBuilder(string symbol, string opcode, string operand) : SIC_LineBuilder() {
		this->opcode = opcode;
		this->operand = operand;
		this->symbol = symbol;
	};

	SIC_LineBuilder(string mnemonic, string opcode, int format, string operand, string symbol) : SIC_LineBuilder() {
		this->mnemonic = mnemonic;
		this->opcode = opcode;
		this->format = format;
		this->operand = operand;
		this->symbol = symbol;	
	};

	SIC_LineBuilder(SymbolTable &symtab, string toExtract, int startPos, int addr) : SIC_LineBuilder() {
		this->mnemonic = Optab::getMnemonic(toExtract[startPos], toExtract[startPos + 1]);
		this->opcode = Optab::getOpcode(mnemonic);
		this->format = Optab::getFormat(mnemonic);
		this->symbol = symtab.getSymbol(addr);
	};

	/*
	  Basically a switchboard operator telling the parameters where to go depending on the format.

	  @param currLine:		--
	  @param posInCurrLine:	--
	*/
	void setDisplacement(string &currLine, int posInCurrLine) {
		if (this->format == 3)
			setDisp(currLine, posInCurrLine);
		else
			setAddress(currLine, posInCurrLine);
	};

	void setOperand(SymbolTable &symtab, LiteralTable &littab, Flags &flags, int addr) {

		bool isRelativeToSomething = true;
		if (this->format == 3)
			isRelativeToSomething = flags.getIsBaseRelative() != flags.getIsPcRelative();

		if (symtab.hasSymbolAt(addr) && isRelativeToSomething)
			this->operand = symtab.getSymbol(addr);
		else if (littab.hasLiteralAt(addr) && isRelativeToSomething)
			this->operand = littab.getLiteral(addr);
		else
			this->operand = to_string(addr);

	}

private:
	void setAddress(string &currLine, int posInCurrLine) {
		displacement = Convert::hexToDecimal(currLine.substr(posInCurrLine + DISPLACEMENT_OFFSET, 5));
	};

	void setDisp(string &currLine, int posInCurrLine) {
		string hexDisp = currLine.substr(posInCurrLine + DISPLACEMENT_OFFSET, 3);
		if (hexDisp[0] != 'F')
			displacement = Convert::hexToDecimal(hexDisp);
		else {
			displacement = Convert::negativeHexToPositiveDecimal(hexDisp);
			isDispNegative = true;
		}
	}

};
#endif