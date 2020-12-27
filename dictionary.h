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
using Entry=std::pair<Pos,std::wstring>;

/**
 * \brief Full dictionary
 *
 * The dictionary has words as keys. Each word is associated to one or several entries.
 */
class Dictionary:public std::unordered_map<std::wstring,std::vector<Entry>> {
	public:
		/**
		 * \brief Construct a dictionary by reading it from a file
		 *
		 * \param path Path name of the file from which the dictionary has to be loaded
		 */
		explicit Dictionary(const std::string &path);

		/**
		 * \brief Print the whole dictionary on an output stream
		 *
		 * This function may be used for debugging purposes.
		 * \param out Output stream
		 */
		void print(std::wostream &out) const;
};

#endif   /* ----- #ifndef DICTIONARY_INC  ----- */
