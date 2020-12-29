/*
 * =====================================================================================
 *
 *       Filename:  dictionary.h
 *
 *    Description:  Definition of the dictionary object and associated methods 
 *
 *        Version:  1.0
 *        Created:  25/12/2020 21:49:11
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  François Hissel
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  DICTIONARY_INC
#define  DICTIONARY_INC

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <string>

/**
 * \brief Possible nature of a word (POS)
 */
enum class Pos {
	UNKNOWN, 	//!< Unknown POS
	ADJ, 	//!< Adjective
	ADV, 	//!< Adverb
	STOP, 	//!< Stop word (determiner, interjection, pronoun, preposition...)
	NOUN, 	//!< Noun
	VER 	//!< Verb
};

/**
 * \brief An entry in the dictionary
 *
 * An entry is made of the nature of a word (POS-tag) and its associated lemma.
 */
class Entry {
	public:
		std::wstring word; 	//!< Original word
		Pos pos; 	//!< Part-of-speech tag
		std::wstring lemma; 	//!< Lemma
		Entry(std::wstring pword,Pos ppos,std::wstring plemma):word(pword),pos(ppos),lemma(plemma) {} 	//!< Construct an entry with its member
		Entry()=default; 	//!< Construct an empty entry
		Entry(const Entry &s)=default; 	//!< Default copy constructor
		Entry(Entry &&s)=default; 	//!< Default move constructor
};

inline bool operator<(const std::wstring &a,const Entry &b) {return a<b.word;}
inline bool operator<(const Entry &a,const std::wstring &b) {return a.word<b;}
inline bool operator<(const Entry &a,const Entry &b) {return a.word<b.word;}

/**
 * \brief Full dictionary
 *
 * The dictionary has words as keys. Each word is associated to one or several entries.
 */
class Dictionary:public std::vector<Entry> {
	public:
		/**
		 * \brief Construct a dictionary by reading it from a file
		 *
		 * \param path Path name of the file from which the dictionary has to be loaded
		 */
		explicit Dictionary(const std::string &path);

		/**
		 * \brief Look for a word in the dictionary
		 *
		 * This function returns the entry associated with the word.
		 * \param word Word to look for in the dictionary
		 * \return Associated entry, or empty entry object if no entry is found
		 */
		std::vector<Entry>::const_iterator lookup(const std::wstring &word) const noexcept {auto it=std::lower_bound(cbegin(),cend(),word);if (it!=cend() && it->word==word) return it;return cend();}

		/**
		 * \brief Print the whole dictionary on an output stream
		 *
		 * This function may be used for debugging purposes.
		 * \param out Output stream
		 */
		void print(std::wostream &out) const;
};

#endif   /* ----- #ifndef DICTIONARY_INC  ----- */
