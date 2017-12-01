#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "Dissassemble.h"

#define DEBUG 0
#define DEBUG_PRINT 0
#define DEBUG_WORD_BYTE 0
#define DEBUG_RWORD_RBYTE 0
#define DEBUG_TA 0
#define DEBUG_FORMAT_3 0
#define LISTING_FILE 1

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

	isIndirect = false;
	isImmediate = false;
	isSimpleAddressing = false;
	isIndexAddressing = false;
	isBaseRelative = false;
	isPcRelative = false;
	isExtended = false;

	base = 0;

	progctr = 0;
	progLength = 0;

	inFile.open(inputPath.c_str(), ios::in);
	outFile.open(outputPath.c_str(), ios::out | ios::ate | ios::app);

};

void Dissassemble::disassemble() {

	readHeadRecord();
	
	int i = 0;	//~~~~temp~~~~
	do {
#if DEBUG_RWORD_RBYTE
		cout << "i:\t" << i << endl;
#endif // DEBUG_RWORD_RBYTE

		readTextRecord();
		
		//~~~~temp~~~~
		if(i++ % 2 == 1) {
			cout	<< "\t." << endl
					<< "\t.\tnew subroutine?" << endl
					<< "\t." << endl;		
		}

	} while (currLine[0] == 'T');

	readEndRecord();

};

void Dissassemble::readHeadRecord() {
	getline(inFile, currLine);
	progName = currLine.substr(RECORD_ADDR_POS, 6);
	progLength = Convert::hexToDecimal(currLine.substr(PROG_LTH_OFFSET));
	//writeOut();
	tempWriteOut(progName, "START", "", -1);
};

void Dissassemble::readTextRecord() {

	/*NOTE: THIS ONLY WORKS FOR ONE LINE IN THE TEXT RECORD.
			IF THERE'S MORE THAN 1 TEXT REC, THEN YOU NEED TO 
			  SURROUND THE FUNCTION CALL IN A FOR LOOP	*/

	getline(inFile, currLine);

	if (currLine[0] == 'M' || currLine[0] == 'E') {
		reswResb();
		return;
	}

	// We could ignore this part, BUT if something weent wrong in the previous text record 
	//   the progctr variable *could* also be wrong, so might as well set it to the 
	//   correct value.
	setProgctr(Convert::hexToDecimal(currLine.substr(RECORD_ADDR_POS, 6)));


	/* TODO: #2???? */

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
			
			flagSet = setFlags(i);
		
			#if DEBUG
				cout << "format 3/4" << endl;
				cout << "n:\t" << (isIndirect ? "true" : "false") << endl;
				cout << "i:\t" << (isImmediate ? "true" : "false") << endl;
				cout << "x:\t" << (isIndexAddressing ? "true" : "false") << endl;
				cout << "b:\t" << (isBaseRelative ? "true" : "false") << endl;
				cout << "p:\t" << (isPcRelative ? "true" : "false") << endl;
				cout << "e:\t" << (isExtended ? "true" : "false") << endl;
				cout << endl;
			#endif

			currFormat = isExtended ? 4 : 3;

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

	//write out resw/resb
	reswResb();

};

void Dissassemble::readEndRecord() {
	while(currLine[0] != 'E')
		getline(inFile, currLine);

	string hexAddr = currLine.substr(RECORD_ADDR_POS);
	int addr = Convert::hexToDecimal(hexAddr);
	string operand = symtab.getSymbol(addr);

	//writeOut();
	tempWriteOut("", "END", operand, -1);

};

int Dissassemble::calculateTargetAddress(int displacement, bool isDispNegative, int format) {
	
#if DEBUG_TA
	cout << endl;
	cout << "-----------------calculateTargetAddress-----------------" << endl;
	cout << "displacement:\t" << displacement << endl;
	cout << "isDispNegative:\t" << (isDispNegative ? "true" : "fase") << endl;
	cout << "isBaseRelative:\t" << (isBaseRelative ? "true" : "fase") << endl;
	cout << "base:\t" << base << endl;
	cout << "format:\t" << format << endl;
#endif

	int modifier = isBaseRelative ? base : progctr + format;
	if (!isBaseRelative && !isPcRelative)
		modifier = 0;

#if DEBUG_TA
	cout << "modifier:\t" << modifier << endl;
	stopper();
#endif

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
	
	/*TODO: MAKE SURE YOU DONT HAVE TO DEAL WITH INDEXED AND INDIRECT DIFFERENTLY*/
	/*TODO: RSUB SHOULD BE HANDLED DIFFERENTLY -> IT'S A SPECIAL CASE*/

	bool isDispNegative = currLine[index + DISPLACEMENT_OFFSET] == 'F' ? true : false;
	string displacement = currLine.substr(index + DISPLACEMENT_OFFSET, 3);
	int disp = disp = Convert::hexToDecimal(displacement);
	if (isDispNegative)
		disp = Convert::negativeHexToPositiveDecimal(displacement);

	int targetAddr = calculateTargetAddress(disp, isDispNegative, 3);

#if DEBUG_FORMAT_3 
	cout << endl << endl;
	cout << "targetAddr:\t" << targetAddr << endl;
	cout << "isDispNegative:\t" << (isDispNegative ? "true" : "false") << endl;
	cout << "disp:\t\t" << disp << endl;
	stopper();
#endif

	bool isRelativeToSomething = isBaseRelative != isPcRelative;
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
	if (isExtended && (isImmediate != isIndirect))
		return false;

#if DEBUG_WORD_BYTE
	cout << "currSymbol:\t" << symbol << endl;
	cout << "mnemonic:\t" << mnemonic << endl; 
	cout << "operand:\t" << operand << endl;
#endif

	int disp = getDisplacement(operand, format);
	disp = isBaseRelative && /*!*/isOperandNumber(operand) ? calculateTargetAddress(disp, (disp < 0), format) : disp;

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

	// unless its format 1 or 2, one or both of the immediate/indirect flags has to be set
	if ((format == 3 || format == 4) && !isImmediate && !isIndirect) {
#if DEBUG_WORD_BYTE
		cout << "both immediate and indirect are false" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}

	if ((format == 3 || format == 4) && isBaseRelative && isPcRelative && !isImmediate) {
#if DEBUG_WORD_BYTE
		cout << "both isBaseRelative and isPcRelative are true" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}

	if ((format == 3 || format == 4) && !isBaseRelative && !isPcRelative && !isImmediate) {
#if DEBUG_WORD_BYTE		
		cout << "both isBaseRelative and isPcRelative are false" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;
	}

	// check if disp is out of scope (i.e. is it < 0 || is it > proglth?)
	if (format == 3 && (disp < 0 || disp > progLength)) {
#if DEBUG_WORD_BYTE		
		cout << "checking if addr/disp is out of the scope of the program" << endl;
		cout << "returning true" << endl;
		stopper();
#endif
		return true;

	}
	// check if addr is larger than proglength
	else if (format == 4 && disp > progLength) {
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
	resetFlags();

	while (progctr < progLength && index < currLine.size()) {

		//wanna check if there's a literal here and break if there is one

		// Update symbol
		symbol = symtab.getSymbol(progctr);

		int nextSymAddr = symtab.getNextSymbol(progctr).getValue();
		nextSymAddr = (nextSymAddr == progctr) ? progLength : nextSymAddr;

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




	/*
	1.	need to peek next line to figure out where it starts -> call nextLineAddr
		a.	if its the same as progctr then return
	2.	get the current symbol at the current addr
	3.	get the next symbol
	4.	find the delta between the two
	5.	if divisible by 3 then its RESW; otherwise its RESB
	6.	if its resw then divide the delta by 3
		a.	the delta can be used as the operand
	7.	Print out!
	8.	update the progctr by the delta(make sure to account for the division by 
		  3 -> so maybe do this before that?)
	9.	goTo 1a
	*/

	//1
	string nextLine = peekNextLine();

	int nextLineAddr = Convert::hexToDecimal(nextLine.substr(RECORD_ADDR_POS, 6));
	if (nextLine[0] == 'M' || nextLine[0] == 'E' || nextLineAddr == 0)
		return;/*nextLineAddr = progLength;*/

	int delta = INT_MIN;

#if DEBUG_RWORD_RBYTE
	cout << "nextLine:\t" << nextLine << endl;
	cout << "nextLineAddr:\t" << Convert::decimalToHex(nextLineAddr) << endl;
	cout << "progLength:\t" << Convert::decimalToHex(progLength) << endl;
	stopper();
#endif // DEBUG_RWORD_RBYTE

	//1a
	while (progctr < nextLineAddr && progctr < progLength && delta != 0) {

#if DEBUG_RWORD_RBYTE
		cout << "-------------RESB_RESW-----------" << endl;
		//cout << "(3)progctr:\t" << Convert::decimalToHex(progctr) << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		//2
		string currSymbol = symtab.getSymbol(progctr);

#if DEBUG_RWORD_RBYTE
		cout << "currSymbol:\t" << currSymbol << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		//3
		int nextSymbolAddr = symtab.getNextSymbol(progctr).getValue();
		// Make sure it has the right value
		nextSymbolAddr = (nextSymbolAddr == INT_MIN) ? progLength : nextSymbolAddr;

#if DEBUG_RWORD_RBYTE
		cout << "nextSymbolAddr:\t" << Convert::decimalToHex(nextSymbolAddr) << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		//4
		delta = nextSymbolAddr - progctr;

#if DEBUG_RWORD_RBYTE
		cout << "delta:\t\t" << delta << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE

		//5 + 6
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

		//7
		//writeOut(currSymbol, assDirective, operand, -1);
		tempWriteOut(currSymbol, assDirective, operand, -1);

#if DEBUG_RWORD_RBYTE
		stopper();
#endif // DEBUG_RWORD_RBYTE

		//8
		updateProgctr(delta);
		
#if DEBUG_RWORD_RBYTE
		cout << "progctr:\t" << Convert::decimalToHex(progctr) << endl;
		cout << "nextLineAddr:\t" << Convert::decimalToHex(nextLineAddr) << endl;
		cout << "progLength:\t" << Convert::decimalToHex(progLength) << endl;
		stopper();
#endif // DEBUG_RWORD_RBYTE


	}

};

//https://stackoverflow.com/questions/10268872/c-fstream-function-that-reads-a-line-without-extracting
string Dissassemble::peekNextLine() {
	string nextLine;

	// Get current position
	int len = inFile.tellg();

	// Read next line
	getline(inFile, nextLine);

	// Go back to where you once belonged DEMON!
	inFile.seekg(len, ios_base::beg);

	return nextLine;
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
	
	// Write out symbol column
	outFile << symbol << "\t\t";

	// Write out opcode column
	if (isExtended)
		outFile << "+";
	
	outFile << opcode << "\t\t";

	// Write out operand column
	if (isImmediate && !isSimpleAddressing)
		outFile << "#";
	else if (isIndirect && !isSimpleAddressing)
		outFile << "@";

	outFile << operand;

	if (isIndexAddressing)
		outFile << ",X";

	outFile << endl;

	// Handle a special case 
	if (opcode == "ldb")
		outFile << "\t\t" << "BASE" << endl;

};

int Dissassemble::writeOutLtorg() {

	if (!littab.hasLiteralAt(progctr))
		return 0;

	int poolLength = 0;
	while (littab.getLiteral(progctr) != "") {
		poolLength = littab.getLength(progctr);
		updateProgctr(littab.getLength(progctr) / 2);
	}

	outFile << "\t\tLTORG" << endl;

	return poolLength;

};

void Dissassemble::tempWriteOut(string symbol, string opcode, string operand, int currFormat) {

	#if DEBUG_PRINT
		cout << "symbol:\t" << symbol << endl;
		cout << "opcode:\t" << opcode << endl;
		cout << "oper&:\t" << operand << endl;
		cout << "format:\t" << currFormat << endl;
		cout << "progctr:\t" << progctr << endl;
		stopper();
	#endif

	#if DEBUG || DEBUG_WORD_BYTE || DEBUG_PRINT
		cout << "/**********************************************************" << endl;
		cout << "*" << endl;
		cout << "*";
	#endif

	#if LISTING_FILE
		if (progctr >= 0) {
			string temp = to_string(progctr);
			string hexProgctr = Convert::decimalToHex(progctr);
			cout << setw(8) << left << hexProgctr;
		}
		else {
			cout << setw(8) << left << "--";
		}
	#endif

	// Write out symbol column
	cout << setw(9) << left << symbol;

	transform(opcode.begin(), opcode.end(), opcode.begin(), ::toupper);

	// Write out opcode column
	if (isExtended)
		cout << "+" << opcode << setw(6) << right;
	else {
		cout << setw(9) << left << opcode;
	}

	// Write out operand column
	if (isImmediate && !isSimpleAddressing && currFormat >= 3)
		cout << "#" << operand << setw(4) << left;
	else if (isIndirect && !isSimpleAddressing && currFormat >= 3)
		cout << "@" << operand << setw(4) << left;
	else if (isIndexAddressing && currFormat >= 3)
		cout << operand << ",X" << setw(4) << left;
	else
		cout << "" << operand << setw(4) << left;
	
	cout << endl;

	// Handle a special case 
	if (opcode == "ldb") {
		#if DEBUG || DEBUG_WORD_BYTE
			cout << "*";
		#endif
		#if LISTING_FILE
			string temp = Convert::decimalToHex(to_string(progctr));
			cout << setw(8) << left << "--";
		#endif
		cout << setw(9) << left << "" << setw(9) << left << "BASE" << setw(12) << left << operand << endl;
	}

#if DEBUG || DEBUG_WORD_BYTE || DEBUG_PRINT
	cout << "*\n**********************************************************/" << endl;
#endif
};

int Dissassemble::tempWriteOutLtorg() {

	if (!littab.hasLiteralAt(progctr))
		return 0;

	#if LISTING_FILE
		string temp = Convert::decimalToHex(to_string(progctr));
		cout << setw(8) << left << temp;
	#endif
	cout << setw(14) << right << "LTORG" << endl;

	int poolLength = 0;
	while (littab.getLiteral(progctr) != "") {
		poolLength = littab.getLength(progctr);
		
		#if LISTING_FILE
			cout << setw(25) << right << littab.getLiteral(progctr) << endl;
		#endif

		updateProgctr(littab.getLength(progctr) / 2);
	}

	return poolLength;

}

bool Dissassemble::setFlags(int index) {

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

void Dissassemble::resetFlags() {

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

void Dissassemble::stopper() {
	// For debugging cause idk how to do that yet with Visual Studio
	int stop;
	cout << "\t\t\t\t\t";
	cin >> stop;
}

