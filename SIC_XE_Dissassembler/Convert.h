#ifndef CONVERT_H_
#define CONVERT_H_

#pragma once
#include <algorithm>
#include <bitset>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#define MAX_BITSET 4
#define DEBUG 1

using namespace std;

class Convert {
public:
	static int hexToDecimal(string toConvert) {

		unsigned int toReturn;
		stringstream ss;
		ss << hex << toConvert;
		ss >> toReturn;

		return toReturn;

	};

	static int hexToDecimal(char toConvert) {
		return hexToDecimal(string(toConvert, 1));
	};

	static string hexToBinary(string toConvert) {

		int decimal = hexToDecimal(toConvert);
		bitset<MAX_BITSET> bs(decimal);

		return bs.to_string();

	};

	static string hexToBinary(char toConvert) {
		return hexToBinary(string(1, toConvert));
	};

	static string binaryToHex(string toConvert) {

		bitset<MAX_BITSET> bs(toConvert);
		unsigned long tempLong = bs.to_ulong();

		stringstream ss;
		ss << hex << tempLong;
		string toReturn = ss.str();
		transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::toupper);

		return toReturn;

	};

	static int binaryToDecimal(string toConvert) {
		return hexToDecimal(binaryToHex(toConvert));
	};

	static int negativeHexToPositiveDecimal(string toConvert) {
		
		bitset<12> bs(hexToDecimal(toConvert));
		string bin = bs.to_string();

		// Flip the bits
		for (int i = 0; i < bin.size(); i++)
			bin[i] = (bin[i] == '1') ? '0' : '1';

		// Add 1
		int val = negToPosHelper(bin);
		val++;

		return val;
		
	};

	static string decimalToHex(int toConvert) {
		// For some reason calling binToHex won't work even though 
		//  this code is LITERALLY THE SAME
		bitset<64> bs(toConvert);
		unsigned long tempLong = bs.to_ulong();

		stringstream ss;
		ss << hex << tempLong;
		string toReturn = ss.str();
		transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::toupper);

		return toReturn;
	};

	static string decimalToHex(string toConvert) {
		return decimalToHex(stoi(toConvert));
	};

private:
	static int negToPosHelper(string toConvert) {

		bitset<12> bs(toConvert);
		unsigned long tempLong = bs.to_ulong();

		stringstream ss;
		ss << hex << tempLong;
		string toReturn = ss.str();
		transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::toupper);

		return hexToDecimal(toReturn);
	};

};

#endif //CONVERT_H_