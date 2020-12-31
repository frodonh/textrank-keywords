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
		int32_t ilemma; 	//!< Index of the lemma in the dictionary, or -1 if it does not exist
		Entry(std::wstring pword,Pos ppos,std::wstring plemma,int32_t pilemma):word(pword),pos(ppos),lemma(plemma),ilemma(pilemma) {} 	//!< Construct an entry with its member
		Entry()=default; 	//!< Construct an empty entry
		Entry(const Entry &s)=default; 	//!< Default copy constructor
		Entry(Entry &&s)=default; 	//!< Default move constructor

		/**
		 * \brief Print the entry on an output stream
		 *
		 * This function may be used for debugging purposes.
		 * \param out Output stream
		 */
		void print(std::wostream &out) const;
};

inline bool operator<(const std::wstring &a,const Entry &b) {return a<b.word;}
inline bool operator<(const Entry &a,const std::wstring &b) {return a.word<b;}
inline bool operator<(const Entry &a,const Entry &b) {return a.word<b.word;}
inline std::wostream& operator<<(std::wostream &out,const Entry &a) {a.print(out);return out;}

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
		 * \param path Path name of the file from which the dictionary has to be loaded. For best performance, a binary format is used. The binary dictionary may be generated from a text file using the 'prepare' executable. For each dictionary entry, the binary file has the following bytes:
		 *   - one byte for the POS tag, either 'N' for noun or 'A' for adjective, or 'S' for non-significant word (verb, adverb, preposition...)
		 *   - four bytes which make an int32_t value corresponding to the number of the associated lemma entry (starting with 0)
		 *   - one byte which stands for the length (read as unsigned char) of the word in UTF-8 encoding 
		 *   - n bytes for the word, n being the number read in the previous step
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
