/*
 * =====================================================================================
 *
 *       Filename:  graph.cpp
 *
 *    Description:  Implementation of graph model and text-rank algorithm 
 *
 *        Version:  1.0
 *        Created:  26/12/2020 14:43:25
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  François Hissel
 *   Organization:  
 *
 * =====================================================================================
 */

#include <locale>
#include <codecvt>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <numeric>
#include <iterator>
#include "dictionary.h"
#include "graph.h"

using namespace std;

auto &global_facet=use_facet<ctype<wchar_t>>(locale());

/*********************************************
 *           Utility functions               *
 *********************************************/
/**
 * \brief Facet used to tokenize a string.
 *
 * The facet is derived from the standard character type facet but considers all punctuation signs as whitespaces.
 */
struct Tokenizer_facet:ctype<wchar_t> {

	/**
	 * \brief Classification of a character
	 */
	bool do_is(mask m,char_type c) const {
		if ((m & space) && (!iswalnum(c) || c==L'-')) return true;
		return ctype::do_is(m,c);
	}
};

/**
 * \brief Perform a lookup of a word in the dictionary
 *
 * This function returns a tuple with the original word, its POS-tag, its associated lemma, and a reference to the associated node in the graph by performing a lookup in the dictionary. It checks both the actual word and its lower-case version.
 * \param w Original word
 * \return Result tuple
 */
tuple<wstring,Pos,wstring,Node*> lookup_word(const wstring &w,const Dictionary &dic) {
	if (w.size()<3) return make_tuple(w,Pos::STOP,L"",nullptr);
	bool has_char=false;
	for (const auto &ch:w) if (global_facet.is(ctype_base::alpha,ch)) {
		has_char=true;
		break;
	}
	if (!has_char) return make_tuple(w,Pos::STOP,L"",nullptr);
	auto it=dic.lookup(w);
	if (it!=dic.cend()) return make_tuple(w,it->pos,(it->pos==Pos::STOP)?L"":it->lemma,nullptr);
	wstring lower;
	transform(w.begin(),w.end(),back_inserter(lower),[](wchar_t c){return global_facet.tolower(c);});
	it=dic.lookup(lower);
	if (it!=dic.cend()) return make_tuple(w,it->pos,(it->pos==Pos::STOP)?L"":it->lemma,nullptr);
	return make_tuple(w,Pos::UNKNOWN,w,nullptr);
}

/**
 * \brief Return a sentence from an input stream
 *
 * A sentence is delimited by either a newline character or a dot. The input stream is read until one of the following conditions is encountered:
 *   - the EOF bit or the bad bit of the fail bit is set
 *   - a dot is read ; the dot is not included in the resulting string
 *   - a newline character is read ; the newline character is not included in the resulting string
 * \param in Input stream
 * \return Sentence read from the input stream
 */
wstring get_sentence(wistream &in) {
	wostringstream oss;
	wchar_t ch;
	bool stop=false;
	while (in && !stop) {
		ch='\0';
		in.get(ch);
		if (in.fail() || in.bad() || ch==L'\n' || ch==L'.') {
			stop=true;
		} else oss.put(ch);
	}
	return oss.str();
}

/*********************************************
 *                  Node                     *
 *********************************************/
void Node::print(wostream &out) const {
	out << "(score=" << score << ",_lscore=" << _lscore << ",_total=" << _total;
	for (const auto &it:edges) out << ",(" << it.first << "," << it.second << ")";
	out << endl;
}

/*********************************************
 *                 Graph                     *
 *********************************************/
void Graph::print(wostream &out) const {
	for (auto &[key,node]:*this) {
		wcout << key << "\t";
		for (auto &[k,n]:node.edges) {
			wcout << '(' << k << ',' << n << ") ";
		}
		wcout << endl;
	}
}

void Graph::add_edge(const wstring &a,const wstring &b) noexcept {
	auto &ela=(*this)[a];
	auto &elb=(*this)[b];
	ela.edges[b]=ela.edges[b]+1;
	elb.edges[a]=elb.edges[a]+1;
}

void Graph::add_edge(const wstring &a,Node &na,const wstring &b,Node &nb) noexcept {
	na.edges[b]=na.edges[b]+1;
	nb.edges[a]=nb.edges[a]+1;
}

Graph::Graph(wistream &ifs,const Dictionary &dic,int window_size) {
	ifs.imbue(locale(ifs.getloc(),new codecvt_utf8<wchar_t>));
	while (ifs) {
		wstring sentence=get_sentence(ifs);
		wistringstream iss(sentence);
		iss.imbue(locale(iss.getloc(),new codecvt_utf8<wchar_t>));
		iss.imbue(locale(iss.getloc(),new Tokenizer_facet));
		vector<wstring> tokens;
		copy(istream_iterator<wstring,wchar_t>(iss),istream_iterator<wstring,wchar_t>(),back_inserter(tokens));
		_tokens.emplace_back();
		auto &t=_tokens.back();
		t.reserve(tokens.size());
		transform(tokens.begin(),tokens.end(),back_inserter(t),[&dic](const wstring &w){return lookup_word(w,dic);});
		for (auto it=t.begin();it!=t.end();it++) if (get<1>(*it)!=Pos::STOP) {
			auto &ela=(*this)[get<2>(*it)];
			get<3>(*it)=&ela;
			auto jt=it;
			for (int i=0;i<window_size;++i) {
				jt++;
				while (jt!=t.end() && get<1>(*jt)==Pos::STOP) jt++;
				if (jt!=t.end()) {
					auto &elb=(*this)[get<2>(*jt)];
					add_edge(get<2>(*it),ela,get<2>(*jt),elb);
				} else break;
			}
		}
	}
}

vector<pair<wstring,double>> Graph::text_rank(int num_keywords,int num_iterations,double d) noexcept {
	// Calculate and store the total weight of edges per node, and initialize scores of nodes to 1
	for (auto &it:*this) {
		it.second.score=1.0;
		it.second._total=accumulate(it.second.edges.begin(),it.second.edges.end(),0,[](const auto &a,const auto &b){return a+b.second;});
	}
	// Main loop of the text-rank algorithm. Iterate to evaluate the scores of each node
	for (int i=0;i<num_iterations;++i) {
		for (auto &it:*this) it.second._lscore=it.second.score;
		for (auto &it:*this) {
			it.second.score=(1-d)+d*accumulate(it.second.edges.begin(),it.second.edges.end(),0.0,[=](const auto &a,const auto &b) {const auto n=find(b.first);if (n!=cend()) return a+b.second*n->second._lscore/n->second._total; else return a;});
		}
	}
	// Store the words and their associated node in the graph in a vector
	std::vector<std::pair<std::wstring,Node*>> keywords;
	transform(begin(),end(),back_inserter(keywords),[](auto &a){return make_pair(a.first,&(a.second));});
	// Partially sort the vector for the first num_keywords terms
	auto middle=keywords.begin();
	if (num_keywords>int(keywords.size())) num_keywords=keywords.size();
	advance(middle,num_keywords);
	partial_sort(keywords.begin(),middle,keywords.end(),[](const auto &a,const auto &b){return a.second->score>b.second->score;});
	// Generate a vector with the keywords and their scores
	for (auto it=keywords.begin();it!=middle;it++) it->second->keyword=true;
	vector<pair<wstring,double>> res;
	transform(keywords.begin(),middle,back_inserter(res),[](const auto &a){return make_pair(a.first,a.second->score);});
	// Go once again through the text to find if two or more consecutive words are keywords. In that case, add a multiple-word keyword to the result vector by merging the keywords and summing up ther scores
	for (auto &it:_tokens) {
		auto jt=it.begin();
		while (jt!=it.end()) {
			if (get<3>(*jt)!=nullptr && get<3>(*jt)->keyword) {
				auto kt=jt;
				do {
					kt++;
				} while (kt!=it.end() && get<3>(*kt)!=nullptr && get<3>(*kt)->keyword);
				if (distance(jt,kt)>1) {
					wostringstream oss;
					transform(jt,kt,ostream_iterator<wstring,wchar_t>(oss,L" "),[](const auto &a){return get<2>(a);});
					auto kw=oss.str();
					kw.erase(kw.end()-1);
					if (find_if(res.cbegin(),res.cend(),[&kw](const auto &a) {return get<0>(a)==kw;})==res.cend()) res.push_back(make_pair(kw,accumulate(jt,kt,0.0,[](const auto &a,const auto &b) {return a+get<3>(b)->score;})));
				}
				jt=kt;
			} else jt++;
		}
	}
	// Sort again with the new multi-word keywords
	sort(res.begin(),res.end(),[](const auto &a,const auto &b){return a.second>b.second;});
	res.erase(res.begin()+num_keywords,res.end());
	// Return the result
	return res;
}
