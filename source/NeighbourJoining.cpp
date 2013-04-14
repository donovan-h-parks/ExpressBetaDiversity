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

#include "NeighbourJoining.hpp"

using namespace std;

NeighbourJoining::~NeighbourJoining()
{
	delete[] m_separationSums;
	delete[] m_separations;
	delete[] m_activeClusters;
}

void NeighbourJoining::BuildTree(Matrix& distMatrix, const std::vector<std::string>& labels, Tree<Node>* tree)
{
	// allocation space for temporary variables
  m_separationSums = new double[distMatrix.size()];
  m_separations = new double[distMatrix.size()];
  m_numActiveClusters = distMatrix.size();      
  m_activeClusters = new bool[distMatrix.size()];

	//calculate initial seperation rows
  for(uint i = 0; i < distMatrix.size(); i++)
	{
    double sum = 0;
    for(uint j = 0; j < distMatrix.size(); j++)
      sum += distMatrix[i][j];

    m_separationSums[i] = sum;
    m_separations[i] = sum / (m_numActiveClusters-2); 
    m_activeClusters[i] = true;
  }

	// create initial singleton clusters
	std::vector< Node* > clusters;
	for(uint i = 0; i < labels.size(); ++i)
	{
		Node* node = new Node(labels.at(i));
		clusters.push_back(node);
	}

  while(m_numActiveClusters > 2)
	{
    FindNearestClusters(distMatrix);
    UpdateClusters(distMatrix, clusters);
    UpdateDistanceMatrix(distMatrix);                       
  }

  // finish by joining the two remaining clusters
  int index1 = -1;
  int index2 = -1;

  // find the last nodes
  for(uint i = 0; i < distMatrix.size(); i++)
	{
    if(m_activeClusters[i])
		{
      if(index1 == -1)
				index1 = i;
      else
			{
				index2 = i;	
				break;
      }            
    }
  }  

	// connect remaining subtrees and define arbitrary root of tree
	clusters.at(index1)->AddChild(clusters.at(index2));
	clusters.at(index2)->SetDistanceToParent(distMatrix[index1][index2]);

	tree->SetRootNode(clusters.at(index1));
}

void NeighbourJoining::FindNearestClusters(Matrix& distMatrix) 
{
  m_rowIndex = -1;
  m_colIndex = -1;
	double min = std::numeric_limits<double>::max();        
  for (uint i = 0; i < distMatrix.size(); i++) 
	{    
    if(m_activeClusters[i])
		{
      double sep1 = m_separations[i];
      for(uint j = 0; j < distMatrix.size(); j++)
			{
				if(m_activeClusters[j] && i != j)
				{                    
					double sep2 = m_separations[j];
					double val = distMatrix[i][j] - sep1 - sep2;                   

					if(val < min)
					{
						// new minimum
						m_rowIndex = i;
						m_colIndex = j;
						min = val;
					}					
				}
      }
    }
  }
}

void NeighbourJoining::UpdateDistanceMatrix(Matrix& distMatrix)
{    
  double newSeparationSum = 0;
  double mutualDistance = distMatrix[m_rowIndex][m_colIndex];
  for(uint i = 0; i < distMatrix.size(); i++)
	{
    if(!m_activeClusters[i] || i == m_rowIndex || i == m_colIndex)
		{
      distMatrix[m_rowIndex][i] = 0;
			distMatrix[i][m_rowIndex] = 0;
    } 
		else 
		{
      double val1 = distMatrix[m_rowIndex][i];
      double val2 = distMatrix[m_colIndex][i];
      double dist = (val1 + val2 - mutualDistance) / 2.0;
      newSeparationSum += dist;

      m_separationSums[i] += (dist - val1 - val2);
      m_separations[i] = m_separationSums[i] / (m_numActiveClusters-2); 
      distMatrix[m_rowIndex][i] = dist;
      distMatrix[i][m_rowIndex] = dist;
    }
  }

  m_separationSums[m_rowIndex] = newSeparationSum;
  m_separations[m_rowIndex] = newSeparationSum / (m_numActiveClusters-2);
  m_separationSums[m_colIndex] = 0;
  m_activeClusters[m_colIndex] = false; 
}

void NeighbourJoining::UpdateClusters(Matrix& distMatrix, std::vector<Node*>& clusters)
{
  // calculate distances
  double dist = distMatrix[m_rowIndex][m_colIndex];
  double sep1 = m_separations[m_rowIndex];
  double sep2 = m_separations[m_colIndex];
  double dist1 = (0.5 * dist) + (0.5 * (sep1 - sep2));
  double dist2 = dist - dist1;

	// create new node
	Node* node = new Node;

	// set children of this node
	node->AddChild(clusters.at(m_rowIndex));
	node->AddChild(clusters.at(m_colIndex));

	// set distance from children to parent
	clusters.at(m_rowIndex)->SetDistanceToParent(dist1);
	clusters.at(m_colIndex)->SetDistanceToParent(dist2);

	// add in new cluster to cluster vector and remove children clusters
	clusters[m_rowIndex] = node;
  m_numActiveClusters--;
}
