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

#include "DataTypes.hpp"
#include "NewickIO.hpp"
#include "Node.hpp"
#include "Tree.hpp"
#include "TreeTools.hpp"
#include "StringTools.hpp"
#include "Cluster.hpp"

int main(int argc, char *argv[])
{
	if(argc < 4)
	{	
		std::cout << "ClusterTree v1.0.0 (Feb. 1, 2013)" << std::endl;
		std::cout << "  by Donovan Parks (donovan.parks@gmail.com) and Rob Beiko (beiko@cs.dal.ca)" << std::endl;
		std::cout << std::endl;
		std::cout << "  Usage: " << argv[0] << " <method> <input matrix> <output tree>" << std::endl;
		std::cout << "    <method> = NJ, UPGMA, SingleLinkage, CompleteLinkage" << std::endl;
		return 0;
	}
	std::string methodStr = argv[1];
	std::string inputFile = argv[2];
	std::string outputFile = argv[3];

	// read dissimilarity matrix
	Matrix dissMatrix;
	std::vector<std::string> labels;

	std::ifstream fin(inputFile.c_str());
	uint numSamples;
	fin >> numSamples;

	dissMatrix.resize(numSamples);
	for(uint i = 0; i < numSamples; ++i)
	{
		dissMatrix[i].resize(numSamples, 0);

		std::string label;
		fin >> label;
		labels.push_back(label);

		for(uint j = 0; j < i; ++j)
		{
			double v;
			fin >> v;
			dissMatrix[i][j] = dissMatrix[j][i] = v;
		}
	}

	fin.close();

	// calculate NJ tree for provided dissimilarity matrix
	Tree<Node>* clusterTree(new Tree<Node>);
	NewickIO newickIO;
	
	if(methodStr == "NJ")
		Cluster::Clustering(Cluster::NEIGHBOUR_JOINING, dissMatrix, labels, clusterTree);
	else if(methodStr == "UPGMA")
		Cluster::Clustering(Cluster::AVERAGE_LINKAGE, dissMatrix, labels, clusterTree);
	else if(methodStr == "SingleLinkage")
		Cluster::Clustering(Cluster::SINGLE_LINKAGE, dissMatrix, labels, clusterTree);
	else if(methodStr == "CompleteLinkage")
		Cluster::Clustering(Cluster::COMPLETE_LINKAGE, dissMatrix, labels, clusterTree);

	newickIO.Write(clusterTree, outputFile);

	return 0;
}