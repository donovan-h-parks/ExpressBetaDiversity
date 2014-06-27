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

#include "NewickIO.hpp"

bool NewickIO::Read(Tree<Node>& tree, const std::string& filename)
{
	// Parse Newick file
	std::string newickFile = filename;
	std::replace(newickFile.begin(), newickFile.end(), '\\', '/');

	std::ifstream file(newickFile.c_str());
	if(!file.is_open())
	{
		std::cerr << "Unable to open Newick file: " << filename << std::endl;
		return false;
	}

	// Concatenate lines in file until we reach the end of the file
	std::string line, newickStr;
	while(file.good())
  {
		getline(file, line);
		newickStr += line;
	}

	file.close();

	// Parse tree and build tree structure
	return ParseNewickString(tree, newickStr);
}

void NewickIO::ParseNodeInfo(Node* node, std::string& nodeInfo)
{	
	std::string length;
	std::string name;

	// check if this element has length
	int colon = nodeInfo.rfind(':');
	if(colon != std::string::npos)
  {
    length = nodeInfo.substr(colon + 1);
		nodeInfo = nodeInfo.substr(0, colon);
  }

	// check for name and/or support value
	int lastP = nodeInfo.rfind('\'');
	int firstP = nodeInfo.find('\'');
	if(firstP != std::string::npos)
	{
		name = nodeInfo.substr(firstP+1, lastP-firstP-1);
	}
	else
	{
		int spacePos = nodeInfo.find(' ');
		if(spacePos != std::string::npos)
		{
			// parse the name, we do not need the support value
			name = nodeInfo.substr(0, spacePos-1);	
		}
		else
		{
			// assume the remaining description is either a name
			name = nodeInfo;
		}
	}	

	if(!name.empty())
		node->SetName(name);

	if(!length.empty())
	{
		node->SetDistanceToParent((double)::atof(length.c_str()));
	}
	else if(!node->IsRoot())
	{
		// assume branches with missing branch length information has a length of zero
		std::cerr << "[Warning] Branch found with no length information. Assuming branch has zero length." << std::endl;
		node->SetDistanceToParent(0.0);
	}
}

bool NewickIO::ParseNewickString(Tree<Node>& tree, const std::string& newickStr)
{
	// create root node
	Node* root = new Node("root");
	tree.SetRootNode(root);

	int lastP  = newickStr.rfind(')');
  int firstP = newickStr.find('(');
  int semi = newickStr.rfind(';');

	std::string content = newickStr.substr(firstP + 1, lastP - firstP);
	std::string rootElements = newickStr.substr(lastP + 1, semi - lastP - 1);
  
	ParseNodeInfo(root, rootElements);

	// parse newick string
	std::stack<Node*> nodeStack;
	nodeStack.push(root);
	std::string nodeInfo;
	Node* activeNode = NULL;	
	bool bInQuotes = false;
	uint numNodes = 1;

	for(uint i = 0; i < content.size(); ++i)
	{
		char ch = content[i];

		if(ch == '(')
		{
			// create a new internal node which will be the child 
			// of the node on the top of the stack
			Node* node(new Node());
			numNodes++;
			nodeStack.top()->AddChild(node);
			nodeStack.push(node);
		}
		else if(ch == ')' || ch == ',')
		{
			if(activeNode)
			{
				// if there is a currently active node, then we are
				// processing an internal node
				ParseNodeInfo(activeNode, nodeInfo);				
			}
			else
			{
				// if there is no currently active node, then we
				// must create a new leaf node
				Node* node(new Node());
				numNodes++;
				nodeStack.top()->AddChild(node);

				ParseNodeInfo(node, nodeInfo);		
			}

			activeNode = NULL;
			nodeInfo = "";

			// we are finished processing all children of the node on the top of the stack
			if(ch == ')')
			{
				activeNode = nodeStack.top();
				nodeStack.pop();
			}
		}
		else
		{
			// character is indicate the properties of a node
			if(ch == '"')
				bInQuotes = !bInQuotes;

			if(bInQuotes || !::isspace(ch))
				nodeInfo += ch;
		}
	}

	return true;
}

void NewickIO::Write(Tree<Node>& tree, std::ostream& out) const
{
	// Checking the existence of specified file, and possibility to open it in write mode
	assert(out != NULL);

	out << "(";

	Node* root = tree.GetRootNode();
	
  if(root->GetLeaves().size() == 0)
  {
    out << "'" << root->GetName().c_str() << "'";

		double dist = root->GetDistanceToParent();
		if(dist != Node::NO_DISTANCE)
			out << " " << dist;
  }
  else
  {
		WriteNodes(tree, out, root);
  }
	out << ")";
	
	// Output the name of the root if it has one
	if(!root->GetName().empty())
		out << "'" << root->GetName().c_str() << "'";
	
	out << ";" << std::endl;
}

void NewickIO::WriteNodes(Tree<Node>&tree, std::ostream& out, Node* parent) const
{
	WriteElements(tree, out, parent, parent->GetChild(0));
	for(uint i = 1; i < parent->GetNumberOfChildren(); i++)
	{
		out << ",";
		WriteElements(tree, out, parent, parent->GetChild(i));
	}
}

void NewickIO::WriteElements(Tree<Node>& tree, std::ostream& out, Node* parent, Node* child) const
{	
  if(child->GetNumberOfChildren() != 0)
  {
    out << "(";
		WriteNodes(tree, out, child); 
    out << ")";
  }

	if(!child->GetName().empty())
		out << "'" << child->GetName().c_str() << "'";

	if(child->GetJackknife() != Node::NO_DISTANCE)
		out << child->GetJackknife();
	
	if(child->GetDistanceToParent() != Node::NO_DISTANCE)
		out << ":" << child->GetDistanceToParent();
}
