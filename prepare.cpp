/*
 * =====================================================================================
 *
 *       Filename:  prepare.cpp
 *
 *    Description:  Prepare the dictionary for the Text-Rank algorithm
 *
 *        Version:  1.0
 *        Created:  28/12/2020 15:41:17
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  François Hissel
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <utility>

using namespace std;

/**
 * \brief Next token in a tab-separated value file
 *
 * This function returns the substring of string s starting at position pos and up to the next tabulation character or the end of the string. The value pos is updated to the position after the tabulation character.
 * \param s Source string
 * \param pos Starting position
 * \return Next token, extracted from string s up to the next tabulation character or the end of the string
 */
string next_token(const string &s,string::size_type &pos) {
	auto i=pos;
	pos=s.find_first_of('\t',i);
	if (pos==string::npos) pos=s.length();
	return s.substr(i,(pos++)-i);
}

/**
 * \brief Tell if an entry of the dictionary is not needed.
 *
 * This function tests for unneeded entries in the resulting dictionary. Currently, nouns and adjectives without associated lemma are unneeded because a word with no POS tag is considered the same.
 */
inline bool unused(const tuple<string,string,string,int32_t> &ent) {
	return (get<1>(ent)=="N" || get<1>(ent)=="A") && get<3>(ent)==-1;
}

int main(int argc,char *argv[]) {
	ifstream ifs(argv[1]);
	unordered_map<string,array<string,3>> dic;
	string word,pos,lemma,freq;
	double dfreq;
	string line;
	while (ifs) {
		getline(ifs,line);
		if (ifs.fail() || ifs.bad()) continue;
		string::size_type i=0;
		word=next_token(line,i);
		pos=next_token(line,i);
		lemma=next_token(line,i);
		freq=next_token(line,i);
		if (freq!="") dfreq=stod(freq); else dfreq=0.0;
		if (pos=="NOM") pos="N";
		else if (pos=="ADJ") pos="A";
		else if (pos=="") pos="";
		else pos="S";
		auto it=dic.find(word);
		if (it==dic.end()) {  
			dic.insert(make_pair(word,array<string,3>{pos,lemma,freq}));
		}
		else if ((it->second[2]=="" && freq!="") || (it->second[2]!="" && stod(it->second[2])<dfreq) || (it->second[0]=="" && lemma!="")) {
			it->second[0]=pos;
			it->second[1]=lemma;
			it->second[2]=freq;
		}
	}
	vector<tuple<string,string,string,int32_t>> words;
	transform(dic.begin(),dic.end(),back_inserter(words),[](const auto &a){return make_tuple(a.first,move(a.second[0]),move(a.second[1]),0);});
	words.erase(remove_if(words.begin(),words.end(),unused));
	sort(words.begin(),words.end(),[](const auto &a,const auto &b){return get<0>(a)<get<0>(b);});
	for (auto &it:words) {
		if (get<1>(it)!="N" && get<1>(it)!="A" && get<2>(it)!="") {
			auto val=make_tuple(get<2>(it),"","",0);
			auto f=lower_bound(words.begin(),words.end(),val);
			if (f!=words.end()) get<3>(it)=distance(words.begin(),f);
		} else get<3>(it)=-1;
	}
	ofstream ofs(argv[2],ofstream::binary);
	for (auto &it:words) {
		char a;
		if (get<1>(it)!="") a=get<1>(it)[0]; else a=' ';
		ofs.write(&a,1);
		ofs.write((char*)(&(get<3>(it))),sizeof(int32_t));
		wstring::size_type l=get<0>(it).length();
		unsigned char b=(unsigned char)(l);
		ofs.write((char*)(&b),sizeof(unsigned char));
		ofs.write(get<0>(it).data(),sizeof(char)*l);
	}
//	for (auto &it:words) {
//		char a;
//		if (get<1>(it)=="N") a='N';
//		else if (get<1>(it)=="A") a='A';
//		else if (get<1>(it)=="") a=' ';
//		else if (get<1>(it)=="S") a='S';
//		cout << a << "\t" << get<3>(it) << "\t";
//		wstring::size_type l=get<0>(it).length();
//		cout << l << "\t" << get<0>(it) << endl;
//	}
}
