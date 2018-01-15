#include "stdafx.h"
#include "Literal.h"

using namespace std;

// Ctor
Literal::Literal() {};

Literal::Literal(string name, string literal, int length, int addr) {
	this->name = name;
	this->literal = literal;
	this->length = length;
	this->addr = addr;
};

// Getters
string Literal::getName() {
	return this->name;
};

string Literal::getLiteral() {
	return this->literal;
};

int Literal::getLength() {
	return this->length;
};

int Literal::getAddress() {
	return this->addr;
};