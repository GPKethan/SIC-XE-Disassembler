#include "stdafx.h"
#include "Symbol.h"

using namespace std;

// This constructor should never be called
Symbol::Symbol() {
	//throw CtorException();
};

Symbol::Symbol(string symbol, int value, bool isAbsolute) {
	this->symbol = symbol;
	this->value = value;
	this->isAbsolute = isAbsolute;
};

// Getters
string Symbol::getSymbol() {
	return this->symbol;
};

int Symbol::getValue() {
	return this->value;
};

bool Symbol::getIsAbsolute() {
	return this->isAbsolute;
};