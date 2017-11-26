#ifndef LITERAL_H_
#define LITERAL_H_

#pragma once
#include <string>

//#include "CtorException.h"

using namespace std;

class Literal {
public:
	// Ctor
	Literal() {
		//throw CtorException();
	};

	Literal(string name, string literal, int length, int addr) {
		this->name = name;
		this->literal = literal;
		this->length = length;
		this->addr = addr;
	};

	// Getters
	string getName() {
		return this->name;
	};

	string getLiteral() {
		return this->literal;
	};

	int getLength() {
		return this->length;
	};

	int getAddress() {
		return this->addr;
	};


private:
	string name;
	string literal;
	int length;
	int addr;

};

#endif //LITERAL_H_