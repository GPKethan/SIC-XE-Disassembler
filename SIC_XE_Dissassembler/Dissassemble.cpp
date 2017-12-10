#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "Dissassemble.h"

#define DEBUG				0
#define DEBUG_FORMAT		0
#define DEBUG_PRINT			0
#define DEBUG_WRITE_OUT		0
#define DEBUG_WORD_BYTE		0
#define DEBUG_RWORD_RBYTE	0
#define LISTING_FILE		1

using namespace std;

/*
	NOTE:	This program does NOT handle subroutines UNLESS the subroutine object starts on
			  a new TextRecord. If this is the case then:
			  "...uh, everything's perfectly all right now. We're fine.
			  We're all fine here, now, thank you. How are you?"
*/

/*
	TODO:	Flags should be part of SIC_LineBuilder
*/

const unordered_map<int, string> Dissassemble::registerTable{
	{ 0, "A" },
	{ 1, "X" },
	{ 2, "L" },
	{ 3, "B" },
	{ 4, "S" },
	{ 5, "T" },
	{ 6, "F" },
	{ 8, "PC" },
	{ 9, "SW" }
};

Dissassemble::Dissassemble(string inputPath, string outputPath, string symbolFile) {

	symtab = SymbolTable::open(symbolFile);
	littab = LiteralTable::open(symbolFile);

	flags = Flags::instantiate();

	base = 0;

	progctr = 0;
	progLength = 0;

	iohandler = IOHandler::open(inputPath, outputPath);

};

Dissassemble::~Dissassemble() {
	iohandler.close();
};

void Dissassemble::disassemble() {

	readHeadRecord();

	do
		readTextRecord();
	while (currLine[0] == 'T');

	readEndRecord();

};

void Dissassemble::readHeadRecord() {
	iohandler.getLine(currLine);
	progName = currLine.substr(RECORD_ADDR_POS, 6);
	progLength = Convert::hexToDecimal(currLine.substr(PROG_LTH_OFFSET));
	SIC_LineBuilder toPrint(progName, "START", "");
	iohandler.writeOut(toPrint, flags);
};

void Dissassemble::readTextRecord() {

	iohandler.getLine(currLine);

	if (currLine[0] == 'M' || currLine[0] == 'E')
		return;

	// We could ignore this part, BUT if something weent wrong in the previous text record 
	//   the progctr variable *could* also be wrong, so might as well set it to the 
	//   correct value. Better safe than sorry
	progctr = Convert::hexToDecimal(currLine.substr(RECORD_ADDR_POS, 6));

	for (int i = TEXT_REC_START_POS; i < currLine.size();) {

		if (littab.hasLiteralAt(progctr)) {
			int poolLength = writeOutLtorg();
			i += poolLength;
			continue;
		}

		SIC_LineBuilder line(symtab, currLine, i, progctr);

		bool flagSet = true;
		if (line.format == 2)
			formatTwo(line, i);
		else if (line.format == 4)
			flagSet = formatThreeAndFour(line, i);
		
		// REFACTOR THIS
		// If the flags weren't set then we know something fucky is going on
		if (!flagSet || isWordOrByteDirective(line)) {
			wordByte(line, i);
			break;
		}

		// multiply currFormat by 2 b/c input is in half-bytes
		i += line.format * 2;
		iohandler.writeOut(line, flags);
		updateProgctr(line.format);

	}

	reswResb();

};

void Dissassemble::readEndRecord() {

	while (currLine[0] != 'E')
		iohandler.getLine(currLine);
	
	int addr = Convert::hexToDecimal(currLine.substr(RECORD_ADDR_POS));
	string operand = symtab.getSymbol(addr);
	SIC_LineBuilder toPrint("", "END", progName);
	iohandler.writeOut(toPrint, flags);

};

int Dissassemble::calculateTargetAddress(SIC_LineBuilder &line) {
	
	if (line.format == 4)
		return line.displacement;

	int modifier = flags.getIsBaseRelative() ? base : progctr + line.format;
	if (!flags.getIsBaseRelative() && !flags.getIsPcRelative())
		modifier = 0;

	if (line.isDispNegative)
		return modifier - line.displacement;

	return line.displacement + modifier;

};

void Dissassemble::updateProgctr(int offset) {
	progctr += offset;
};

void Dissassemble::formatOne(SIC_LineBuilder &line) {
	//TODO: code this
};

void Dissassemble::formatTwo(SIC_LineBuilder &line, int index) {

	int registerMnemonic = stoi(currLine.substr(index + 2, 1));
	string registerOne = registerTable.at(registerMnemonic);
	line.operand.append(registerOne);

	// Handle special case opcodes
	if (line.opcode == "clear" || line.opcode == "tixr")
		return;

	registerMnemonic = stoi(currLine.substr(index + 3, 1));
	string registerTwo = registerTable.at(registerMnemonic);
	line.operand.push_back(',');
	line.operand.append(registerTwo);

};

bool Dissassemble::formatThreeAndFour(SIC_LineBuilder &line, int index) {

	// If the flags didn't get set something weird is happening, so return
	if (!flags.setFlags(currLine, index))
		return false;

	line.format = flags.getIsExtended() ? 4 : 3;
	line.setDisplacement(currLine, index);
	int addr = calculateTargetAddress(line);
	line.setOperand(symtab, littab, flags, addr);

	base = (line.opcode == "ldb") ? addr : base;

	return true;

};

// Needs refactoring
bool Dissassemble::isWordOrByteDirective(SIC_LineBuilder &line) {

	//(isImmediate != isIndirect) == isImmediate XOR isIndirect
	// if format4 && immediate XOR indirect is set -> return false
	if (flags.getIsExtended() && (flags.getIsImmediate() || flags.getIsIndirect()))
		return false;

	int targetAddr = calculateTargetAddress(line);

	// While its possible this may be a legit line, it makes no sense for it to happen so 
	//  I'm willing to risk it.
	// Basically saying: are you using BASE to get to a something in memory that comes 
	//  before the BASE address?
	if (flags.getIsBaseRelative() && targetAddr < 0)
		return true;

	if (Optab::getFormat(line.mnemonic) != line.format && Optab::getFormat(line.mnemonic) != 4)
		return true;

	if (line.format <= 2)
		return false;

	// unless its format 1 or 2, one or both of the immediate/indirect flags has to be set
	if (!flags.getIsImmediate() && !flags.getIsIndirect())
		return true;

	if (flags.getIsBaseRelative() && flags.getIsPcRelative() && !flags.getIsImmediate())
		return true;

	if (!flags.getIsBaseRelative() && !flags.getIsPcRelative() && !flags.getIsImmediate())
		return true;

	// check if disp is out of range
	if (targetAddr < 0 || targetAddr > progLength)
		return true;

	return false;

};

void Dissassemble::wordByte(SIC_LineBuilder &line, int index) {

	// We want to reset the flags because they're set to whatever was printed earlier
	flags.resetFlags();
	
	// set format to a invalid format so that nothing weird prints
	line.format = -1;

	while (progctr < progLength && index < currLine.size()) {

		if (littab.hasLiteralAt(progctr))
			break;

		int nextSymAddr = symtab.getNextSymbol(progctr).getValue();
		nextSymAddr = (nextSymAddr == progctr || nextSymAddr == INT_MIN) ? progLength : nextSymAddr;

		// Subtract that val from the current addr - this is the number of bytes to be read in
		int delta = nextSymAddr - progctr;
		int lthToReadIn = delta * 2;

		line.symbol = symtab.getSymbol(progctr);
		line.opcode = (delta % 3 == 0) ? "WORD" : "BYTE";
		line.operand = currLine.substr(index, lthToReadIn);		

		updateProgctr(delta);
		index += lthToReadIn;

		iohandler.writeOut(line, flags);		

	}

};

void Dissassemble::reswResb() {

	string nextLine = iohandler.peekNextLine();
	int nextLineAddr = Convert::hexToDecimal(nextLine.substr(RECORD_ADDR_POS, 6));
	if (nextLine[0] == 'M' || nextLine[0] == 'E' || nextLineAddr == 0)
		nextLineAddr = progLength;

	int delta = INT_MIN;
	while (progctr < nextLineAddr && progctr < progLength && delta != 0) {

		SIC_LineBuilder line;

		line.symbol = symtab.getSymbol(progctr);
		int nextSymbolAddr = symtab.getNextSymbol(progctr).getValue();
		nextSymbolAddr = (nextSymbolAddr == INT_MIN) ? progLength : nextSymbolAddr;

		delta = nextSymbolAddr - progctr;

		line.opcode = "RESB";
		line.operand = to_string(delta);
		if (symtab.getFlag(progctr))
			line.opcode = "EQU";
		else if (delta % 3 == 0) {
			line.opcode = "RESW";
			line.operand = to_string(delta / 3);
		}

		iohandler.writeOut(line, flags);
		updateProgctr(delta);

	}

};

int Dissassemble::writeOutLtorg() {
	int poolLength = iohandler.writeOutLtorg(littab, progctr);
	updateProgctr(poolLength / 2);
	return poolLength;
};

void Dissassemble::stopper() {
	// For debugging cause idk how to do that yet with Visual Studio
	int stop;
	cout << "\t\t\t\t\t";
	cin >> stop;
	cout << endl;
}