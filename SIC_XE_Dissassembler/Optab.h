#ifndef OPTAB_H_
#define OPTAB_H_

#pragma once
#include <iostream>
#include <unordered_map>

#include "Convert.h"

using namespace std;

static class Optab {
public:
	Optab() { };

	static string getOpcode(string key);

	static string getMnemonic(char first, char second);

	// Key is the Mnemonic, not the opcode (i.e: "04", NOT ldx)
	static int getFormat(string key);

private:
	/* 
	How to initialize a static const:
	https://stackoverflow.com/questions/11367141/how-to-declare-and-initialize-a-static-const-array-as-a-class-member
	*/
	const static unordered_map<string, pair<string, int>> table;

};
#endif //OPTAB_H_