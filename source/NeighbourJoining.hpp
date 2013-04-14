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

#ifndef _NEIGHBOUR_JOINING_
#define _NEIGHBOUR_JOINING_

#include "DataTypes.hpp"

#include "Tree.hpp"
#include "Node.hpp"

class NeighbourJoining
{
 public:
	/** Constructor. */
	NeighbourJoining() {}

	/** Destructor. */
	~NeighbourJoining();

	/** 
	 * @brief Build neighbour joining (NJ) tree from a distance matrix.
	 * @param distMatrix Matrix indicating pairwise distance between objects.
	 * @param labels Labels identifying each row/col of the distance matrix.
	 * @param tree Resulting NJ tree.
	 */
	void BuildTree(Matrix& distMatrix, const std::vector<std::string>& labels, Tree<Node>* tree);
  
private:
	double* m_separationSums;
	double* m_separations;	
	int m_rowIndex, m_colIndex;

	int m_numActiveClusters;
	bool* m_activeClusters;

	void FindNearestClusters(Matrix& distMatrix);
	void UpdateClusters(Matrix& distMatrix, std::vector<Node*>& clusters);
	void UpdateDistanceMatrix(Matrix& distMatrix);
};

#endif
