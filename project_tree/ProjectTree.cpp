//=======================================================================
// Author: Donovan Parks
//
// Copyright 2013 Donovan Parks
//
// This file is part of ProjectTree.
//
// ProjectTree is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ProjectTree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ProjectTree. If not, see <http://www.gnu.org/licenses/>.
//=======================================================================

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <cmath>

#include "Tree.hpp"
#include "NodePhylo.hpp"
#include "NewickIO.hpp"
#include "StringTools.hpp"

/**
 * Display program help info.
 */
void Help(char *argv[])
{
	std::cout << "ProjectTree v1.0.0 (Feb. 5, 2013)" << std::endl;
	std::cout << "  by Donovan Parks (donovan.parks@gmail.com) and Rob Beiko (beiko@cs.dal.ca)" << std::endl;
	std::cout << std::endl;
	std::cout << "  Usage: " << argv[0] << " <seq-count-file> <input tree> <output tree>" << std::endl;
}

int main(int argc, char *argv[])
{
	// parse command line
	if(argc != 4 && argc != 5)
	{
		Help(argv);
		return 0;
	}

	std::string seqFile(argv[1]);
	std::string treeFile(argv[2]);
	std::string outputFile(argv[3]);

	bool bVerify = false;
	if(argc == 5)
	{
		if(std::string(argv[4]) == "-v")
			bVerify = true;
	}

	// read in tree
	Tree<NodePhylo> tree;
	NewickIO newickIO;
	newickIO.Read(tree, treeFile);
		
	// get set of taxa to project tree onto
	std::vector<std::string> leavesToRetain;
	std::ifstream seqStream(seqFile.c_str());
	if(!seqStream.is_open())
	{
		std::cerr << "Failed to open sequence count file: " << seqFile << std::endl;
		return -1;
	}

	std::string seqStr;
	getline(seqStream, seqStr, '\n'); 
	std::istringstream seqs(util::StringTools::RemoveSurroundingWhiteSpaces(seqStr));

	std::string seq;
	while(std::getline(seqs, seq, '\t'))
	{
		if(!seq.empty())
			leavesToRetain.push_back(seq);
	}

	seqStream.close();

	// calculate distance between all pairs of nodes
	std::vector<double> origDist;
	if(bVerify)
	{
		std::cout << "Calculating distance between all pairs of retained nodes." << std::endl << std::endl;
		for(uint i = 0; i < leavesToRetain.size(); ++i)
		{
			NodePhylo* nodeI = tree.SearchLeavesForName(leavesToRetain[i]);

			for(uint j = 0; j < i; ++j)
			{
				NodePhylo* nodeJ = tree.SearchLeavesForName(leavesToRetain[j]);
				double dist = TreeTools<NodePhylo>::GetDistanceBetweenAnyTwoNodes(nodeI, nodeJ);
				origDist.push_back(dist);
			}
		}
	}

	// project tree onto specified taxa
	std::cout << "Projecting tree of " << tree.GetNumberOfLeaves() << " leaf nodes onto " << leavesToRetain.size() << " leaf nodes." << std::endl;
	std::cout << "  Total branch length of original tree: " << tree.GetTotalLength() << std::endl;
	tree.Project(leavesToRetain);
	std::cout << "  Total branch length of projected tree: " <<tree.GetTotalLength()  << std::endl;

	// write out projected tree
	newickIO.Write(tree, outputFile);

	// verify distance between all pairs of nodes in projected tree
	if(bVerify)
	{
		std::cout << std::endl;
		std::cout << "Verifying projection." << std::endl;
		uint index = 0;
		for(uint i = 0; i < leavesToRetain.size(); ++i)
		{
			NodePhylo* nodeI = tree.SearchLeavesForName(leavesToRetain[i]);

			for(uint j = 0; j < i; ++j)
			{
				NodePhylo* nodeJ = tree.SearchLeavesForName(leavesToRetain[j]);
				double dist = TreeTools<NodePhylo>::GetDistanceBetweenAnyTwoNodes(nodeI, nodeJ);
				if(fabs(dist - origDist[index]) > 1e-6)
				{
					std::cerr << "[ERROR] Projection appears to have failed. Project tree should not be used. Please contact Donovan Parks (donovan.parks@gmail.com)." << std::endl;
					return -1;
				}
				index++;
			}
		}

		std::cout << "Projection verified. Pairwise distances between retain nodes are the same in the original and project trees." << std::endl;
	}

	return 0;
}
