#pragma once
#include <iostream>
#include <unordered_map>

#include "CtorException.h"

using namespace std;

class Optab {
public:
	Optab() {
		throw defaultCtorExcept;
	};

	string getOpcode(string key) {
		// In case it doesnt exist
		auto it = table.find(key);
		if (it == table.end())
			return "";

		pair<string, int> temp = table.find(key)->second;
		return temp.first;

	};

	int getFormat(string key) {

		auto it = table.find(key);
		if (it == table.end())
			return -1;

		pair<string, int> temp = table.find(key)->second;
		return temp.second;

	};

private:
	const unordered_map<string, pair<string, int>> table {
		{ "18", pair<string, int>("add", 4) },
		{ "58", pair<string, int>("addf", 4) },
		{ "90", pair<string, int>("addr", 2) },
		{ "40", pair<string, int>("and", 4) },
		{ "B4", pair<string, int>("clear", 2) },
		{ "28", pair<string, int>("comp", 4) },
		{ "88", pair<string, int>("compf", 4) },
		{ "A0", pair<string, int>("compr", 2) },
		{ "24", pair<string, int>("div", 4) },
		{ "64", pair<string, int>("divf", 4) },
		{ "9C", pair<string, int>("divr", 2) },
		{ "C4", pair<string, int>("fix", 1) },
		{ "C0", pair<string, int>("float", 1) },
		{ "F4", pair<string, int>("hio", 1) },
		{ "3C", pair<string, int>("j", 4) },
		{ "30", pair<string, int>("jeq", 4) },
		{ "34", pair<string, int>("jgt", 4) },
		{ "38", pair<string, int>("jlt", 4) },
		{ "48", pair<string, int>("jsub", 4) },
		{ "00", pair<string, int>("lda", 4) },
		{ "68", pair<string, int>("ldb", 4) },
		{ "50", pair<string, int>("ldch", 4) },
		{ "70", pair<string, int>("ldf", 4) },
		{ "08", pair<string, int>("ldl", 4) },
		{ "6C", pair<string, int>("lds", 4) },
		{ "74", pair<string, int>("ldt", 4) },
		{ "04", pair<string, int>("ldx", 4) },
		{ "D0", pair<string, int>("lps", 4) },
		{ "20", pair<string, int>("mul", 4) },
		{ "60", pair<string, int>("mulf", 4) },
		{ "98", pair<string, int>("mulr", 2) },
		{ "C8", pair<string, int>("norm", 1) },
		{ "44", pair<string, int>("or", 4) },
		{ "D8", pair<string, int>("rd", 4) },
		{ "AC", pair<string, int>("rmo", 2) },
		{ "4C", pair<string, int>("rsub", 4) },
		{ "A4", pair<string, int>("shiftl", 2) },
		{ "A8", pair<string, int>("shiftr", 2) },
		{ "F0", pair<string, int>("sio", 1) },
		{ "EC", pair<string, int>("ssk", 4) },
		{ "0C", pair<string, int>("sta", 4) },
		{ "78", pair<string, int>("stb", 4) },
		{ "54", pair<string, int>("stch", 4) },
		{ "80", pair<string, int>("stf", 4) },
		{ "D4", pair<string, int>("sti", 4) },
		{ "14", pair<string, int>("stl", 4) },
		{ "7C", pair<string, int>("sts", 4) },
		{ "E8", pair<string, int>("stsw", 4) },
		{ "84", pair<string, int>("stt", 4) },
		{ "10", pair<string, int>("stx", 4) },
		{ "1C", pair<string, int>("sub", 4) },
		{ "5C", pair<string, int>("subf", 4) },
		{ "94", pair<string, int>("subr", 2) },
		{ "B0", pair<string, int>("svc", 2) },
		{ "E0", pair<string, int>("td", 4) },
		{ "F8", pair<string, int>("tio", 1) },
		{ "2C", pair<string, int>("tix", 4) },
		{ "B8", pair<string, int>("tixr", 2) },
		{ "DC", pair<string, int>("wd", 4) }
	};
};