#pragma once
#include <string>
#include <exception>

#include "CtorException.h"

using namespace std;

class Literal {
public:
	// Ctor
	Literal() {
		throw defaultCtorExcept;
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