/*
 * =====================================================================================
 *
 *       Filename:  keywords.cpp
 *
 *    Description:  Automatic extraction of keywords from a markdown file. The input
 *    				markdown file is read from standard input. Keywords are written on
 *    				standard output.
 *
 *        Version:  1.0
 *        Created:  18/12/2020 10:23:08
 *       Revision:  none
 *       Compiler:  clang++
 *
 *         Author:  François Hissel
 *   Organization:  
 *
 * =====================================================================================
 */

#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <memory>
#include <algorithm>
#include "dictionary.h"
#include "graph.h"

using namespace std;

/**
 * \brief Main program
 *
 * \param argc Number of command-line arguments, including the name of the program
 * \param argv Array or command-line arguments, the first element is the name of the program
 * \return 0 if the execution was successful, an error code otherwise
 */
int main(int argc,char *argv[]) {
	if (argc<2) {
		cerr << "Syntax: keywords [path_do_dictionary] [path_to_file]\n";
		return 1;
	}
	locale loc("");
	locale::global(loc);
	wcout.imbue(locale(loc,new codecvt_utf8<wchar_t>));
	wifstream ifs;
	Dictionary dict(argv[1]);
	if (argc>2) {
		ifs.open(argv[2]);
		wcin.rdbuf(ifs.rdbuf());
	}
	Graph graph(wcin,dict,3);
	auto res=graph.text_rank(10,20,0.85);
	for (const auto &[word,score]:res) {
		wcout << word << '\t' << score << endl;
	}
}
