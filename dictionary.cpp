/*
 * =====================================================================================
 *
 *       Filename:  dictionary.cpp
 *
 *    Description:  Implementation of the dictionary object 
 *
 *        Version:  1.0
 *        Created:  25/12/2020 22:15:41
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  François Hissel
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <utility>
#include <filesystem>
#include <limits>
#include "dictionary.h"

using namespace std;

wstring next_token(const wstring &s,wstring::size_type &pos) {
	auto i=pos;
	pos=s.find_first_of(L'\t',i);
	if (pos==wstring::npos) pos=s.length();
	return s.substr(i,(pos++)-i);
}

void Dictionary::print(wostream &out) const {
	for (const auto &it:*this) {
		out << it.word << L"\t(" ;
		switch (it.pos) {
			case Pos::ADJ: out << "ADJ"; break;
			case Pos::ADV: out << "ADV"; break;
			case Pos::NOUN: out << "NOUN"; break;
			case Pos::VER: out << "VER"; break;
			case Pos::UNKNOWN: out << "UNKNOWN"; break;
			case Pos::STOP: out << "STOP"; break;
		}
		out << "," << it.lemma << ") " << endl;
	}
}

Dictionary::Dictionary(const string &path) {
	locale loc("");
	wifstream ifs(path);
	reserve(std::filesystem::file_size(path)/15);
	ifs.imbue(locale(ifs.getloc(),new codecvt_utf8<wchar_t>));
	wstring word,pos,lemma;
	wstring line;
	while (ifs) {
		getline(ifs,line);
		if (ifs.fail() || ifs.bad()) continue;
		string::size_type i=0;
		word=next_token(line,i);
		pos=next_token(line,i);
		lemma=next_token(line,i);
		Pos p;
		if (pos==L"N") p=Pos::NOUN;
		else if (pos==L"A") p=Pos::ADJ;
		else if (pos==L"") p=Pos::UNKNOWN;
		else p=Pos::STOP;
		push_back(Entry(word,p,lemma));
	}
}
