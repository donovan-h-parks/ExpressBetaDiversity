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

#ifndef _CLUSTER_
#define _CLUSTER_

#include "Precompiled.hpp"

#include "DataTypes.hpp"

#include "Tree.hpp"
#include "Node.hpp"

class Cluster
{
public:
		/** Type of clustering to perform. */
	enum CLUSTER_TYPE { COMPLETE_LINKAGE, SINGLE_LINKAGE, AVERAGE_LINKAGE };

public:
	/** Constructor. */
	Cluster() {}

	/** Destructor. */
	~Cluster() {}

	/** 
	 * @brief Cluster distance matrix.
	 * @param clusterType Type of clustering to perform.
	 * @param distMatrix Matrix indicating pairwise distance between objects.
	 * @param labels Labels identifying each row/col of the distance matrix.
	 * @param tree Resulting hierarchical tree.
	 */
	static void Clustering(CLUSTER_TYPE clusterType, const Matrix& distMatrix, const std::vector<std::string>& labels, Tree<Node>* tree);


	/** 
	 * @brief Complete linkage clustering (aka, farthest neighbour clustering).
	 * @param distMatrix Matrix indicating pairwise distance between objects.
	 * @param labels Labels identifying each row/col of the distance matrix.
	 * @param tree Resulting hierarchical tree.
	 */
	static void CompleteLinkage(const Matrix& distMatrix, const std::vector<std::string>& labels, Tree<Node>* tree)
	{
		Clustering(COMPLETE_LINKAGE, distMatrix, labels, tree);
	}

	/** 
	 * @brief Single linkage clustering (aka, nearest neighbour clustering).
	 * @param distMatrix Matrix indicating pairwise distance between objects.
	 * @param labels Labels identifying each row/col of the distance matrix.
	 * @param tree Resulting hierarchical tree.
	 */
	static void SingleLinkage(const Matrix& distMatrix, const std::vector<std::string>& labels, Tree<Node>* tree)
	{
		Clustering(SINGLE_LINKAGE, distMatrix, labels, tree);
	}

	/**
	 * @brief Unweighted Pair Group Method with Arithmetic (UPGMA) mean clustering (aka, average linkage clustering).
	 * @param distMatrix Matrix indicating pairwise distance between objects.
	 * @param labels Labels identifying each row/col of the distance matrix.
	 * @param tree Resulting hierarchical tree.
	 */
	static void UPGMA(const Matrix& distMatrix, const std::vector<std::string>& labels, Tree<Node>* tree)
	{
		Clustering(AVERAGE_LINKAGE, distMatrix, labels, tree);
	}

protected:
	static void FindNearestClusters(Matrix& distMatrix, uint& row, uint& col);

	static double GetDistanceToNode(Node* node);

	static void UpdateClusters(std::vector<Node*>& clusters, uint row, uint col, double value);

	static void UpdateDistanceMatrix(CLUSTER_TYPE clusterType, Matrix& distMatrix, std::vector<Node*>& clusters, uint row, uint col);
};

#endif