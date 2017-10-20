#ifndef CONVERT_H_
#define CONVERT_H_

#pragma once
#include <algorithm>
#include <bitset>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#define MAX_BITSET 4 // was 8

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

	static string hexToBinary(string toConvert) {

		int decimal = hexToDecimal(toConvert);
		bitset<MAX_BITSET> bs(decimal);

		return bs.to_string();

	};

	static string binToHex(string toConvert) {

		bitset<MAX_BITSET> bs(toConvert);
		unsigned long tempLong = bs.to_ulong();

		stringstream ss;
		ss << hex << tempLong;
		string toReturn = ss.str();
		transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::toupper);

		return toReturn;

	};

	static int stringToInt(string toConvert) {

		stringstream ss(toConvert);
		int x;
		ss >> x;
		
		return x;

	};


};

#endif //CONVERT_H_