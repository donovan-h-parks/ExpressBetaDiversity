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

// Based on code by: Julien Dutheil and Celine Scornavacca (Bio++ project)

#ifndef _TREE_
#define _TREE_

#include "TreeTools.hpp"
#include "DataTypes.hpp"

#include <string>
#include <ostream>
#include <iostream>
#include <set>
#include <iterator>
#include <vector>

/**
 * @brief Build a new tree. The nodes of the tree can be any class derived from Node.
 *
 * Code example:
 * @code
 * @endcode
 *
 * @see Node
 */
template<class N> class Tree
{
public:	
	/** Constructor. */
	Tree(): m_root(NULL), m_name("") {}

	/**
	 * @brief Constructor
	 * @param root Root of tree.
	 */
	Tree(N* root): m_root(root), m_name("") {}

	/** Copy constructor. */
	Tree(const Tree<N>& t);

	/** Assignment operator. */
	Tree<N>& operator=(const Tree<N>& t);

	/** Destructor. */
	~Tree();

	/** Clone tree. */
	Tree* Clone() const { return new Tree<N>(*this); }

public:
	/** Get name of tree. */
	std::string GetName() const  { return m_name; }

	/**
	 * @brief Set name of tree.
	 * @param name Desired name of tree.
	 */		
	void SetName(const std::string & name) { m_name = name; }

	/** 
	 * @brief Set root node of tree.
	 * @param root Desired root node.
	 */
	void SetRootNode(N* root) { m_root = root; }

	/** Get root node. */
	N* GetRootNode() const { return m_root; }

	/** Get id of root node. */
	unsigned int GetRootId() const { return m_root->GetId(); }

	/** Get node with the given id. */	
	N* GetNode(unsigned int id) const; 

	/** Get node with the given name. */
	N* GetNode(const std::string& name) const;

	/** Get number of leaf nodes in tree. */
	unsigned int GetNumberOfLeaves() const { return TreeTools<N>::GetNumberOfLeaves(m_root); }

	/** Get all leaf nodes in tree. */
	std::vector<N*> GetLeaves() const { return TreeTools<N>::GetLeaves(m_root); }

	/** Search for leaf with a given name. */
	N* SearchLeavesForName(const std::string& name) const
	{
		std::vector<N*> leaves = GetLeaves();
		for(uint i = 0; i < leaves.size(); ++i)
		{
			if(leaves[i]->GetName() == name)
				return leaves[i];
		}

		return 0;
	}

	/** Get id of all leaf nodes. */
	std::vector<unsigned int> GetLeafIds() const { return TreeTools<N>::GetLeafIds(m_root); }

	/** Get name of all leaf nodes in depth first order. */
	std::vector<std::string> GetLeafNames() const { return TreeTools<N>::GetLeafNames(m_root); }

	/** Get number of nodes in tree. */
	unsigned int GetNumberOfNodes() const { return TreeTools<N>::GetNumberOfNodes(m_root); }

	/** Get all nodes in tree. */
	std::vector<N*> GetNodes() const { return TreeTools<N>::GetNodes(m_root); }

	/** Get id of all nodes. */
	std::vector<unsigned int> GetNodesId() const { return TreeTools<N>::GetNodesId(m_root); }

	/** Get length of all branches. */
	std::vector<double> GetBranchLengths() const { return TreeTools<N>::GetBranchLengths(m_root); }

	/** Get depth of a give node (i.e. the maximum number of children 'generations'). */
	unsigned int GetDepth(N* node) const { return TreeTools<N>::GetDepth(node); }

	/** Get height of a give node (i.e. maximum distance between a leaf and the root of the subtree). */
	unsigned int GetHeight(N* node) const { return TreeTools<N>::GetHeight(node); }

	/** 
	 * @brief Get id of all children to the given node. 
	 * @param parentId Id of parent node.
	 * @return Id of all chilren to the node with the given id.
	 */
	std::vector<unsigned int> GetChildrenIds(unsigned int parentId) const { return GetNode(parentId)->GetChildrenIds(); }

	/** 
	 * @brief Get id of parent node to the given node.
	 * @param nodeId Id of node of interest.
	 * @return Id of parent node.
	 */
	unsigned int GetParentId(unsigned int nodeId) const { return GetNode(nodeId)->GetParentId(); }

	/** 
	 * @brief Get name of node.
	 * @param nodeId Id of node of interest.
	 * @return Name of node.
	 */
	std::string GetNodeName(int nodeId) const { return GetNode(nodeId)->GetName(); }

	/** 
	 * @brief Set name of node.
	 * @param nodeId Id of node of interest.
	 * @param name Desired name of node.
	 */
	void SetNodeName(int nodeId,  const std::string & name) { GetNode(nodeId)->SetName(name); }

	/** 
	 * @brief Indicate if tree has a node with a given id.
	 * @param nodeId Id to check.
	 * @return True if tree has a node with the given id.
	 */
	bool HasNode(unsigned int nodeId) const { return TreeTools<N>::HasNodeWithId(m_root, nodeId); }

	/** 
	 * @brief Indicate if node is a leaf.
	 * @param nodeId Id of node of interest.
	 * @return True if node is a leaf.
	 */
	bool IsLeaf(unsigned int nodeId) const { return GetNode(nodeId)->IsLeaf(); }

	/** 
	 * @brief Indicate if node is the root.
	 * @param nodeId Id of node of interest.
	 * @return True if node is the root.
	 */
	bool IsRoot(unsigned int nodeId) const { return TreeTools<N>::IsRoot(GetNode(nodeId)); }

	/** 
	 * @brief Get distance to parent node for the specified node.
	 * @param nodeId Id of node of interest.
	 * @return Distance to parent node.
	 */
	double GetDistanceToParent(unsigned int nodeId) const { return GetNode(nodeId)->GetDistanceToParent(); }

	/** 
	 * @brief Set distance to parent node from the specified node.
	 * @param nodeId Id of node of interest.
	 * @param dist Distance to parent node.
	 */
	void SetDistanceToParent(unsigned int nodeId, double dist)  { GetNode(nodeId)->SetDistanceToParent(dist); }
		
	/** 
	 * @brief Reset node ids so they are all unique values.
	 * @param nodeId Id of node of interest.
	 * @param dist Distance to parent node.
	 */
	void ResetNodeIds();
	
	/** Indicate if tree is multifurcating. */
	bool IsMultifurcating() const;
	
	/** Get total length of all branches in tree. */
	double GetTotalLength() const;

	/** 
	 * @brief Set all branches to the specified length.
	 * @param length Desired length.
	 */
	void SetBranchLengths(double length);

	/**
	 * @brief Scale tree distances by a constant factor.
	 * @param factor Amount to scale tree distances by.
	 */
	void ScaleTree(double factor);

	/**
	 * @brief Project tree onto the specified set of leaf nodes.
	 * @param leavesToRetain List of leave nodes to retain in tree.
	 */
	void Project(const std::vector<std::string>& leavesToRetain);
 
protected:		
		void DestroySubtree(N* node);

protected:
	N* m_root;
	std::string m_name;		
};

// --- Function implementations -----------------------------------------------

template <class N>
Tree<N>::Tree(const Tree<N>& t): m_root(NULL), m_name(t.GetName())
{
	//Perform a hard copy of the nodes:
	m_root = TreeTools<N>::CloneSubtree(t.GetRootNode());
}

template <class N>
Tree<N>& Tree<N>::operator=(const Tree<N>& t)
{
	// not implemented

	return *this;
}

template <class N>
Tree<N>::~Tree()
{
	if(m_root)
	{
		DestroySubtree(m_root);
		delete m_root;
	}
}
	
template <class N>
void Tree<N>::DestroySubtree(N* node)
{
	for(unsigned int i = 0; i < node->GetNumberOfChildren(); i++)
	{
		N* child = node->GetChild(i);
		DestroySubtree(child);

		delete child;
	}
}

template <class N>
N* Tree<N>::GetNode(unsigned int id) const
{
	std::vector<N*> nodes = TreeTools<N>::SearchNodeWithId(m_root, id);

	if(nodes.size() > 1)
	{
		std::cerr << "Node::GetNode(): multiple nodes have the same id." << std::endl;
		return NULL;
	}
	else if(nodes.size() == 0)
	{
		std::cerr << "Node::GetNode(): no nodes with the provided id." << std::endl;
		return NULL;
	}

	return nodes[0];
}

template <class N>
N* Tree<N>::GetNode(const std::string& name) const
{
	std::vector<N*> nodes = TreeTools<N>::SearchNodeWithName(m_root, name);

	if(nodes.size() > 1)
	{
		std::cerr << "Node::GetNode(): multiple nodes have the same id." << std::endl;
		return NULL;
	}
	else if(nodes.size() == 0)
	{
		std::cerr << "Node::GetNode(): no nodes with the provided id." << std::endl;
		return NULL;
	}

	return nodes[0];
}

template <class N>
void Tree<N>::ResetNodeIds()
{
	std::vector<N*> nodes = GetNodes();
	for(unsigned int i = 0; i < nodes.size(); i++)
  {
    nodes[i]->SetId(i);
  }
}

template <class N>
bool Tree<N>::IsMultifurcating() const
{
	bool b = false;
	for(unsigned int i = 0; i < m_root->GetNumberOfChildren(); i++)
  {
		b = b || TreeTools<N>::IsMultifurcating(m_root->GetChild(i));
	}
	return b;
}

template <class N>
double Tree<N>::GetTotalLength() const
{
	double length = 0;
	for(unsigned int i = 0; i < m_root->GetNumberOfChildren(); i++)
  {
    length += TreeTools<N>::GetTotalLength(m_root->GetChild(i));
  }
  return length;
}

template <class N>
void Tree<N>::SetBranchLengths(double length)
{
	for(unsigned int i = 0; i < m_root->GetNumberOfChildren(); i++)
  {
    TreeTools<N>::SetBranchLengths(m_root->GetChild(i), length);
  }
}

template <class N>
void Tree<N>::ScaleTree(double factor)
{
	for(unsigned int i = 0; i < m_root->GetNumberOfChildren(); i++)
  {
	  TreeTools<N>::ScaleTree(m_root->GetChild(i), factor);
  }
}

template <class N>
void Tree<N>::Project(const std::vector<std::string>& leavesToRetain)
{
	// mark all internal node as active so we can distinguish them from true leaf nodes
	std::vector<N*> nodes = GetNodes();
	for(uint i = 0; i < nodes.size(); ++i)
		nodes[i]->SetProcessed(!nodes[i]->IsLeaf());

	// 1. Removes leave nodes from the tree.
	std::set<std::string> leavesToRetainSet;
	for(uint i = 0; i < leavesToRetain.size(); ++i)
		leavesToRetainSet.insert(leavesToRetain[i]);

	std::vector<N*> leaves = GetLeaves();
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
	std::vector<N*> curNodes = GetLeaves();
	std::set<N*> nextNodes;
	while(!curNodes.empty())
	{
		nextNodes.clear();
		for(uint i = 0; i < curNodes.size(); ++i)
		{
			N* node = curNodes[i];

			if(!node->IsRoot())
				nextNodes.insert(node->GetParent());

			if(node->IsProcessed() && node->GetNumberOfChildren() == 0)
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
			else if(node->IsProcessed() && node->GetNumberOfChildren() == 1)
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


#endif	
