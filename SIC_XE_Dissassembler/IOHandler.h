#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#pragma once
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <stdexcept>

#include "Flags.h"
#include "LiteralTable.h"

#define DEBUG 0
#define DEBUG_PRINT 0
#define DEBUG_WORD_BYTE 0
#define DEBUG_RWORD_RBYTE 0
#define DEBUG_TA 0
#define DEBUG_FORMAT_3 0
#define LISTING_FILE 1

using namespace std;

class IOHandler {
public:
	IOHandler() { };

	IOHandler(string inputPath, string outputPath) {

		inFile.open(inputPath.c_str(), ios::in);
		outFile.open(outputPath.c_str(), ios::out | ios::ate | ios::app);

		if (!inFile) {
			cout << "INPUT_FILE DOESN'T EXIST; PANIC!" << endl;
			this->close();
			exit(EXIT_SUCCESS);
		}

	};

	static IOHandler open(string inputPath, string outputPath) {
		return IOHandler(inputPath, outputPath);
	};	

	void close() {
		this->inFile.close();
		this->outFile.close();
	};

	void writeOut(string symbol, string opcode, string operand, int currFormat, Flags &flags) {

		// Write out symbol column
		outFile << setw(9) << left << symbol;

		transform(opcode.begin(), opcode.end(), opcode.begin(), ::toupper);

		// Write out opcode column
		if (flags.getIsExtended())
			outFile << "+" << opcode << setw(6) << right;
		else
			outFile << setw(9) << left << opcode;

		// Write out operand column
		if (flags.getIsImmediate() && !flags.getIsSimpleAddressing() && currFormat >= 3)
			outFile << "#" << operand << setw(4) << left;
		else if (flags.getIsIndirect() && !flags.getIsSimpleAddressing() && currFormat >= 3)
			outFile << "@" << operand << setw(4) << left;
		else if (flags.getIsIndexAddressing() && currFormat >= 3)
			outFile << operand << ",X" << setw(4) << left;
		else
			outFile << "" << operand << setw(4) << left;

		outFile << endl;

		// Handle a special case 
		if (opcode == "ldb")
			outFile << setw(9) << left << "" << setw(9) << left << "BASE" << setw(12) << left << operand << endl;

	};

	//for testing only
	void tempWriteOut(string symbol, string opcode, string operand, int currFormat, Flags &flags, int progctr) {

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
		if (flags.getIsExtended())
			cout << "+" << opcode << setw(6) << right;
		else {
			cout << setw(9) << left << opcode;
		}

		// Write out operand column
		if (flags.getIsImmediate() && !flags.getIsSimpleAddressing() && currFormat >= 3)
			cout << "#" << operand << setw(4) << left;
		else if (flags.getIsIndirect() && !flags.getIsSimpleAddressing() && currFormat >= 3)
			cout << "@" << operand << setw(4) << left;
		else if (flags.getIsIndexAddressing() && currFormat >= 3)
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

	int writeOutLtorg(LiteralTable &littab, int currAddr) {

		if (!littab.hasLiteralAt(currAddr))
			return 0;

		int poolLength = 0;
		while (littab.getLiteral(currAddr) != "") {
			poolLength = littab.getLength(currAddr);
			currAddr += (littab.getLength(currAddr) / 2);
		}

		outFile << "\t\tLTORG" << endl;

		return poolLength;

	};

	//For testing
	int tempWriteOutLtorg(LiteralTable &littab, int progctr) {

		if (!littab.hasLiteralAt(progctr))
			return 0;

#if LISTING_FILE
		string temp = Convert::decimalToHex(to_string(progctr));
		cout << setw(8) << left << temp;
#endif
		cout << setw(14) << right << "LTORG" << endl;

		int poolLength = 0;
		while (littab.getLiteral(progctr) != "") {
			poolLength += littab.getLength(progctr);

#if LISTING_FILE
			cout << setw(25) << right << littab.getLiteral(progctr) << endl;
#endif

			progctr += (littab.getLength(progctr) / 2);

		}

		return poolLength;

	};

	//https://stackoverflow.com/questions/10268872/c-fstream-function-that-reads-a-line-without-extracting
	string peekNextLine() {
		string nextLine;

		// Get current position
		int len = inFile.tellg();

		// Read next line
		getline(inFile, nextLine);

		// Go back to where you once belonged DEMON!
		inFile.seekg(len, ios_base::beg);

		return nextLine;
	};

	void getLine(string &holder) {
		getline(inFile, holder);
	};

private:
	ifstream inFile;
	ofstream outFile;

};
#endif
