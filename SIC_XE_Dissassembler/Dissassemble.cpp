#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "Dissassemble.h"


using namespace std;

Dissassemble::Dissassemble(string inputPath, string outputPath) {
	this->inputPath = inputPath;
	this->outputPath = outputPath;

	isIndirect = false;
	isImmedaite = false;
	isSimpleAddressing = false;
	isIndexAddressing = false;
	isBaseRelative = false;
	isPcRelative = false;
	isExtended = false;

	progctr = 0;

};

void Dissassemble::disassemble() {

	readHeadRecord();
	readTextRecord();
	readEndRecord();

};

void Dissassemble::readHeadRecord() {
};

void Dissassemble::readTextRecord() {
};

void Dissassemble::readEndRecord() {
};

int Dissassemble::calculateTargetAddress(string) {
	return 0;
};

void Dissassemble::setProgctr(string address) {
};

void Dissassemble::setProgctr(int address) {
};


