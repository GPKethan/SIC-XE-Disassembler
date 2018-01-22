#include "stdafx.h"
#include "IOHandler.h"

using namespace std;

IOHandler::IOHandler() {};

IOHandler::IOHandler(string inputPath, string outputPath) {

	inFile.open(inputPath.c_str(), ios::in);
	outFile.open(outputPath.c_str(), ios::out | ios::ate | ios::app);

	if (!inFile) {
		cout << "INPUT_FILE DOESN'T EXIST; PANIC!" << endl;
		this->close();
		exit(EXIT_SUCCESS);
	}

};

void IOHandler::close() {
	this->inFile.close();
	this->outFile.close();
};

/*
Gets a IOHandler object

Parameter: string inputPath
string outputPath
*/
IOHandler IOHandler::open(string inputPath, string outputPath) {
	return IOHandler(inputPath, outputPath);
};

/*
Prints out the lineBuilder Object
*/
void IOHandler::writeOut(SIC_LineBuilder &line, Flags &flags) {

	writeOutSymbol(line);
	writeOutOpcode(flags, line);
	writeOutOperand(flags, line);

	// Handle a special case 
	if (line.opcode == "ldb")
		outFile /*cout*/<< setw(9) << left << "" << setw(9) << left << "BASE" << setw(12) << left << line.operand << endl;

};

int IOHandler::writeOutLtorg(LiteralTable &littab, int currAddr) {

	if (!littab.hasLiteralAt(currAddr))
		return 0;

	outFile /*cout*/<< setw(14) << right << "LTORG" << endl;

	int poolLength = 0;
	while (littab.getLiteral(currAddr) != "") {
		poolLength += littab.getLength(currAddr);
		currAddr += (littab.getLength(currAddr) / 2);
	}

	return poolLength;

};

// "Borrowed" from:
// https://stackoverflow.com/questions/10268872/c-fstream-function-that-reads-a-line-without-extracting
string IOHandler::peekNextLine() {
	string nextLine;

	// Get current position
	int len = inFile.tellg();
	getline(inFile, nextLine);

	// Get back to where you once belonged
	// Get back Jojo, go home
	inFile.seekg(len, ios_base::beg);

	return nextLine;
};

void IOHandler::getLine(string &holder) {
	getline(inFile, holder);
};

void IOHandler::writeOutSymbol(SIC_LineBuilder &line) {
	outFile /*cout*/<< setw(9) << left << line.symbol;
};

void IOHandler::writeOutOpcode(Flags &flags, SIC_LineBuilder &line) {
	if (flags.getIsExtended())
		outFile /*cout*/<< "+" << line.opcode << setw(6) << right;
	else
		outFile /*cout*/<< setw(9) << left << line.opcode;
};

void IOHandler::writeOutOperand(Flags & flags, SIC_LineBuilder &line) {
	if (flags.getIsImmediate() && !flags.getIsSimpleAddressing() && line.format >= 3)
		outFile /*cout*/<< "#" << line.operand << setw(4) << left;
	else if (flags.getIsIndirect() && !flags.getIsSimpleAddressing() && line.format >= 3)
		outFile /*cout*/<< "@" << line.operand << setw(4) << left;
	else if (flags.getIsIndexAddressing() && line.format >= 3)
		outFile /*cout*/<< line.operand << ",X" << setw(4) << left;
	else
		outFile /*cout*/<< "" << line.operand << setw(4) << left;
	outFile /*cout*/<< endl;
};