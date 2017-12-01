// SIC_XE_Dissassembler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string>

#include "Dissassemble.h"

using namespace std;


int main() {

	/*
	string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\practice sym file (all).txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\practice obj file (all).txt";
	*/

	/*
	string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\symbol file.txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\object file.txt";
	*/

	string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\sym file (more testing).txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\obj file (more testing).txt";

	string outputFilePath = "";

	/*SymbolTable symtab(symbolFilePath);
	LiteralTable littab(symbolFilePath);

	auto ans = symtab.getSymbol(0);

	cout << ans << endl;*/

	/*int progctr = 0;
	string currLine = "T0000001E";
	string objectCode = "050000";

	currLine.append(objectCode);*/

	Dissassemble test(inputFilePath, outputFilePath, symbolFilePath);
	test.disassemble();

	//cout << Convert::negativeHexToPositiveDecimal("FEC") << endl;

    return 0;
}

