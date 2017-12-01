#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "Dissassemble.h"


#define DEBUG				0
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

const unordered_map<int, string> Dissassemble::registerTable {
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
#if DEBUG
	cout << currLine << endl;
#endif

	progName = currLine.substr(RECORD_ADDR_POS, 6);
	progLength = Convert::hexToDecimal(currLine.substr(PROG_LTH_OFFSET));
	//writeOut();
	tempWriteOut(progName, "START", "", -1);
};

void Dissassemble::readTextRecord() {

	iohandler.getLine(currLine);

	if (currLine[0] == 'M' || currLine[0] == 'E')
		return;
	
	// We could ignore this part, BUT if something weent wrong in the previous text record 
	//   the progctr variable *could* also be wrong, so might as well set it to the 
	//   correct value.
	setProgctr(Convert::hexToDecimal(currLine.substr(RECORD_ADDR_POS, 6)));

	for (int i = TEXT_REC_START_POS; i < currLine.size();) {

		#if DEBUG
			cout << "------------------TopOfTheLoopToYa----------------------" << endl;
			cout << "progctr:\t" << progctr << endl;
			cout << "substr(i):\t" << currLine.substr(i) << endl;
			stopper();
		#endif

		if (littab.hasLiteralAt(progctr)) {
			//int poolLength = writeOutLtorg();
			int poolLength = tempWriteOutLtorg();
			i += poolLength;
			continue;
		}
				
		string currMnemonic = Optab::getMnemonic(currLine[i], currLine[i + 1]);
		string currOpcode = Optab::getOpcode(currMnemonic);
		int currFormat = Optab::getFormat(currMnemonic);

		#if DEBUG
			cout << "currMnemonic\t" << currMnemonic << endl;
			cout << "currOpcode\t" << currOpcode << endl;
			cout << "currFormat\t" << currFormat << endl;
			stopper();
		#endif

		string currOperand;
		string currSymbol = symtab.getSymbol(progctr);

		#if DEBUG
			cout << "progctr:\t" << progctr << endl;
			cout << "currOperand:\t" << currOperand << endl;
			cout << "currSymbol:\t" << currSymbol << endl;
			stopper();
		#endif

		
		bool flagSet = true;
		if (currFormat == 2)
			formatTwo(currOpcode, currOperand, i);
		else if (currFormat == 4) {
			
			flagSet = flags.setFlags(currLine, i);
		
			#if DEBUG
				cout << "format 3/4" << endl;
				cout << "flagset:\t" << (flagSet ? "treu" : "false") << endl;
				cout << "n:\t" << (flags.getIsIndirect() ? "true" : "false") << endl;
				cout << "i:\t" << (flags.getIsImmediate() ? "true" : "false") << endl;
				cout << "x:\t" << (flags.getIsIndexAddressing() ? "true" : "false") << endl;
				cout << "b:\t" << (flags.getIsBaseRelative() ? "true" : "false") << endl;
				cout << "p:\t" << (flags.getIsPcRelative() ? "true" : "false") << endl;
				cout << "e:\t" << (flags.getIsExtended() ? "true" : "false") << endl;
				cout << endl;
			#endif

			currFormat = flags.getIsExtended() ? 4 : 3;

			if (flagSet && currFormat == 3) 
				formatThree(currOpcode, currOperand, i);
			else if(flagSet)
				formatFour(currOpcode, currOperand, i);
			
		}

		#if DEBUG
			cout << "currOperand:\t" << currOperand << endl;
			cout << "currSymbol:\t" << currSymbol << endl;
			stopper();
			cout << endl << endl;
		#endif

		//REFACTOR THIS
		// If the flags weren't set then we know something weird is going on
		if (!flagSet || isWordOrByteDirective(currSymbol, currMnemonic, currOperand, currFormat)) {
			#if DEBUG_WORD_BYTE
				cout << "dealing with word/byte ass. dir." << endl;
				stopper();
			#endif

			wordByte(currSymbol, currOpcode, currOperand, i);
			break;
		}
		

		// multiply currFormat by 2 b/c input is in half-bytes
		i += currFormat * 2;

		tempWriteOut(currSymbol, currOpcode, currOperand, currFormat);

		updateProgctr(currFormat);

		#if DEBUG
			stopper();
		#endif

	}

	reswResb();

};

void Dissassemble::readEndRecord() {
	while(currLine[0] != 'E')
		iohandler.getLine(currLine);

	string hexAddr = currLine.substr(RECORD_ADDR_POS);
	int addr = Convert::hexToDecimal(hexAddr);
	string operand = symtab.getSymbol(addr);

	//writeOut();
	tempWriteOut("", "END", operand, -1);

};

int Dissassemble::calculateTargetAddress(int displacement, bool isDispNegative, int format) {
	
	int modifier = flags.getIsBaseRelative()? base : progctr + format;
	if (!flags.getIsBaseRelative() && !flags.getIsPcRelative())
		modifier = 0;

	if (isDispNegative)
		return modifier - displacement;

	return displacement + modifier;
	
};

int Dissassemble::calculateTargetAddress(string hexVal, bool isDispNegative, int format) {
	return calculateTargetAddress(Convert::hexToDecimal(hexVal), isDispNegative, format);
};

void Dissassemble::updateProgctr(int offset) {
	progctr += offset;
};

void Dissassemble::setProgctr(int val) {
	progctr = val;
};

void Dissassemble::formatOne(string &opcode) {
	//TODO: code this
};

void Dissassemble::formatTwo(string &opcode, string &operand, int index) {

	int temp = stoi(currLine.substr(index + 2, 1));
	string registerOne = registerTable.at(temp);
	operand.append(registerOne);
	
	if (opcode == "clear" || opcode == "tixr") 
		return;
	
	temp = stoi(currLine.substr(index + 3, 1));
	string registerTwo = registerTable.at(temp);
	operand.push_back(',');
	operand.append(registerTwo);

};

void Dissassemble::formatThree(string &opcode, string &operand, int index) {

	bool isDispNegative = currLine[index + DISPLACEMENT_OFFSET] == 'F' ? true : false;
	string displacement = currLine.substr(index + DISPLACEMENT_OFFSET, 3);
	int disp = disp = Convert::hexToDecimal(displacement);
	if (isDispNegative)
		disp = Convert::negativeHexToPositiveDecimal(displacement);

	int targetAddr = calculateTargetAddress(disp, isDispNegative, 3);

	bool isRelativeToSomething = flags.getIsBaseRelative() != flags.getIsPcRelative();
	if (symtab.getSymbol(targetAddr) != "" && isRelativeToSomething)
		operand = symtab.getSymbol(targetAddr);
	else if (littab.getLiteral(targetAddr) != "" && isRelativeToSomething)
		operand = littab.getLiteral(targetAddr);
	else
		operand = to_string(targetAddr);

	base = (opcode == "ldb") ? targetAddr : base;

};

void Dissassemble::formatFour(string &opcode, string &operand, int index) {

	/*TODO: MAKE SURE YOU DONT HAVE TO DEAL WITH INDEXED AND INDIRECT DIFFERENTLY*/

	#if DEBUG
		cout << "format 4" << endl;
	#endif

	string tempAddr = currLine.substr(index + DISPLACEMENT_OFFSET, 5);
	int address = Convert::hexToDecimal(tempAddr);

	if (symtab.getSymbol(address) != "")
		operand = symtab.getSymbol(address);
	else if (littab.getLiteral(address) != "")
		operand = littab.getLiteral(address);
	else
		operand = to_string(address);

	base = (opcode == "ldb") ? address : base;

#if DEBUG
	cout << "returning from f4()" << endl;
#endif

};

// Needs refactoring
bool Dissassemble::isWordOrByteDirective(string &symbol, string &mnemonic, string &operand, int &format) {

#if DEBUG_WORD_BYTE
	cout << "----------isWordOrByteDirective----------" << endl;
#endif

	//(isImmediate != isIndirect) == isImmediate XOR isIndirect
	if (flags.getIsExtended()&& (flags.getIsImmediate()!= flags.getIsIndirect()))
		return false;

#if DEBUG_WORD_BYTE
	cout << "currSymbol:\t" << symbol << endl;
	cout << "mnemonic:\t" << mnemonic << endl; 
	cout << "operand:\t" << operand << endl;
#endif

	int disp = getDisplacement(operand, format);
	disp = flags.getIsBaseRelative() && /*!*/isOperandNumber(operand) ? 
			calculateTargetAddress(disp, (disp < 0), format) : disp;

#if DEBUG_WORD_BYTE
	cout << "disp:\t\t" << disp << endl;
	cout << "format:\t\t" << format << endl;
	stopper();
#endif

	if (Optab::getFormat(mnemonic) != format && Optab::getFormat(mnemonic) != 4) {
#if DEBUG_WORD_BYTE
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}

	if (format == 1 || format == 2)
		return false;

	// unless its format 1 or 2, one or both of the immediate/indirect flags has to be set
	if (!flags.getIsImmediate() && !flags.getIsIndirect()) {
#if DEBUG_WORD_BYTE
		cout << "both immediate and indirect are false" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}



	if (flags.getIsBaseRelative() && flags.getIsPcRelative() && !flags.getIsImmediate()) {
#if DEBUG_WORD_BYTE
		cout << "both isBaseRelative and isPcRelative are true" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}

	if (!flags.getIsBaseRelative() && !flags.getIsPcRelative() && !flags.getIsImmediate()) {
#if DEBUG_WORD_BYTE		
		cout << "both isBaseRelative and isPcRelative are false" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}

	// check if disp is out of scope (i.e. is it < 0 || is it > proglth?)
	if (disp < 0 || disp > progLength) {
#if DEBUG_WORD_BYTE		
		cout << "checking if addr/disp is out of the scope of the program" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;

	}

	// There are probably more things to check


#if DEBUG_WORD_BYTE
	cout << "@end" << endl;
	cout << "returning false" << endl;
	//stopper();
#endif
	return false;

}

void Dissassemble::wordByte(string &symbol, string &directive, string &operand, int index) {
	
	// reset the flags so that they don't print weird stuff out
	flags.resetFlags();

	while (progctr < progLength && index < currLine.size()) {

		//wanna check if there's a literal here and break if there is one
		if (littab.hasLiteralAt(progctr))
			break;

		// Update symbol
		symbol = symtab.getSymbol(progctr);

		int nextSymAddr = symtab.getNextSymbol(progctr).getValue();
		nextSymAddr = (nextSymAddr == progctr || nextSymAddr == INT_MIN) ? progLength : nextSymAddr;

		// Subtract that val from the current addr - this is the number of bytes to be read in
		int delta = nextSymAddr - progctr;
		int lthToReadIn = delta * 2;

		// Update operand
		operand = currLine.substr(index, lthToReadIn);
		
		// Update directive
		directive = (delta % 3 == 0) ? "WORD" : "BYTE";
		
		updateProgctr(delta);
		index += lthToReadIn;

		tempWriteOut(symbol, directive, operand, -1);

	}
	
};

void Dissassemble::reswResb() {

#if DEBUG_RWORD_RBYTE
	cout << "progctr:\t" << progctr << endl;
	stopper();
#endif // DEBUG_RWORD_RBYTE

	string nextLine = peekNextLine();

	int nextLineAddr = Convert::hexToDecimal(nextLine.substr(RECORD_ADDR_POS, 6));
	if (nextLine[0] == 'M' || nextLine[0] == 'E' || nextLineAddr == 0)
		nextLineAddr = progLength;

	int delta = INT_MIN;

#if DEBUG_RWORD_RBYTE
	cout << "nextLine:\t" << nextLine << endl;
	cout << "nextLineAddr:\t" << Convert::decimalToHex(nextLineAddr) << endl;
	cout << "progLength:\t" << Convert::decimalToHex(progLength) << endl;
	cout << "progctr:\t" << progctr << endl;
	stopper();
#endif // DEBUG_RWORD_RBYTE

	bool temp = false;

	while (progctr < nextLineAddr && progctr < progLength && delta != 0) {

		temp = true;

#if DEBUG_RWORD_RBYTE
		cout << "-------------RESB_RESW-----------" << endl;
		//cout << "(3)progctr:\t" << Convert::decimalToHex(progctr) << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		string currSymbol = symtab.getSymbol(progctr);

#if DEBUG_RWORD_RBYTE
		cout << "currSymbol:\t" << currSymbol << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		int nextSymbolAddr = symtab.getNextSymbol(progctr).getValue();
		// Make sure it has the right value
		nextSymbolAddr = (nextSymbolAddr == INT_MIN) ? progLength : nextSymbolAddr;

#if DEBUG_RWORD_RBYTE
		cout << "?!?!?!?!" << endl;
		cout << "nextSymbolAddr:\t" << Convert::decimalToHex(nextSymbolAddr) << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		delta = nextSymbolAddr - progctr;

#if DEBUG_RWORD_RBYTE
		cout << "delta:\t\t" << delta << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		string assDirective = "RESB";
		string operand = to_string(delta);
		if (delta % 3 == 0) {
			assDirective = "RESW";
			operand = to_string(delta / 3);
		}

#if DEBUG_RWORD_RBYTE
		cout << "assDir:\t\t" << assDirective << endl;
		cout << "operand:\t" << operand << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		//writeOut(currSymbol, assDirective, operand, -1);
		tempWriteOut(currSymbol, assDirective, operand, -1);

		updateProgctr(delta);
		
#if DEBUG_RWORD_RBYTE
		cout << "progctr:\t" << Convert::decimalToHex(progctr) << endl;
		cout << "nextLineAddr:\t" << Convert::decimalToHex(nextLineAddr) << endl;
		cout << "progLength:\t" << Convert::decimalToHex(progLength) << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

	}

	if (nextLine[0] != 'T' || !temp)
		return;

	cout << "\t\t." << endl;
	cout << "\t\t. new subroutine?!?!?" << endl;
	cout << "\t\t." << endl;

};

string Dissassemble::peekNextLine() {
	return iohandler.peekNextLine();
};

int Dissassemble::getDisplacement(string &operand, int &format) {
	if (format <= 2)
		return 0;
	int symAddr = symtab.getAddress(operand);
	int litAddr = littab.getAddress(operand);

#if DEBUG
	cout << "----------getDisplacement----------" << endl;
	cout << "symAddr:\t" << symAddr << endl;
	cout << "litAddr:\t" << litAddr << endl;
#endif
	if (symAddr == -1 && litAddr == -1) {
#if DEBUG
		cout << "returning:\t" << stoi(operand) << endl;
		stopper();
#endif
		return stoi(operand);
	}
	else if (symAddr == -1) {
#if DEBUG
		cout << "returning:\t" << litAddr << endl;
		stopper();
#endif
		return litAddr;
	}
#if DEBUG
	cout << "returning:\t" << symAddr << endl;
	stopper();
#endif
	return symAddr;
};

bool Dissassemble::isOperandNumber(string &operand) {
	int symAddr = symtab.getAddress(operand);
	int litAddr = littab.getAddress(operand);
	return (symAddr == -1 && litAddr == -1) ? true : false;
};

void Dissassemble::writeOut(string symbol, string opcode, string operand, int currFormat) {
	iohandler.writeOut(symbol, opcode, operand, currFormat, flags);
};

int Dissassemble::writeOutLtorg() {
	int poolLength = iohandler.writeOutLtorg(littab, progctr);
	updateProgctr(poolLength / 2);
	return poolLength;
};

void Dissassemble::tempWriteOut(string symbol, string opcode, string operand, int currFormat) {
	iohandler.tempWriteOut(symbol, opcode, operand, currFormat, flags, progctr);
};

int Dissassemble::tempWriteOutLtorg() {
	int poolLength = iohandler.tempWriteOutLtorg(littab, progctr);
#if DEBUG_WRITE_OUT
	cout << "poopoo lenght:\t" << poolLength << endl;
#endif
	updateProgctr(poolLength / 2);
	return poolLength;

}

void Dissassemble::stopper() {
	// For debugging cause idk how to do that yet with Visual Studio
	int stop;
	cout << "\t\t\t\t\t";
	cin >> stop;
}

