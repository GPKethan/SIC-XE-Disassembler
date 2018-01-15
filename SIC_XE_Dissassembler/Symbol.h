#ifndef SYMBOL_H_
#define SYMBOL_H_

#pragma once
#include <string>

using namespace std;

class Symbol {
public:
	// This constructor should never be called
	Symbol();

	Symbol(string symbol, int value, bool isAbsolute);

	// Getters
	string getSymbol();

	int getValue();

	bool getIsAbsolute();

protected:
	// Variables
	string symbol;				// Name of the symbol
	int value;					// Value/Address of the symbol
	bool isAbsolute;			// Is the value absolute? If not then its relative

};
#endif //SYMBOL_H_