/*
 * =====================================================================================
 *
 *       Filename:  graph.h
 *
 *    Description:  Graph model and text rank algorithm 
 *
 *        Version:  1.0
 *        Created:  26/12/2020 13:59:53
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  François Hissel
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  GRAPH_INC
#define  GRAPH_INC

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <tuple>
#include "dictionary.h"

/**
 * \brief Node of the network
 *
 * A node holds a word (or lemma), and a list of edges which link the node to other nodes in the graph.
 */
class Node {
	public:
		friend class Graph;
		double score; 	//!< Score of the node
		bool keyword; 	//!< Variable reserved to tell if the word is a keyword
		std::unordered_map<std::wstring,int> edges; 	//!< Edges to other nodes, together with their weight

		Node():score(1.0),keyword(false) {}

		/**
		 * \brief Print the whole node on an output stream
		 *
		 * This method is mainly for debug purposes. It prints the node in a readable way on an output stream.
		 * \param out Output stream
		 */
		void print(std::wostream &out) const;
	private:
		double _lscore; 	//!< Previous score of the node
		int _total; 	//!< Sum of the weights of edges from this node
};

/**
 * \brief Graph representation of the text
 *
 * The graph is a list of Node.
 */
class Graph:public std::unordered_map<std::wstring,Node> {
	public:

		/**
		 * \brief Build the graph from a text
		 *
		 * This constructor reads a text from the given input stream and builds the graph from it.
		 * \param ifs Input stream
		 * \param dic Dictionary used to POS tag and lemmify words
		 * \param window_size Size of the window for linked edges
		 */
		Graph(std::wistream &ifs,const Dictionary &dic,int window_size);

		/**
		 * \brief TextRank algorithm
		 *
		 * The method updates the score of each node based on the number of inbound edges.
		 * \param num_keywords Number of keywords expected
		 * \param num_iterations Number of iterations
		 * \param d Damping coefficient
		 * \return Sorted list of keywords, together with their score
		 */
		std::vector<std::pair<std::wstring,double>> text_rank(int num_keywords,int num_iterations,double d) noexcept;

		/**
		 * \brief Print the whole graph on an output stream
		 *
		 * This method is mainly for debug purposes. It prints the full graph in a readable way on an output stream.
		 * \param out Output stream
		 */
		void print(std::wostream &out) const;

	private:
		std::vector<std::vector<std::tuple<std::wstring,Pos,std::wstring,Node*>>> _tokens; 	//!< Array of tokens. Each internal array represents a sentence. Each element of the array is made of the orginal word, its POS tag, its associated lemma and a pointer to the node in the graph

		/**
		 * \brief Add a non-directional edge between two nodes of the graph
		 *
		 * This function adds an edge between two nodes of the graph. If the nodes do not exist, they are created. If the edge already exist, its weight is increased by 1.
		 * \param a Key of the first node, starting point of the edge
		 * \param b Key of the second node, to which the new edge is pointed
		 */
		void add_edge(const std::wstring &a,const std::wstring &b) noexcept;

		/**
		 * \brief Add a non-directional edge between two nodes of the graph
		 *
		 * This function adds an edge between two nodes of the graph. If the edge already exist, its weight is increased by 1.
		 * \param a Key of the first node, starting point of the edge
		 * \param na First node, starting point of the edge
		 * \param b Key of the second node, to which the new edge is pointed
		 * \param nb Second node, to which the new edge is pointed
		 */
		void add_edge(const std::wstring &a,Node &na,const std::wstring &b,Node &nb) noexcept;
};

#endif   /* ----- #ifndef GRAPH_INC  ----- */
