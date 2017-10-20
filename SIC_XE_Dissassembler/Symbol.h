#ifndef SYMBOL_H_
#define SYMBOL_H_

#pragma once
#include <string>
#include <exception>

#include "CtorException.h"

using namespace std;

class Symbol {
public:
	// This constructor should never be called
	Symbol() {
		throw defaultCtorExcept;
	};

	Symbol(string symbol, int value, bool isAbsolute) {
		this->symbol = symbol;
		this->value = value;
		this->isAbsolute = isAbsolute;
	};

	// Getters
	string getSymbol() {
		return this->symbol;
	};

	int getValue() {
		return this->value;
	};

	bool getIsAbsolute() {
		return this->isAbsolute;
	};

private:
	// Variables
	string symbol;				// Name of the symbol
	int value;					// Value/Address of the symbol
	bool isAbsolute;			// Is the value absolute? If not then its relative

};
#endif //SYMBOL_H_