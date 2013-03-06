//=======================================================================
// Author: Donovan Parks
//
// Copyright 2013 Donovan Parks
//
// This file is part of ClusterTree.
//
// ClusterTree is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ClusterTree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ClusterTree. If not, see <http://www.gnu.org/licenses/>.
//=======================================================================

#ifndef _GENGIS_NEWICK_IO_
#define _GENGIS_NEWICK_IO_

#include "Tree.hpp"
#include "Node.hpp"
#include "Exceptions.hpp"
#include "DataTypes.hpp"

#include <string>
#include <iostream>
#include <fstream>

/**
 * @brief Read/write data in Newick format.
 *
 * Branch lengths and bootstrap value are supported:
 *
 * ex:
 * <code>
 * ((Human:0.1,Gorilla:0.1):0.4,(Mouse:0.2,Rat:0.2):0.3);
 * </code>
 *
 * Code example:
 * @code
 * #include <NewickIO.hpp>
 * #include <Tree.hpp>
 * 
 * NewickIO newickIO;
 * Tree<Node>* tree(new Tree<Node>());
 * newickIO.Read(tree, "E:/PhD/Development/Projects/Data/Trees/Newick_test1.tre");
 * @endcode
 */
class NewickIO
{
public:		
	/** Constructor. */
	NewickIO(): m_numElement(0), m_processedElement(NULL) {}
	/**
	* @brief Read a phylogenetic tree from a stream.
	*
	* @param tree Tree to populate from file.
	* @param in The input stream.
	* @return True if tree loaded successfully, false loading was cancelled.
	*/
	bool Read(Tree<Node>* tree, std::istream& in);

	/**
	* @brief Read a phylogenetic tree from a file.
	*
	* @param tree Tree to populate from file.
	* @param filename The file path.
	* @return True if tree loaded successfully, false loading was cancelled.
	*/
	bool Read(Tree<Node>* tree, const std::string& filename);

	/**
	* @brief Parse a string in Newick format and convert it to a tree.
	*
	* @param tree Tree to populate from file.
	* @param description String to parse.
	* @return True if tree loaded successfully, false loading was cancelled.
	*/
	bool ParseTree(Tree<Node>* tree, const std::string& description);

	/**
	* @brief Write a phylogenetic tree to a stream.
	*
	* @param tree Tree to write out to file.
	* @param out The output stream.
	* @throw IOException If output stream is invalid
	*/
	void Write(Tree<Node>* tree, std::ostream & out) const; 

  /**
   * @brief Write a phylogenetic tree to a file.
   *
	 * @param tree Tree to write out to file.
   * @param filename The file path.
   * @param overwrite Tell if existing file should be overwritten or appended to.
	 * @throw IOException If file can not be opened.
   */
	void Write(Tree<Node>* tree, const std::string & filename, bool overwrite = true) const
	{
		std::ofstream output(filename.c_str(), overwrite ? (std::ios::out) : (std::ios::out|std::ios::app));
		Write(tree, output); 
		output.close();
	}

protected:
		/**
     * @brief Different elements that comprise a Newick node.
     */
		struct Element
    {
			std::string node;
			std::string name;
      std::string length;
      std::string bootstrap;
    };

		/**
     * @brief Parse a string in Newick format and return a single paranthesized element.
     *
     * @param description String to parse.
     * @return Newick element
     */
		Element Elements(const std::string& description);

    /**
     * @brief Parse a string in Newick format and convert it to a subtree.
     *
		 * @param tree Tree to populate from file.
     * @param description String to parse.
		 * @param parentID Id of parent node.
		 * @return True if tree loaded successfully, false if loading was cancelled.
     */
		bool ParseNode(Tree<Node>* tree, const std::string& description, Node* parent);
  
		/**
     * @brief Write elements of a node to file in Newick format.
     *
		 * @param tree Tree to write out to file.
     * @param out The output stream.
		 * @param parentID Index of parent node.
		 * @param childID Node structure of the child node.
     */
		void WriteElements(Tree<Node>* tree, std::ostream& out, Node* parent, Node* child) const;

		/**
     * @brief Write each node to file in Newick format.
     *
		 * @param tree Tree to write out to file.
     * @param out The output stream.
		 * @param parentID Index of parent node.
     */
		void WriteNodes(Tree<Node>* tree, std::ostream& out, Node* parent) const;

protected:
	/** Number of elements to be read from the file (useful for indicating loading progress). */
	uint m_numElement;

	/** Track number of elements processed so far (useful for indicating loading progress and assigning unique node ids). */
	uint m_processedElement;
};


#endif

