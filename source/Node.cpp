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

#include "Node.hpp"

const double Node::NO_DISTANCE = -1;
const uint Node::NO_INDEX = std::numeric_limits<uint>::max();

Node::Node(const std::string& name)
	: m_name(name), m_parent(NULL), m_distanceToParent(NO_DISTANCE), m_seqIndex(NO_INDEX), m_postOrderIndex(NO_INDEX), m_jackknife(Node::NO_DISTANCE)
{

}

Node::Node(const Node &rhs)
{
	m_name = rhs.GetName();
	m_distanceToParent = rhs.GetDistanceToParent();
	m_seqIndex = rhs.GetSeqIndex();
	m_postOrderIndex = rhs.GetPostOrderIndex();
	m_jackknife = rhs.GetJackknife();
	
	m_parent = NULL;
	m_children.clear();
}

Node& Node::operator=(const Node &rhs)
{
	if(this != &rhs)
	{
		m_name = rhs.GetName();
		m_distanceToParent = rhs.GetDistanceToParent();
		m_seqIndex = rhs.GetSeqIndex();
		m_postOrderIndex = rhs.GetPostOrderIndex();
		m_jackknife = rhs.GetJackknife();
	
		m_parent = NULL;
		m_children.clear();
	}

  return *this;
}

std::vector<Node*> Node::GetLeaves()
{
	std::vector<Node*> leaves;
	GetLeaves(this, leaves);
	return leaves;
}

void Node::GetLeaves(Node* node, std::vector<Node*>& leaves)
{
	if(node->IsLeaf())
	{
		leaves.push_back(node);
	}
	for(unsigned int i = 0; i < node->GetNumberOfChildren(); i++)
	{
		GetLeaves(node->GetChild(i), leaves);
	}
}

void Node::RemoveChild(unsigned int pos)
{
	m_children.erase(m_children.begin() + pos);
}

void Node::RemoveChild(Node* node)
{
	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		if(m_children.at(i) == node)
		{
			m_children.erase(m_children.begin() + i);
			return;
		}
	}
}

std::vector<Node*> Node::GetNodes()
{
	std::vector<Node*> nodes;
	GetNodes(this, nodes);
	return nodes;
}

void Node::GetNodes(Node* node, std::vector<Node*>& nodes)
{
	for(unsigned int i = 0; i < node->GetNumberOfChildren(); i++)
	{
		GetNodes(node->GetChild(i), nodes);
	}

	nodes.push_back(node);
}