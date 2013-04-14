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

#ifndef _NODE_
#define _NODE_

#include "Precompiled.hpp"

class Node
{
public:
	static const double NO_DISTANCE;
	static const uint NO_INDEX;

public:
	Node(const std::string& name = "");
	Node(const Node &rhs);
	Node& operator=(const Node &rhs);
	  
	/** Get the name associated to this node. */
	std::string GetName() const	{	return m_name; }
	    
	/**
	 * @brief Set name of node.
	 * @param name Name to give node.
	 */
	void SetName(const std::string& name)	{	m_name = name; }

	/** Get the distance to the parent node.	 */     
	double GetDistanceToParent() const { return m_distanceToParent; }
	    
	/**
	 * @brief Set the distance to the parent node.
	 * @param distance Distance to parent node.
	 */
	void SetDistanceToParent(double distance) { m_distanceToParent = distance; }

	/** Get sequence index. */
	uint GetSeqIndex() const	{	return m_seqIndex; }
	    
	/** Set sequence index. */
	void SetSeqIndex(uint index)	{	m_seqIndex = index; }

	/** Get post-order traversal index. */
	uint GetPostOrderIndex() const	{	return m_postOrderIndex; }
	    
	/** Set post-order traversal index. */
	void SetPostOrderIndex(uint index)	{	m_postOrderIndex = index; }

	/** Get generic counter value. */
	uint GetCounter() const	{	return m_counter; }
	    
	/** Set generic counter value. */
	void SetCounter(uint value)	{	m_counter = value; }

	/** Increment generic counter value. */
	void IncCounter()	{	++m_counter; }

	/** Get generic value. */
	double GetValue() const	{	return m_value; }
	    
	/** Set generic value. */
	void SetValue(double value)	{	m_value = value; }

	/** Get parent of this node. */     
	Node* GetParent() const  { return m_parent; }
	    
	/**
	 * @brief Set the parent node.
	 * @param node Parent node.
	 */
	void SetParent(Node* parent) { m_parent = parent; }
	    
	/** Remove the parent node. */
	void RemoveParent() { m_parent = NULL; }
	    
	/** Indicate if this node is the root node.	 */
	bool IsRoot() const { return m_parent == NULL; }

	/** Get number of child nodes. */	     
	uint GetNumberOfChildren() const { return m_children.size(); }
    
	/** 
	 * @brief Get specified child node. 
	 * @param pos Indicates child that should be returned.
	 * @return Child at specified index position.
	 */
	Node* GetChild(uint pos) const { return m_children[pos]; }

	/** 
	 * @brief Add child node.
	 * @param node Child node to add.
	 */
	void AddChild(Node* node)
	{
		m_children.push_back(node);
		node->SetParent(this);
	}

	/** 
	 * @brief Remove child node.
	 * @param pos Specifies which child to remove.
	 */
	void RemoveChild(unsigned int pos);

	/** 
	 * @brief Remove child node.
	 * @param node Node to be removed.
	 */
	void RemoveChild(Node* node);

	/** Remove all child nodes. */
	void RemoveChildren() { m_children.clear(); }

	/** Get all child nodes. */
	const std::vector<Node*>& GetChildren() const { return m_children; }
	    
	/** Check if node is a leaf node. */
	bool IsLeaf() const  { return m_children.empty(); }

	/**
	 * @brief Retrieve all leaves from a subtree.
	 * @param node Node that defines the subtree.
	 * @return vector of leaves in the subtree.
	 */
	std::vector<Node*> GetLeaves();

	/**
	 * @brief Retrieve all son nodes from a subtree.
	 * @param node Node that defines the subtree.
	 * @return Vector of nodes in the subtree.
	 */
	std::vector<Node*> GetNodes();

	void SetJackknife(double value) { m_jackknife = value; }
	double GetJackknife() const { return m_jackknife; }

private:
	/**
	 * @brief Retrieve all leaves from a subtree.
	 * @param node Node that defines the subtree.
	 * @param leaves A vector of leaves in the subtree.
	 */
	void GetLeaves(Node* node, std::vector<Node*>& leaves);

	/**
	 * @brief Retrieve all son nodes from a subtree.
	 * @param node Node that defines the subtree.
	 * @param nodes Vector of pointers toward each son node in the subtree.
	 */
	void GetNodes(Node* node, std::vector<Node*>& nodes);

private:
	/** Name of node. */
	std::string m_name;

	/** Children of node. */
  std::vector<Node*> m_children;

	/** Parent of node. */
	Node* m_parent;

	/** Distance to parent of this node. */
  double m_distanceToParent; 

	/** Sequence data index. */
	uint m_seqIndex;

	/** Post-order traversal index. */
	uint m_postOrderIndex;

	/** Generic counter. */
	uint m_counter;

	/** Generic value. */
	double m_value;

	/** Jackknife value of node. */
	double m_jackknife;
};

#endif