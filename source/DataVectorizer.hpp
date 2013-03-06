//=======================================================================
// Author: Donovan Parks
//
// Copyright 2011 Donovan Parks
//
// This file is part of ExpressBetaDiversity.
//
// ExpressBetaDiversity is free software: you can redistribute it 
// and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation, either version 3 of 
// the License, or (at your option) any later version.
//
// ExpressBetaDiversity is distributed in the hope that it will be 
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ExpressBetaDiversity. If not, see 
// <http://www.gnu.org/licenses/>.
//=======================================================================

#ifndef _DATA_VECTORIZER_
#define _DATA_VECTORIZER_

#include "Precompiled.hpp"

#include "Tree.hpp"
#include "Node.hpp"

/**
 * @brief Calculate various vectorial representation of sequence data.
 */
class DataVectorizer
{
public:		
	/** Constructor. */
	DataVectorizer(): m_size(0), m_bWeighted(false), m_bPhylogenetic(false) {}

	/** Initialize. */
	bool Init(Tree<Node>* tree, bool bPhylogenetic, bool bWeighted, bool bNormalize, const std::vector<std::string>& seqs);

	/**
	* @brief Calculate data vector for tree.
	*
	* @param count Count data for leaf nodes.
	* @param bLeavesOnly Flag indicating if data vector should only be calculated over leaf nodes.
	* @param totalNumSeq Sum of count data.
	* @param data Data to be calculated.
	*/
	void CalculateDataVector(const std::vector<double>& count, bool bLeavesOnly, double totalNumSeq, std::vector<double>& data);

	/** 
	* @brief Restrict branch vector to the MRCA tree.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param MRCAi MRCA vector for sample i.
	* @param MRCAj MRCA vector for sample j.
	* @param branchWeight Branch weights over MRCA subtree.
	*/
	void RestrictToMRCA(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& MRCAi, std::vector<double>& MRCAj, std::vector<double>& branchWeight);

	/** 
	* @brief Find proportion weighted minimum distance between leaf nodes of two communities.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param minLeafI Proportion weighted minimum distance from leaf nodes in community i to leaf nodes in community j. 
	* @param minLeafJ Proportion weighted minimum distance from leaf nodes in community j to leaf nodes in community i. 
	*/
	void LeafSetMinDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& minLeafI, std::vector<double>& minLeafJ);

	/** 
	* @brief Find proportion weighted mean distance between leaf nodes of two communities.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param meanLeafI Proportion weighted mean distance from leaf nodes in community i to leaf nodes in community j. 
	* @param meanLeafJ Proportion weighted mean distance from leaf nodes in community j to leaf nodes in community i. 
	*/
	void LeafSetMeanDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& meanLeafI, std::vector<double>& meanLeafJ);

	/** 
	* @brief Find leaf sets for each community and distances between leaves.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param leafPropI Proportions for leaves in sample i.
	* @param leafPropJ Proportions for leaves in sample j.
	* @param leafDistances Distances from a leaf in sample i to a leaf in sample j.
	*/
	void LeafSetDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& leafPropI, std::vector<double>& leafPropJ, 
																					std::vector< std::vector<double> >& leafDistances);

	/** 
	* @brief Find leaf sets spanning two communities and distances between their leaves.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param leafPropI Proportions for sample i for leaves in sample i or j.
	* @param leafPropJ Proportions for sample j for leaves in sample i or j.
	* @param leafDistances Distances from a leaf i to j.
	*/
	void PairedLeafSetDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& leafPropI, std::vector<double>& leafPropJ, 
																					std::vector< std::vector<double> >& leafDistances);

	/** 
	* @brief Get distances between leaves.
	*
	* @param colSum Sum of proportions across all samples for each column in the data vector.
	* @param leafColSum Sum of proportions across all samples for leaf nodes.
	* @param leafDistances Distances from a leaf i to j.
	*/
	void FullLeafSetDistance(const std::vector<double>& colSum, std::vector<double>& leafColSum, std::vector< std::vector<double> >& leafDistances);

	/** 
	* @brief Find proportion weighted minimum distance between leaf nodes of two communities.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param rootDistI Community i proportion weighted distances from leaf nodes in community i or j to the root. 
	* @param rootDistJ Community j proportion weighted distances from leaf nodes in community i or j to the root. 
	*/
	void LeafSetRootDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& rootDistI, std::vector<double>& rootDistJ);

	/** 
	* @brief Apply MRCA weightings to each branch.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param branchWeightsMRCA MRCA weighted branch lengths in post-order traversal order.
	*/
	void ApplyWeightsMRCA(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& branchWeightsMRCA);
		
	/** Get size of data vector. */
	uint GetSize() const { return m_size; }

private:
	/** 
	* @brief Find proportion weighted distances between leaf nodes of two communities.
	*
	* @param branchVecI Branch vector for sample i.
	* @param branchVecJ Branch vector for sample j.
	* @param meanLeafI Proportion weighted distance from leaf nodes in community i to leaf nodes in community j. 
	* @param meanLeafJ Proportion weighted distance from leaf nodes in community j to leaf nodes in community i. 
	* @param bMinOrMean Calculate either minimum (true) or mean (false) distances between leaf nodes.
	*/
	void LeafSetMinMeanDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& meanLeafI, std::vector<double>& meanLeafJ, bool bMinOrMean);

private:
	/** Tree for phylogenetic beta-diversity calculations. */
	Tree<Node>* m_tree;

	/** Vector of nodes in tree in post-order traversal order. */
	std::vector<Node*> m_postOrder;

	/** Vector of nodes in tree in breadth-first traversal order. */
	std::vector<Node*> m_breadthFirstOrder;

	/** Flag indicating if weighted vectors are to be generated. */
	bool m_bWeighted;

	/** Flag indicating if phylogenetic vectors are to be generated. */
	bool m_bPhylogenetic;

	/** Flag indicating if normalized data vectors are to be generated. */
	bool m_bNormalize;

	/** Size of data vector. */
	uint m_size;
};

#endif

