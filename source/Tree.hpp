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

#include "Node.hpp"

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

	/** Get all nodes in tree. */
	std::vector<N*> GetNodes(N* node);

	/** Get phylogenetic distance between two nodes. */
	double GetPhylogeneticDistance(N* node1, N* node2);

	/** Get phylogenetic distance to root. */
	double GetDistanceToRoot(N* node);

	/**
	 * @brief Project tree onto the specified set of leaf nodes.
	 * @param leavesToRetain List of leave nodes to retain in tree.
	 */
	void Project(const std::vector<std::string>& leavesToRetain);

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

	/**
	 * @brief Retrieve all son nodes from a subtree.
	 * @param node Node that defines the subtree.
	 * @param nodes Vector of pointers toward each son node in the subtree.
	 */
	static void GetNodes(N* node, std::vector<N*>& nodes);

private:
	N* m_root;
	std::string m_name;		
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

template <class N>
void Tree<N>::Project(const std::vector<std::string>& leavesToRetain)
{
	// mark all internal node as active so we can distinguish them from true leaf nodes
	std::vector<N*> nodes = GetNodes(m_root);
	for(uint i = 0; i < nodes.size(); ++i)
		nodes[i]->SetCounter(!nodes[i]->IsLeaf());

	// 1. Removes leave nodes from the tree.
	std::set<std::string> leavesToRetainSet;
	for(uint i = 0; i < leavesToRetain.size(); ++i)
		leavesToRetainSet.insert(leavesToRetain[i]);

	std::vector<N*> leaves = GetLeaves(m_root);
	for(uint i = 0; i < leaves.size(); ++i)
	{
		if(!leavesToRetainSet.count(leaves[i]->GetName()))
		{
			leaves[i]->GetParent()->RemoveChild(leaves[i]);
			delete leaves[i];
		}
	}

	// 2. Collapse any internal nodes that have less than 2 children. This
	// is done in a breadth first manner from the leaf nodes to the root node.
	std::vector<N*> curNodes = GetLeaves(m_root);
	std::set<N*> nextNodes;
	while(!curNodes.empty())
	{
		nextNodes.clear();
		for(uint i = 0; i < curNodes.size(); ++i)
		{
			N* node = curNodes[i];

			if(!node->IsRoot())
				nextNodes.insert(node->GetParent());

			if(node->GetCounter() && node->GetNumberOfChildren() == 0)
			{
				if(node->IsRoot())
				{
					// we have a root with no children so just leave it as the sole node in the tree
				}
				else
				{
					// remove this node from the tree
					node->GetParent()->RemoveChild(node);
					nextNodes.erase(node);
					delete node;
				}
			}
			else if(node->GetCounter() && node->GetNumberOfChildren() == 1)
			{		
				if(node->IsRoot())
				{
					// the root is degenerate so we must make its sole child the new root
					SetRootNode(node->GetChild(0));
					node->GetChild(0)->SetParent(NULL);
					node->GetChild(0)->SetDistanceToParent(Node::NO_DISTANCE);
					nextNodes.erase(node);
					delete node;
				}
				else
				{
					// remove node from tree after assigning its sole child to its parent
					node->GetParent()->AddChild(node->GetChild(0));	
					
					if(node->GetChild(0)->GetDistanceToParent() != Node::NO_DISTANCE)
					{
						// keep track of branch lengths
						node->GetChild(0)->SetDistanceToParent(node->GetChild(0)->GetDistanceToParent() 
																											+ node->GetDistanceToParent()); 
					}

					node->GetParent()->RemoveChild(node);
					nextNodes.erase(node);
					delete node;
				}
			}	
		}

		curNodes.clear();
		std::copy(nextNodes.begin(), nextNodes.end(), std::back_inserter(curNodes));
	}
}

template <class N>
std::vector<N*> Tree<N>::GetNodes(N* node)
{
	std::vector<N*> nodes;
	GetNodes(node, nodes);
	return nodes;
}

template <class N>
void Tree<N>::GetNodes(N* node, std::vector<N*>& nodes)
{
	for(unsigned int i = 0; i < node->GetNumberOfChildren(); i++)
	{
		GetNodes(node->GetChild(i), nodes);
	}

	nodes.push_back(node);
}

#endif	
