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

#ifndef _TREE_
#define _TREE_

#include "Precompiled.hpp"

/**
 * @brief Build a new tree.
 */
template<class N> class Tree
{
public:	
	/** Constructor. */
	Tree(): m_root(NULL), m_name("")  {}

	/**
	 * @brief Constructor
	 * @param root Root of tree.
	 */
	Tree(N* root): m_root(root), m_name("") {}

	/** Destructor. */
	~Tree();

	/** Get name of tree. */
	std::string GetName() const  { return m_name; }

	/**
	 * @brief Set name of tree.
	 * @param name Desired name of tree.
	 */		
	void SetName(const std::string& name) { m_name = name; }

	/** Get number of nodes in tree. */
	uint GetNumNodes() const { return m_numNodes; }

	/** Set number of nodes in tree. */
	void SetNumNodes(uint numNodes) { m_numNodes = numNodes; }

	/** 
	 * @brief Set root node of tree.
	 * @param root Desired root node.
	 */
	void SetRootNode(N* root) 
	{ 
		m_root = root; 
		m_root->SetParent(NULL); 
		m_root->SetDistanceToParent(N::NO_DISTANCE); 
	}

	/** Get root node. */
	N* GetRootNode() const { return m_root; }

	/** Get all leaf nodes in tree. */
	std::vector<N*> GetLeaves(N* subtree) const;

	/** Get nodes in post-order traversal order within the specified subtree. */
	std::vector<N*> PostOrder(N* subtree) const;

	/** Get nodes in breadth-first traversal order within the specified subtree. */
	std::vector<N*> BreadthFirstOrder(N* subtree) const;

	/** Get phylogenetic distance between two nodes. */
	double GetPhylogeneticDistance(N* node1, N* node2);

	/** Get phylogenetic distance to root. */
	double GetDistanceToRoot(N* node);

private:
	void DestroySubtree(N* node);

	/**
	 * @brief Retrieve all leaves from a subtree.
	 * @param node Node that defines the subtree.
	 * @param leaves A vector of leaves in the subtree.
	 */
	void GetLeaves(N* node, std::vector<N*>& leaves) const;

	/**
	 * @brief Retrieve nodes from subtree in post-order traversal order.
	 * @param node Node that defines the subtree.
	 * @param nodes A vector of nodes in post-order traversal order.
	 */
	void PostOrder(N* node, std::vector<N*>& nodes) const;

private:
	N* m_root;
	std::string m_name;		

	uint m_numNodes;
};

// --- Function implementations -----------------------------------------------

template <class N>
Tree<N>::~Tree()
{
	if(m_root)
	{
		DestroySubtree(m_root);

		if(m_root)
		{
			delete m_root;
			m_root = NULL;
		}
	}
}
	
template <class N>
void Tree<N>::DestroySubtree(N* node)
{
	for(uint i = 0; i < node->GetNumberOfChildren(); i++)
	{
		N* child = node->GetChild(i);
		DestroySubtree(child);

		if(child)
		{
			delete child;
			child = NULL;
		}
	}
}

template <class N>
std::vector<N*> Tree<N>::GetLeaves(N* subtree) const
{
	std::vector<N*> leafList;
	GetLeaves(subtree, leafList);
	return leafList;
}

template <class N>
void Tree<N>::GetLeaves(N* node, std::vector<N*>& leafList) const
{
	if(node->IsLeaf())
		leafList.push_back(node);
	else
	{
		for(uint i = 0; i < node->GetNumberOfChildren(); i++)
			GetLeaves(node->GetChild(i), leafList);
	}
}

template <class N>
std::vector<N*> Tree<N>::PostOrder(N* subtree) const
{
	std::vector<N*> nodes;
	PostOrder(subtree, nodes);

	return nodes;
}

template <class N>
void Tree<N>::PostOrder(N* node, std::vector<N*>& nodes) const
{
	for(uint i = 0; i < node->GetNumberOfChildren(); i++)
		PostOrder(node->GetChild(i), nodes);

	nodes.push_back(node);
}

template <class N>
std::vector<N*> Tree<N>::BreadthFirstOrder(N* subtree) const
{
	std::vector<N*> nodes;

	std::queue<N*> q;
	q.push(subtree);
	while(!q.empty())
	{
		N* curNode = q.front();
		q.pop();

		nodes.push_back(curNode);
		std::vector<N*> children = curNode->GetChildren();
		for(uint i = 0; i < children.size(); ++i)
			q.push(children[i]);
	}

	return nodes;
}

template <class N>
double Tree<N>::GetPhylogeneticDistance(N* node1, N* node2)
{
	std::vector<N*> path;
	std::vector<N*> pathMatrix1;
	std::vector<N*> pathMatrix2;

	// path from node 1 to the root
	N* nodeUp = node1;
	while(!nodeUp->IsRoot())  
	{ 
		pathMatrix1.push_back(nodeUp);
		nodeUp = nodeUp->GetParent();
	}
	pathMatrix1.push_back(nodeUp); // The root.

	// path from node 2 to the root
	nodeUp = node2;
	while(!nodeUp->IsRoot())  
	{
		pathMatrix2.push_back(nodeUp);
		nodeUp = nodeUp->GetParent();
	}
	pathMatrix2.push_back(nodeUp); // The root.

	// find most terminal node where paths diverge
	int tmp1 = pathMatrix1.size() - 1;
	int tmp2 = pathMatrix2.size() - 1;

	while((tmp1 >= 0) && (tmp2 >= 0)) 
	{
		if (pathMatrix1[tmp1] != pathMatrix2[tmp2]) 
			break;
		tmp1--; tmp2--;
	}

	// get distance from node 1 to node 2
	double dist = 0;
	for (int i = 0; i <= tmp1; ++i)
		dist += pathMatrix1[i]->GetDistanceToParent();

	for (int i = tmp2; i >= 0; --i) 
		dist += pathMatrix2[i]->GetDistanceToParent();

	return dist;
}

template <class N>
double Tree<N>::GetDistanceToRoot(N* node)
{
	double dist = 0;
	N* curNode = node;
	while(!curNode->IsRoot())
	{
		dist += curNode->GetDistanceToParent();
		curNode = curNode->GetParent();
	}

	return dist;
}

#endif	
