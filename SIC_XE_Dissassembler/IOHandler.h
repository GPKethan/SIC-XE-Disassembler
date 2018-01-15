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
	IOHandler();

	IOHandler(string inputPath, string outputPath);

	void close();

	/*
	Gets a IOHandler object

	Parameter: string inputPath
	string outputPath
	*/
	static IOHandler open(string inputPath, string outputPath);

	/*
	Prints out the lineBuilder Object
	*/
	void writeOut(SIC_LineBuilder &line, Flags &flags);

	int writeOutLtorg(LiteralTable &littab, int currAddr);

	// "Borrowed" from:
	// https://stackoverflow.com/questions/10268872/c-fstream-function-that-reads-a-line-without-extracting
	string peekNextLine();

	void getLine(string &holder);

private:
	ifstream inFile;
	ofstream outFile;

	void writeOutSymbol(SIC_LineBuilder &line);

	void writeOutOpcode(Flags &flags, SIC_LineBuilder &line);

	void writeOutOperand(Flags & flags, SIC_LineBuilder &line);

};
#endif
