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

#include "Node.hpp"

#include "StringTools.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

Node::Node(const Node& node):
  m_id(node.GetId()), m_name(node.GetName()), m_parent(node.GetParent()),
		m_distanceToParent(node.GetDistanceToParent())
{
	m_children = node.GetChildren();
}

Node& Node::operator=(const Node& node)
{
  m_id = node.GetId();
  m_name = node.GetName();
  m_parent = node.GetParent();
	m_distanceToParent = node.GetDistanceToParent();
	m_children = node.GetChildren();

  return * this;
}

     
std::vector<unsigned int> Node::GetChildrenIds() const
{
	std::vector<unsigned int> childrenId(m_children.size());
	for(unsigned int i = 0; i < m_children.size(); i++) 
	{
		childrenId.at(i) = m_children.at(i)->GetId();
	}

	return childrenId;
}

Node* Node::GetChild(unsigned int pos) const
{
	return m_children.at(pos);
} 

void Node::AddChild(unsigned int pos, Node* node)
{
	m_children.insert(m_children.begin() + pos, node);
	node->SetParent(this);
}

void Node::AddChild(Node* node)
{
	m_children.push_back(node);
	node->SetParent(this);
}

void Node::RemoveChild(unsigned int pos)
{
	m_children.erase(m_children.begin() + pos);
}

void Node::RemoveChildren() 
{  
	while(m_children.size() != 0) 
		m_children.erase(m_children.begin()); 
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
void Node::Swap(unsigned int pos1, unsigned int pos2)
{
  Node* node1 = GetChild(pos1);
  Node* node2 = GetChild(pos2);

	// remove higher position item first!
	if(pos1 < pos2)
	{
		RemoveChild(pos2);
		RemoveChild(pos1);
	}
	else
	{
		RemoveChild(pos1);
		RemoveChild(pos2);
	}

  AddChild(pos1, node2);
  AddChild(pos2, node1);
}

    
vector<Node*> Node::GetNeighbors() const
{
	vector<Node*> neighbors;

	Node* parent = GetParent();

  for(unsigned int i = 0; i < parent->GetNumberOfChildren(); i++) 
	{
		if(parent->GetChild(i) != this)
			neighbors.push_back(parent->GetChild(i));
	}

  return neighbors;
}

unsigned int Node::GetChildPosition(Node* child) const
{
  for(unsigned int i = 0; i < m_children.size(); i++) 
	{
    if(m_children.at(i) == child) 
			return i;
  }

	return m_children.size();	// invalid position
}
