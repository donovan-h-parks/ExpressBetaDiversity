//=======================================================================
// Author: Donovan Parks
//
// Copyright 2009 Donovan Parks
//
// This file is part of Chameleon.
//
// Chameleon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Chameleon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Chameleon. If not, see <http://www.gnu.org/licenses/>.
//=======================================================================

#ifndef _PARSIMONY_CALCULATOR_
#define _PARSIMONY_CALCULATOR_

#include "DataTypes.hpp"

#include "Tree.hpp"
#include "Node.hpp"

typedef struct sCHARACTER_DATA
{
	sCHARACTER_DATA(): score(0), bPresent(false) {}

	uint score;
	bool bPresent;
} CharacterData;

typedef struct sPARSIMONY_DATA
{
	sPARSIMONY_DATA(): nodeScore(0), consistencyIndex(0) {}

	float GetConsistencyIndex() { return consistencyIndex; }
	uint GetNodeScore() { return nodeScore; }
	uint GetScore(const std::string& character) { return characterScores[character].score; }

	std::map<std::string, CharacterData> characterScores;

	uint nodeScore;
	float consistencyIndex;
	std::set<std::string> parsimoniousCharacters;

	static std::string field;
} ParsimonyData;

/**
 * @brief Calculates parsimony score (i.e., minimum number of changes required to explain the data)
 *				and assignment of internal labels leading to this parsimony score using the Sankoff algorithm.
 */
class ParsimonyCalculator
{
public:
	/** Constructor. */
	ParsimonyCalculator(): m_parsimonyScore(1) {}

	/** Destructor. */
	~ParsimonyCalculator() {}

	/** Calculate parismony score and assignment of labels giving this score. */

	/** 
	 * @brief Calculate parismony score and assignment of labels giving this score.
	 * @param tree Tree to calculate parsimony scores for.
	 * @param attributeMap Indicates attribure for each leaf node.
	 */
	void Run(Tree<Node>* tree, std::map<std::string, std::string>& attributeMap);

	int Score() const { return m_parsimonyScore; }
	float Consistency() const;

protected:
	/** Propagate parsimony scores up tree.*/
	void CalculateUp(Tree<Node>*  tree, const std::vector<std::string>& characters);

	/** Propagate most parsimonious character states down tree. */
	void CalculateDown(Tree<Node>*  tree, const std::vector<std::string>& characters);

	bool IsMissingData(const std::string& character);

protected:
	/** Parsimony data for each node (indexed by node id). */
	std::map<uint, ParsimonyData> m_parsimonyData;	

	/** Unique character set. */
	std::set<std::string> m_uniqueCharacters;

	/** Calculate parsimony score.*/
	int m_parsimonyScore;
};

#endif
