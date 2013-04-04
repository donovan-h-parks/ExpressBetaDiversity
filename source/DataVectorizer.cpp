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

#include "Precompiled.hpp"

#include "DataVectorizer.hpp"

bool DataVectorizer::Init(Tree<Node>* tree, bool bPhylogenetic, bool bWeighted, bool bNormalize, const std::vector<std::string>& seqs)
{
	m_tree = tree;
	m_bPhylogenetic = bPhylogenetic;
	m_bWeighted = bWeighted;
	m_bNormalize = bNormalize;

	if(m_bPhylogenetic)
	{
		m_breadthFirstOrder = m_tree->BreadthFirstOrder(m_tree->GetRootNode());

		m_postOrder = m_tree->PostOrder(m_tree->GetRootNode());
		m_size = m_postOrder.size()-1;	// ignore the branch of the root node

		// create map of leaf names to leaf nodes
		std::map<std::string, Node*> leafMap;
		std::vector<Node*>::const_iterator postOrderIt;
		for(postOrderIt = m_postOrder.begin(); postOrderIt != m_postOrder.end(); ++postOrderIt)
		{
			Node* curNode = *postOrderIt;
			if(!curNode->IsLeaf())
				continue;

			leafMap[curNode->GetName()] = curNode;
		}

		// assign sequence index for each node
		bool bMissingSeqs = false;
		for(uint i = 0; i < seqs.size(); ++i)
		{
			std::map<std::string, Node*>::iterator it = leafMap.find(seqs[i]);
			if(it == leafMap.end())
			{
				if(!bMissingSeqs)
					std::cout << "[Error] Failed to find the following sequences in the input tree:" << std::endl;

				bMissingSeqs = true;
				std::cout << seqs[i] << std::endl;
			}
			else
				it->second->SetSeqIndex(i);
		}

		if(bMissingSeqs)
			return false;

		// check that all leaf nodes have been assigned a sequence index and set the post-order
		// traversal index for each node
		for(uint i = 0; i < m_postOrder.size(); ++i)
		{
			Node* curNode = m_postOrder[i];
			curNode->SetPostOrderIndex(i);

			if(curNode->IsLeaf())
			{
				if(curNode->GetSeqIndex() == Node::NO_INDEX)
				{	
					std::cerr << "No sequence count data specified for the leaf node '" << curNode->GetName() << "'." << std::endl;
					return false;
				}
			}
		}
	}
	else
	{
		m_size = seqs.size();
	}

	return true;
}

void DataVectorizer::CalculateDataVector(const std::vector<double>& count, bool bLeavesOnly, double totalNumSeq, std::vector<double>& data)
{
	data.clear();
	data.reserve(m_size);

	if(m_bPhylogenetic)
	{
		std::vector<Node*>::iterator it;
		for(it = m_postOrder.begin(); it != m_postOrder.end(); ++it)
		{
			Node* curNode = *it;

			if(curNode->IsLeaf())
			{
				if(m_bNormalize)
					data.push_back(count[curNode->GetSeqIndex()] / totalNumSeq);
				else
					data.push_back(count[curNode->GetSeqIndex()]);
			}
			else if(!bLeavesOnly && !curNode->IsRoot())
			{
				double p = 0;
				for(uint i = 0; i < curNode->GetNumberOfChildren(); ++i)
					p += data[curNode->GetChild(i)->GetPostOrderIndex()];
				data.push_back(p);
			}
		}
	}
	else
	{
		std::vector<double>::const_iterator countIt;
		for(countIt = count.begin(); countIt != count.end(); ++countIt)
		{
			if(m_bNormalize)
				data.push_back(*countIt / totalNumSeq);
			else
				data.push_back(*countIt);
		}
	}

	if(!m_bWeighted)
	{
		// calculate binary equivalent of data vector
		std::vector<double>::iterator dataIt;
		for(dataIt = data.begin(); dataIt != data.end(); ++dataIt)
		{
			if(*dataIt > 0)
				*dataIt = 1.0;
		}
	}
}

void DataVectorizer::RestrictToMRCA(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																			std::vector<double>& MRCAi, std::vector<double>& MRCAj, std::vector<double>& branchWeight)
{
	MRCAi.clear();
	MRCAj.clear();
	branchWeight.clear();

	if(m_bPhylogenetic)
	{
		// determine leaf nodes in MRCA subtree and clear node counters
		std::vector<Node*> leavesInMRCA;
		for(uint i = 0; i < branchVecI.size(); ++i)
		{
			if(m_postOrder[i]->IsLeaf() && (branchVecI[i] > 0 || branchVecJ[i] > 0))
				leavesInMRCA.push_back(m_postOrder[i]);

			m_postOrder[i]->SetCounter(0);
		}

		// trace each leaf node in subtree to the root and count number of times
		// each internal node is encountered
		m_tree->GetRootNode()->SetCounter(0);
		std::vector<Node*>::iterator it;
		for(it = leavesInMRCA.begin(); it != leavesInMRCA.end(); ++it)
		{
			Node* curNode = *it;

			while(curNode != NULL)
			{
				curNode->IncCounter();
				curNode = curNode->GetParent();
			}
		}

		// start of MRCA subtree is the deepest node with the same count value as the root
		Node* root = m_postOrder[m_postOrder.size()-1];
		uint highestCount = root->GetCounter();
		Node* startMRCA = root;
		bool bNewMRCA = true;
		while(bNewMRCA)
		{
			bNewMRCA = false;
			std::vector<Node*> children = startMRCA->GetChildren();
			for(uint i = 0; i < startMRCA->GetNumberOfChildren(); ++i)
			{
				if(startMRCA->GetChild(i)->GetCounter() == highestCount)
				{
					startMRCA = startMRCA->GetChild(i);
					bNewMRCA = true;
					break;
				}
			}
		}
		
		// create MRCA data vectors
		std::vector<Node*> postOrderMRCA = m_tree->PostOrder(startMRCA);
		for(it = postOrderMRCA.begin(); it != postOrderMRCA.end(); ++it)
		{
			Node* curNode = *it;

			if(curNode != startMRCA)
			{
				MRCAi.push_back(branchVecI[curNode->GetPostOrderIndex()]);
				MRCAj.push_back(branchVecJ[curNode->GetPostOrderIndex()]);
				branchWeight.push_back(curNode->GetDistanceToParent());
			}
		}
	}
}

void DataVectorizer::LeafSetMinDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& minLeafI, std::vector<double>& minLeafJ)
{
	LeafSetMinMeanDistance(branchVecI, branchVecJ, minLeafI, minLeafJ, true);
}

void DataVectorizer::LeafSetMeanDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& meanLeafI, std::vector<double>& meanLeafJ)
{
	LeafSetMinMeanDistance(branchVecI, branchVecJ, meanLeafI, meanLeafJ, false);
}

void DataVectorizer::LeafSetMinMeanDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& leafI, std::vector<double>& leafJ, bool bMinOrMean)
{
	leafI.clear();
	leafJ.clear();

	if(m_bPhylogenetic)
	{
		// determine leaf nodes in communities i and j
		std::vector<Node*> leafSetI;
		std::vector<Node*> leafSetJ;
		for(uint i = 0; i < branchVecI.size(); ++i)
		{
			if(m_postOrder[i]->IsLeaf())
			{
				if(branchVecI[i] > 0)
					leafSetI.push_back(m_postOrder[i]);

				if(branchVecJ[i] > 0)
					leafSetJ.push_back(m_postOrder[i]);
			}
		}

		// find distance from leaf nodes in community i to leaf nodes in community j
		for(uint i = 0; i < leafSetI.size(); ++i)
		{
			double minDist = std::numeric_limits<double>::max();
			double meanDist = 0;
			for(uint j = 0; j < leafSetJ.size(); ++j)
			{
				double dist = m_tree->GetPhylogeneticDistance(leafSetI[i], leafSetJ[j]);
				if(dist < minDist)
					minDist = dist;

				meanDist += branchVecJ[leafSetJ[j]->GetPostOrderIndex()]*dist;
			}

			double prop = branchVecI[leafSetI[i]->GetPostOrderIndex()];

			if(bMinOrMean)
				leafI.push_back(prop*minDist);
			else
			{
				if(m_bWeighted)
					leafI.push_back(prop*meanDist);
				else
					leafI.push_back(prop*(meanDist/leafSetJ.size()));
			}
		}

		// find distance from leaf nodes in community j to leaf nodes in community i
		for(uint j = 0; j < leafSetJ.size(); ++j)
		{
			double minDist = std::numeric_limits<double>::max();
			double meanDist = 0;
			for(uint i = 0; i < leafSetI.size(); ++i)
			{
				double dist = m_tree->GetPhylogeneticDistance(leafSetJ[j], leafSetI[i]);
				if(dist < minDist)
					minDist = dist;

				meanDist += branchVecI[leafSetI[i]->GetPostOrderIndex()]*dist;
			}

			double prop = branchVecJ[leafSetJ[j]->GetPostOrderIndex()];

			if(bMinOrMean)
				leafJ.push_back(prop*minDist);
			else
			{
				if(m_bWeighted)
					leafJ.push_back(prop*meanDist);
				else
					leafJ.push_back(prop*(meanDist/leafSetI.size()));
			}
		}
	}
}

void DataVectorizer::LeafSetDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& leafPropI, std::vector<double>& leafPropJ, 
																					std::vector< std::vector<double> >& leafDistances)
{
	leafPropI.clear();
	leafPropJ.clear();
	leafDistances.clear();

	if(m_bPhylogenetic)
	{
		// determine leaf nodes in communities i and j
		std::vector<Node*> leafSetI;
		std::vector<Node*> leafSetJ;
		for(uint i = 0; i < branchVecI.size(); ++i)
		{
			if(m_postOrder[i]->IsLeaf())
			{
				if(branchVecI[i] > 0)
				{
					leafSetI.push_back(m_postOrder[i]);
					leafPropI.push_back(branchVecI[m_postOrder[i]->GetPostOrderIndex()]);
				}

				if(branchVecJ[i] > 0)
				{
					leafSetJ.push_back(m_postOrder[i]);
					leafPropJ.push_back(branchVecJ[m_postOrder[i]->GetPostOrderIndex()]);
				}
			}
		}

		// find distance from leaf nodes in community i to leaf nodes in community j
		for(uint i = 0; i < leafSetI.size(); ++i)
		{
			std::vector<double> distances;
			for(uint j = 0; j < leafSetJ.size(); ++j)
			{
				double dist = m_tree->GetPhylogeneticDistance(leafSetI[i], leafSetJ[j]);
				distances.push_back(dist);
			}

			leafDistances.push_back(distances);
		}
	}
}

void DataVectorizer::PairedLeafSetDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																					std::vector<double>& leafPropI, std::vector<double>& leafPropJ, 
																					std::vector< std::vector<double> >& leafDistances)
{
	leafPropI.clear();
	leafPropJ.clear();
	leafDistances.clear();

	if(m_bPhylogenetic)
	{
		// determine leaf nodes in communities i and j
		std::vector<Node*> leafSet;
		for(uint i = 0; i < branchVecI.size(); ++i)
		{
			if(m_postOrder[i]->IsLeaf())
			{
				if(branchVecI[i] > 0 || branchVecJ[i] > 0)
				{
					leafSet.push_back(m_postOrder[i]);
					leafPropI.push_back(branchVecI[m_postOrder[i]->GetPostOrderIndex()]);
					leafPropJ.push_back(branchVecJ[m_postOrder[i]->GetPostOrderIndex()]);
				}
			}
		}

		// find distance between leaf nodes
		for(uint i = 0; i < leafSet.size(); ++i)
		{
			std::vector<double> distances;
			for(uint j = 0; j < leafSet.size(); ++j)
			{
				double dist = m_tree->GetPhylogeneticDistance(leafSet[i], leafSet[j]);
				distances.push_back(dist);
			}

			leafDistances.push_back(distances);
		}
	}
}

void DataVectorizer::FullLeafSetDistance(const std::vector<double>& colSum, std::vector<double>& leafColSum, std::vector< std::vector<double> >& leafDistances)
{
	leafDistances.clear();

	if(m_bPhylogenetic)
	{
		// determine leaf nodes
		std::vector<Node*> leafSet;
		for(uint i = 0; i < m_postOrder.size(); ++i)
		{
			if(m_postOrder[i]->IsLeaf())
			{
				leafSet.push_back(m_postOrder[i]);
				leafColSum.push_back(colSum[m_postOrder[i]->GetPostOrderIndex()]);
			}
		}

		// find distance between leaf nodes
		for(uint i = 0; i < leafSet.size(); ++i)
		{
			std::vector<double> distances;
			for(uint j = 0; j < i; ++j)
			{
				double dist = m_tree->GetPhylogeneticDistance(leafSet[i], leafSet[j]);
				distances.push_back(dist);
			}

			leafDistances.push_back(distances);
		}
	}
}

void DataVectorizer::LeafSetRootDistance(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, 
																				 std::vector<double>& rootDistI, std::vector<double>& rootDistJ)
{
	rootDistI.clear();
	rootDistJ.clear();

	if(m_bPhylogenetic)
	{
		// determine leaf nodes in communities i or j
		std::vector<Node*> leafSet;
		for(uint i = 0; i < branchVecI.size(); ++i)
		{
			if(m_postOrder[i]->IsLeaf())
			{
				if(branchVecI[i] > 0 || branchVecJ[i] > 0)
					leafSet.push_back(m_postOrder[i]);
			}
		}

		// calculate distance from leaf nodes in community i or j weighted by seq. proportions
		for(uint i = 0; i < leafSet.size(); ++i)
		{
			double dist = m_tree->GetDistanceToRoot(leafSet[i]);

			rootDistI.push_back(branchVecI[leafSet[i]->GetPostOrderIndex()] * dist);
			rootDistJ.push_back(branchVecJ[leafSet[i]->GetPostOrderIndex()] * dist);
		}
	}
}

void DataVectorizer::ApplyWeightsMRCA(const std::vector<double>& branchVecI, const std::vector<double>& branchVecJ, std::vector<double>& branchWeightsMRCA)
{
	// calculate MRCA weighting of each node	
	for(uint i = 0; i < m_breadthFirstOrder.size(); ++i)
	{
		Node* curNode = m_breadthFirstOrder[i];

		if(!curNode->IsLeaf())
		{
			double sumAvgProps = 0;
			double maxProp = 0;
			std::vector<Node*> children = curNode->GetChildren();
			for(uint c = 0; c < children.size(); ++c)
			{
				double propI = branchVecI[children[c]->GetPostOrderIndex()];
				double propJ = branchVecJ[children[c]->GetPostOrderIndex()];

				double avgProp = 0.5*(propI+propJ);
				sumAvgProps += avgProp;

				if(avgProp > maxProp)
					maxProp = avgProp;

			}

			double weight = sumAvgProps - maxProp;

			if(curNode->IsRoot())
				curNode->SetValue(weight);
			else
				curNode->SetValue(weight + curNode->GetParent()->GetValue());
		}
	}

	// get weightings for each branch in post-order traversal order
	branchWeightsMRCA.clear();
	for(uint i = 0; i < m_size; ++i)
	{
		double branchLength = m_postOrder[i]->GetDistanceToParent();
		double mrcaWeight = m_postOrder[i]->GetParent()->GetValue();
		branchWeightsMRCA.push_back(mrcaWeight*branchLength);
	}
}