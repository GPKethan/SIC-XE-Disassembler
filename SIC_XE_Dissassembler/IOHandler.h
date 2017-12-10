#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#pragma once
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <stdexcept>

#include "SIC_LineBuilder.h"
#include "Flags.h"
#include "LiteralTable.h"

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

	void writeOut(SIC_LineBuilder &line, Flags &flags) {

		writeOutSymbol(line);
		writeOutOpcode(flags, line);
		writeOutOperand(flags, line);

		// Handle a special case 
		if (line.opcode == "ldb")
			/*outFile*/ cout << setw(9) << left << "" << setw(9) << left << "BASE" << setw(12) << left << line.operand << endl;

	};

	int writeOutLtorg(LiteralTable &littab, int currAddr) {

		if (!littab.hasLiteralAt(currAddr))
			return 0;

		/*outFile*/ cout << setw(14) << right << "LTORG" << endl;

		int poolLength = 0;
		while (littab.getLiteral(currAddr) != "") {
			poolLength += littab.getLength(currAddr);
			currAddr += (littab.getLength(currAddr) / 2);
		}

		return poolLength;

	};

	// "Borrowed" from:
	// https://stackoverflow.com/questions/10268872/c-fstream-function-that-reads-a-line-without-extracting
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

	void writeOutSymbol(SIC_LineBuilder &line) {
		/*outFile*/ cout << setw(9) << left << line.symbol;
	};

	void writeOutOpcode(Flags &flags, SIC_LineBuilder &line) {
		if (flags.getIsExtended())
			/*outFile*/ cout << "+" << line.opcode << setw(6) << right;
		else
			/*outFile*/ cout << setw(9) << left << line.opcode;
	};

	void writeOutOperand(Flags & flags, SIC_LineBuilder &line) {
		if (flags.getIsImmediate() && !flags.getIsSimpleAddressing() && line.format >= 3)
			/*outFile*/ cout << "#" << line.operand << setw(4) << left;
		else if (flags.getIsIndirect() && !flags.getIsSimpleAddressing() && line.format >= 3)
			/*outFile*/ cout << "@" << line.operand << setw(4) << left;
		else if (flags.getIsIndexAddressing() && line.format >= 3)
			/*outFile*/ cout << line.operand << ",X" << setw(4) << left;
		else
			/*outFile*/ cout << "" << line.operand << setw(4) << left;
		/*outFile*/ cout << endl;
	};

};
#endif
