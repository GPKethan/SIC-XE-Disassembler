// SIC_XE_Dissassembler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string>

#include "Dissassemble.h"

using namespace std;


int main() {

	

	
	
	/*string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\practice sym file (all).txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\practice obj file (all).txt";*/
	
	
	
	/*string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\fack SYM.txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\fack OBJ.txt";*/
	

	
	string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\symbol file.txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\object file.txt";
	

	
	/*string symbolFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\sym file (more testing).txt";
	string inputFilePath = "C:\\Users\\ethan\\Documents\\School\\11. Spring 2017\\CS 530 - System Software\\Labs\\assignment 2 stuff\\obj file (more testing).txt";*/
	

	string outputFilePath = "C:\\Users\\ethan\\Desktop\\out.txt";

	
	/*int avg = 0;
	int numTimes = 1000;
	for (int i = 0; i < numTimes; i++) {
		clock_t Start = clock();

		Dissassemble test(inputFilePath, outputFilePath, symbolFilePath);
		test.disassemble();

		avg += (clock() - Start);

		
	}
	
	cout << "took: " << (double) avg / numTimes << " ms" << endl << endl;
	*/
	

	Dissassemble test(inputFilePath, outputFilePath, symbolFilePath);
	test.readRecords();


    return 0;
}

