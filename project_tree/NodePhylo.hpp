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

#ifndef _NODE_PHYLO_H_
#define _NODE_PHYLO_H_

#include "Node.hpp"

#include <vector>
#include <string>
#include <limits.h>

/**
 * @brief Nodes used for phylogenetic trees.
 */
class NodePhylo : public Node
{  
public:  
	/**
	 * @brief Constructor.
	 * @param id Unique id identifying node.
	 */
	NodePhylo(int id): Node(id) {}

	/**
	 * @brief Constructor.
	 * @param id Unique id identifying node.
	 * @param name Name of node.
	 */
	NodePhylo(int id,  const std::string & name): Node(id, name) {}

	/** Destructor. */
	virtual ~NodePhylo() {}

	/**
   * @brief Copy constructor.
   * 
   * @param node The node to copy.
   */
  NodePhylo(const NodePhylo & node): Node(node)  {}

  /**
   * @brief Assignation operator.
   *
   * @param node the node to copy.
   * @return A reference toward this node.
   */
  NodePhylo & operator=(const NodePhylo & node)
	{
		Node::operator=(node);

		return * this;
	}

	/** 
	 * @brief Get specified child node. 
	 * @param pos Indicates child that should be returned.
	 * @return Child at specified index position.
	 */
	virtual NodePhylo* GetChild(unsigned int pos) const { return (NodePhylo*)Node::GetChild(pos); }

	/** Get parent of this node. */     
	virtual NodePhylo* GetParent() const  { return (NodePhylo*)Node::GetParent(); }

	/** Remove the parent node. */
	virtual NodePhylo* RemoveParent() { return (NodePhylo*)Node::RemoveParent(); }

	/** Get all neighbouring nodes (i.e., nodes that are children of this nodes parent). */
	std::vector<NodePhylo*> GetNeighbors() const
	{
		std::vector<NodePhylo*> neighbors;

		NodePhylo* parent = GetParent();

		for(unsigned int i = 0; i < parent->GetNumberOfChildren(); i++) 
		{
			if(parent->GetChild(i) != this)
				neighbors.push_back(parent->GetChild(i));
		}

		return neighbors;
	}

	/** Get all child nodes. */
	// Note: this function is purposely not virual. Derived classes must implement there own
	// version of this class if they wish to allow users to return a vector that is not of type NodeGeoTree*.
	std::vector<NodePhylo*> GetChildren() const
	{ 
		std::vector<NodePhylo*> children;

		for(unsigned int i = 0; i < GetNumberOfChildren(); ++i)
		{
			children.push_back((NodePhylo*)GetChild(i));
		}

		return children; 
	}

	/** Check if node has been processed. */
	bool IsProcessed() const { return m_bProcessed; }

	/**
	 * @brief Set processed flag.
	 * @param processed Set node to processed (true) or unprocessed (false).
	 */
	void SetProcessed(bool processed) { m_bProcessed = processed; }

protected:
	/** General purpose flag. */
	bool m_bProcessed;

};


#endif

