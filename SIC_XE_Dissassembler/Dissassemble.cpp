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

	flags = Flags::getInstance();

	baseRegisterVal = 0;

	progctr = 0;
	progLength = 0;

	iohandler = IOHandler::open(inputPath, outputPath);

};


/*
Kills IOHandler.
*/
Dissassemble::~Dissassemble() {
	iohandler.close();
};


void Dissassemble::readRecords() {
	readHeadRecord();
	readTextRecords();
	readEndRecord();
};


void Dissassemble::readHeadRecord() {
	iohandler.getLine(currLine);
	progName = currLine.substr(RECORD_ADDR_POS, RECORD_ADDR_LTH);
	progLength = Convert::hexToDecimal(currLine.substr(PROG_LTH_OFFSET));
	SIC_LineBuilder toPrint(progName, "START", "");
	iohandler.writeOut(toPrint, flags);
};


/*
Wrapper function for readSingleTextRecord()
*/
void Dissassemble::readTextRecords() {
	do
		readSingleTextRecord();
	while (currLine[0] == 'T');
};


/*
	There's a lot of stuff going on in this method (I'm working on refactoring it), but for 
now this is the jist of whats happening:
1) Read in a text record
2) Set up SIC_LineBuilder with all the necessary info
3) Lets IOHandler do the rest of the work
*/
void Dissassemble::readSingleTextRecord() {

	iohandler.getLine(currLine);

	if (isModOrEndRecord(currLine))
		return;

	//	We could ignore this part, BUT if something went wrong in the previous text record 
	// the progctr variable *could* also be wrong, so we set it to the correct value.
	progctr = Convert::hexToDecimal(currLine.substr(RECORD_ADDR_POS, RECORD_ADDR_LTH));

	for (int i = TEXT_REC_START_POS; i < currLine.size();) {

		// This should probably be its own method...
		// Don't really know how to do that though because of the continue ;_;
		if (littab.hasLiteralAt(progctr)) {
			int poolLength = iohandler.writeOutLtorg(littab, progctr);
			updateProgctr(poolLength / 2);
			i += poolLength;
			continue;
		}

		//	See page 38, 1st paragraph of Clean Code; This "section" of code violates EVERYTHING 
		// that he said shouldn't be violated. R E F A C T O R   T H I S
		//
		//  "The solution to this problem... is to bury [it] in the basement of an AbstractFactor, 
		// and never let anyone see it."
		SIC_LineBuilder line(symtab, currLine, i, progctr);
		bool flagSet = true;
		if (line.format == 2)
			handleFormatTwo(line, i);
		else if (line.format == 4)
			flagSet = handleFormatThreeAndFour(line, i);

		if (!flagSet || isWordByteDirective(line)) {
			handleWordByteDirectives(line, i);
			break;
		}

		// multiply currFormat by 2 b/c input is in half-bytes
		i += line.format * 2;
		iohandler.writeOut(line, flags);
		updateProgctr(line.format);

	}

	handleReservationDirectives();

};


void Dissassemble::readEndRecord() {

	while (currLine[REC_TYPE_FLAG_POS] != END)
		iohandler.getLine(currLine);

	int addr = Convert::hexToDecimal(currLine.substr(RECORD_ADDR_POS));
	string operand = symtab.getSymbol(addr);
	SIC_LineBuilder toPrint("", "END", progName);
	iohandler.writeOut(toPrint, flags);

};


bool Dissassemble::isModOrEndRecord(string &lineToCheck) {
	return	lineToCheck[REC_TYPE_FLAG_POS] == MODIFICATION ||
			lineToCheck[REC_TYPE_FLAG_POS] == END;
};


/*
Parameter:	int offset - The format of the current opcode
*/
void Dissassemble::updateProgctr(int offset) {
	progctr += offset;
};


void Dissassemble::handleFormatOne(SIC_LineBuilder &line) {
	//Turns out, you dont need a method for this, but I'm keeping it as a reminder to myself
};


void Dissassemble::handleFormatTwo(SIC_LineBuilder &line, int index) {

	const int REG_1_START_POS = index + 2;
	const int REG_2_START_POS = index + 3;

	int registerMnemonic = stoi(currLine.substr(REG_1_START_POS, 1));
	string registerOne = registerTable.at(registerMnemonic);
	line.operand.append(registerOne);

	// Handle special case opcodes
	if (line.opcode == "clear" || line.opcode == "tixr")
		return;

	registerMnemonic = stoi(currLine.substr(REG_2_START_POS, 1));
	string registerTwo = registerTable.at(registerMnemonic);
	line.operand.push_back(',');
	line.operand.append(registerTwo);

};


/*
Return:		false if the flag was NOT set, otherwise true
*/
bool Dissassemble::handleFormatThreeAndFour(SIC_LineBuilder &line, int index) {

	// If the flags didn't get set something weird is happening, so return
	if (!flags.setFlags(currLine, index))
		return false;

	line.format = flags.getIsExtended() ? 4 : 3;
	line.setDisplacement(currLine, index);
	int addr = calculateTargetAddress(line);
	line.setOperand(symtab, littab, flags, addr);

	baseRegisterVal = (line.opcode == "ldb") ? addr : baseRegisterVal;

	return true;

};


/*
If you're reading this method, I AM SORRY.

Checks if the current info if "nonsense". If it is, then we know that we're
dealing with a WORD or BYTE Directive.

Parameter:	SIC_LineBuilder &line - the line we're creating
Return:		true if it is a WORD/BYTE directive, otherwise false.
*/
bool Dissassemble::isWordByteDirective(SIC_LineBuilder &line) {

	// is format4 && immediate OR indirect set
	if (flags.getIsExtended() && (flags.getIsImmediate() || flags.getIsIndirect()))
		return false;

	int targetAddr = calculateTargetAddress(line);

	// While its possible this may be a legit line, it makes no sense for it to happen so 
	//  I'm willing to risk it.
	// Basically saying: are you using BASE to get to a something in memory that comes 
	//  before the BASE address?
	if (flags.getIsBaseRelative() && targetAddr < 0)
		return true;

	bool formatsDoNotMatch = Optab::getFormat(line.mnemonic) != line.format &&
		Optab::getFormat(line.mnemonic) != 4;
	if (formatsDoNotMatch)
		return true;

	if (line.format <= 2)
		return false;

	// unless its format 1 or 2, one or both of the immediate/indirect flags have to be set
	bool isImmediateOrIndirect = !flags.getIsImmediate() && !flags.getIsIndirect();
	if (isImmediateOrIndirect)
		return true;

	if (flags.getIsBaseRelative() && flags.getIsPcRelative() && !flags.getIsImmediate())
		return true;
	if (!flags.getIsBaseRelative() && !flags.getIsPcRelative() && !flags.getIsImmediate())
		return true;

	bool isOutOfRange = targetAddr < 0 || targetAddr > progLength;
	if (isOutOfRange)
		return true;

	return false;

};


/*
Calculates what the line's operand is.

Parameter:	SIC_LineBuilder &line - the line we're creating
Return:		The target address
*/
int Dissassemble::calculateTargetAddress(SIC_LineBuilder &line) {

	if (line.format == 4)
		return line.displacement;

	int modifier = flags.getIsBaseRelative() ? baseRegisterVal : progctr + line.format;
	if (!flags.getIsBaseRelative() && !flags.getIsPcRelative())
		modifier = 0;

	if (line.isDispNegative)
		return modifier - line.displacement;

	return line.displacement + modifier;

};


void Dissassemble::handleWordByteDirectives(SIC_LineBuilder &line, int index) {

	// We want to reset the flags because they're set to whatever was printed earlier
	flags.resetFlags();

	// set format to an invalid format so that nothing weird prints
	line.format = -1;

	while (progctr < progLength && index < currLine.size()) {

		if (littab.hasLiteralAt(progctr))
			break;

		int nextSymbolAddr = symtab.getNextSymbol(progctr).getValue();
		nextSymbolAddr = (nextSymbolAddr == progctr || nextSymbolAddr == INT_MIN) ? progLength : nextSymbolAddr;

		int delta = nextSymbolAddr - progctr;
		int lthToReadIn = delta * 2;

		line.symbol = symtab.getSymbol(progctr);
		line.opcode = (delta % 3 == 0) ? "WORD" : "BYTE";
		line.operand = currLine.substr(index, lthToReadIn);

		updateProgctr(delta);
		index += lthToReadIn;

		iohandler.writeOut(line, flags);

	}

};


/*
Handles the RESW && RESB assembler directives.
*/
void Dissassemble::handleReservationDirectives() {

	string nextLine = iohandler.peekNextLine();
	int nextLineAddr = Convert::hexToDecimal(nextLine.substr(RECORD_ADDR_POS, RECORD_ADDR_LTH));
	if (isModOrEndRecord(nextLine) || nextLineAddr == 0)
		nextLineAddr = progLength;

	int delta = INT_MIN;
	while (progctr < nextLineAddr && progctr < progLength && delta != 0) {

		SIC_LineBuilder line;
		line.buildReswLine(symtab, delta, progctr, progLength);

		iohandler.writeOut(line, flags);
		updateProgctr(delta);

	}

};


void Dissassemble::stopper() {
	// For debugging cause idk how to do that yet with Visual Studio
	int stop;
	cout << "\t\t\t\t\t";
	cin >> stop;
	cout << endl;
}