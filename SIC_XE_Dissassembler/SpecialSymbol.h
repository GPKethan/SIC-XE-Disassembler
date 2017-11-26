#ifndef SPECIAL_SYMBOL_H_
#define SPECIAL_SYMBOL_H_

#pragma once
#include <string>
#include "Symbol.h"

using namespace std;

class SpecialSymbol : public Symbol {
public:
	// This constructor should never be called
	SpecialSymbol() {
		//throw CtorException();
	};

	SpecialSymbol(string symbol, int value, bool isAbsolute, int indexInArray) 
	: Symbol(symbol, value, isAbsolute) {
		this->indexInArray = indexInArray;
	};

	int getIndexInArray() {
		return indexInArray;
	}

private:
	// Variables
	int indexInArray;

};
#endif //SPECIAL_SYMBOL_H_