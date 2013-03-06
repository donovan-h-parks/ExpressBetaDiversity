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

#ifndef _NODE_H_
#define _NODE_H_

#include <vector>
#include <string>
#include <limits.h>

/**
 * @brief Nodes used in trees.
 */
class Node
{
public:
	enum NODE_FLAGS { NO_DISTANCE = INT_MIN };
    
public:  
	/**
	 * @brief Constructor.
	 * @param id Unique id identifying node.
	 */
	Node(unsigned int id) : m_id(id), m_name(""), m_parent(NULL), m_distanceToParent(NO_DISTANCE) {}

	/**
	 * @brief Constructor.
	 * @param id Unique id identifying node.
	 * @param name Name of node.
	 */
	Node(unsigned int id,  const std::string & name) : m_id(id), m_name(name), m_parent(NULL), m_distanceToParent(NO_DISTANCE) {}

	/** Destructor. */
	virtual ~Node() {}

	/**
   * @brief Copy constructor.
   * 
   * @param node The node to copy.
   */
  Node(const Node & node);

  /**
   * @brief Assignation operator.
   *
   * @param node the node to copy.
   * @return A reference toward this node.
   */
  Node & operator=(const Node & node);


public:          
	/** Get id associated with this node. */
	virtual unsigned int GetId() const  { return m_id; }
	/**
	 * @brief Set node's id.
	 * @param id New identity tag.
	 */
	virtual void SetId(unsigned int id) { m_id = id; }

	/** Get id of all children. */
	virtual std::vector<unsigned int> GetChildrenIds() const;
	  
	/** Get the name associated to this node. */
	virtual const std::string& GetName() const	{	return m_name; }
	    
	/**
	 * @brief Set name of node.
	 * @param name Name to give node.
	 */
	virtual void SetName(const std::string& name)	{	m_name = name; }

	/** Get the distance to the parent node.	 */     
	virtual double GetDistanceToParent() const { return m_distanceToParent; }
	    
	/**
	 * @brief Set the distance to the parent node.
	 * @param distance Distance to parent node.
	 */
	virtual void SetDistanceToParent(double distance) { m_distanceToParent = distance; }
	  
	/** Get parent of this node. */     
	virtual Node* GetParent() const  { return m_parent; }

	/** Get id of parent node. */
	virtual unsigned int GetParentId() const  { return m_parent->GetId(); }
	    
	/**
	 * @brief Set the parent node.
	 * @param node Parent node.
	 */
	virtual void SetParent(Node* parent) { m_parent = parent; }
	    
	/** Remove the parent node. */
	virtual Node* RemoveParent() { Node* p = m_parent; m_parent = NULL; return p; }
	    
	/** Indicate if this node is the root node.	 */
	virtual bool IsRoot() const { return m_parent == NULL; }

	/** Get number of child nodes. */	     
	virtual unsigned int GetNumberOfChildren() const { return m_children.size(); }
    
	/** 
	 * @brief Get specified child node. 
	 * @param pos Indicates child that should be returned.
	 * @return Child at specified index position.
	 */
	virtual Node* GetChild(unsigned int pos) const;
	    
	/** 
	 * @brief Add child node at specified index position. 
	 * @param pos Position to add child.
	 * @param node Child node to add.
	 */
	virtual void AddChild(unsigned int pos, Node* node);

	/** 
	 * @brief Add child node.
	 * @param node Child node to add.
	 */
	virtual void AddChild(Node* node);
    
	/** 
	 * @brief Remove child node.
	 * @param pos Specifies which child to remove.
	 */
	virtual void RemoveChild(unsigned int pos);

	/** 
	 * @brief Remove child node.
	 * @param node Node to be removed.
	 */
	virtual void RemoveChild(Node* node);

	/** Remove all child nodes. */
	virtual void RemoveChildren();
	    
	/**
	 * @brief Get position of specified child node.
	 * @param child Child node to determine position of.
	 * @return Position of child node.
	 */
	virtual unsigned int GetChildPosition(Node* child) const;

	/** Get all neighbouring nodes (i.e., nodes that are children of this nodes parent). */
	// Note: this function is purposely not virual. Derived classes must implement there own
	// version of this class if they wish to allow users to return a vector that is not of type Node*.
	std::vector<Node*> GetNeighbors() const;

	/** Get all child nodes. */
	// Note: this function is purposely not virual. Derived classes must implement there own
	// version of this class if they wish to allow users to return a vector that is not of type Node*.
	std::vector<Node*> GetChildren() const { return m_children; }
	    
	/** Check if node is a leaf node. */
	virtual bool IsLeaf() const  { return m_children.empty(); }

	/** 
	 * @brief Swap the position of the two indicated children.
	 * @brief pos1 Position of first child.
	 * @brief pos2 Position of second child.
	 */
	virtual void Swap(unsigned int pos1, unsigned int pos2);

protected:
	/** Unique id identifying node. */
  unsigned int m_id;

	/** Name of node. */
  std::string m_name;

	/** Children of node. */
  std::vector<Node*> m_children;

	/** Parent of node. */
	Node* m_parent;

	/** Distance to parent of this node. */
  double m_distanceToParent;  
};

#endif

