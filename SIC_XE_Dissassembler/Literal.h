#ifndef LITERAL_H_
#define LITERAL_H_

#pragma once
#include <string>

//#include "CtorException.h"

using namespace std;

class Literal {
public:
	// Ctor
	Literal();

	Literal(string name, string literal, int length, int addr);

	// Getters
	string getName();

	string getLiteral();

	int getLength();

	int getAddress();


private:
	string name;
	string literal;
	int length;
	int addr;

};

#endif //LITERAL_H_