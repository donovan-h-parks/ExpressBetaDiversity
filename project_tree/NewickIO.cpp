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

#include "NewickIO.hpp"

#include "StringTools.hpp"
#include "NewickTokenizer.hpp"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

bool NewickIO::Read(Tree<NodePhylo>& tree, istream & in)
{	
	m_numElement = 0;
	m_processedElement = 0;

	// Checking the existence of specified input stream
	if(!in)
	{
		std::cerr << "NewickIO::Read: invalid input stream." << std::endl;
		return false;
	}	
	
	// We concatenate all line in file till we reach the ending semi colon:
	string temp, description;

	// Read entire input stream
	while (!in.eof())
	{
		getline(in, temp, '\n');  // Copy current line in temporary string
		string::size_type index = temp.find(";");
		if(index != string::npos)
		{
			description += temp.substr(0, index + 1);
			break;
		}
		else 
			description += temp;
	}

	description = util::StringTools::RemoveSubstrings(description, '[', ']');

	return ParseTree(tree, description);
}

bool NewickIO::Read(Tree<NodePhylo>& tree, const string& filename)
{
	// Set name of tree to the filename
	string file = util::StringTools::ReplaceChar(filename, '\\', '/');
	string::size_type slashPos = file.rfind('/');
	string::size_type dotPos = file.rfind('.');
	if(dotPos > slashPos)
		tree.SetName(file.substr(slashPos+1, dotPos-slashPos-1));
	else
		tree.SetName("");

	// Parse Newick file
	ifstream input(file.c_str(), std::ios::in);
	bool bLoaded = Read(tree, input);
	input.close();

	return bLoaded;
}

void NewickIO::Write(Tree<NodePhylo>& tree, ostream& out) const
{
	// Checking the existence of specified file, and possibility to open it in write mode
	if(out == NULL)
		std::cerr << "Newick::Write: invalid output stream." << std::endl;

	out << "(";

	NodePhylo* root = tree.GetRootNode();
	
  if(tree.GetNumberOfLeaves() == 0)
  {
    out << root->GetName();

		double dist = root->GetDistanceToParent();
		if(dist != NodePhylo::NO_DISTANCE)
			out << " " << dist;
  }
  else
  {
		WriteNodes(tree, out, root);
  }
	out << ")";
	
	// Output the name of the root if it has one
	if(!util::StringTools::IsEmpty(root->GetName()))
		out << root->GetName();
	
	out << ";" << endl;
}

void NewickIO::WriteNodes(Tree<NodePhylo>& tree, ostream& out, NodePhylo* parent) const
{
	vector<unsigned int> childrenId = parent->GetChildrenIds();
	if(childrenId.size() <= 0)
	{
		std::cerr << "NewickIO::Write: non-leaf node has no children." << std::endl;
	}

	WriteElements(tree, out, parent, parent->GetChild(0));
	for(uint i = 1; i < childrenId.size(); i++)
	{
		out << ",";
		WriteElements(tree, out, parent, parent->GetChild(i));
	}
}

void NewickIO::WriteElements(Tree<NodePhylo>& tree, ostream& out, NodePhylo* parent, NodePhylo* child) const
{	
  if(child->GetNumberOfChildren() != 0)
  {
    out << "(";
		WriteNodes(tree, out, child); 
    out << ")";
  }

	if(!util::StringTools::IsEmpty(child->GetName()))
		out << child->GetName();

	double dist = child->GetDistanceToParent();
	if(dist != NodePhylo::NO_DISTANCE)
		out << ":" << dist;
}

NewickIO::Element NewickIO::Elements(const string& description)
{
	Element element;
	element.node			= "";
	element.name			= "";
	element.length    = "";
	element.bootstrap = "";

	string desc = description;

	// Check if description is a node or leaf
	string::size_type lastP = desc.rfind(')');
	string::size_type firstP = desc.find('(');
	if(firstP != string::npos)
	{
		//This is a node:
		if(lastP <= firstP)
		{
			std::cerr << "NewickIO::Elements: invalid tree format." << std::endl;
		}

		element.node = desc.substr(firstP + 1, lastP - firstP - 1);
		desc = util::StringTools::RemoveSurroundingWhiteSpaces(desc.substr(lastP + 1));
	}
  
	// Check if this element has length
	string::size_type colon = desc.rfind(':');
	if(colon != string::npos)
	{
		element.length = util::StringTools::RemoveSurroundingWhiteSpaces(desc.substr(colon + 1));
		desc = util::StringTools::RemoveSurroundingWhiteSpaces(desc.substr(0, colon));
	}

	// Check for name and bootstrap value
	lastP = desc.rfind('\'');
	firstP = desc.find('\'');
	if(firstP != string::npos)
	{
		// Quoted name:
		element.name = desc.substr(firstP+1, lastP-firstP-1);
		element.name = util::StringTools::ReplaceChar(element.name, '_', ' ');
		element.bootstrap = util::StringTools::RemoveSurroundingWhiteSpaces(desc.substr(lastP+1));
	}
	else
	{
		// No quotations:
		firstP = desc.find(' ');
		if(firstP != string::npos)
		{
			// Parse the name and bootstrap value
			element.name = desc.substr(0, firstP-1);		
			element.bootstrap = util::StringTools::RemoveSurroundingWhiteSpaces(desc.substr(firstP+1));
		}
		else
		{
			// Assume we have a node name
			element.name = desc;
		}
	}
 
  return element;
}  

bool NewickIO::ParseNode(Tree<NodePhylo>& tree, const string& description, NodePhylo* parent)
{
	Element element = Elements(description);
	m_processedElement++;

	// New node:
	NodePhylo* node(new NodePhylo(m_processedElement));
	parent->AddChild(node);

	node->SetName("");
	if(!util::StringTools::IsEmpty(element.name))
	{
		node->SetName(element.name);
	}

	node->SetDistanceToParent(NodePhylo::NO_DISTANCE);
	if(!util::StringTools::IsEmpty(element.length))
	{
		node->SetDistanceToParent(util::StringTools::ToDouble(element.length));
	}

	if(!util::StringTools::IsEmpty(element.bootstrap))
	{
		// ignore
	}
  
	// Parse rest of tree
	NewickTokenizer nt(element.node);

	if(nt.NumTokens() != 1)
	{
		for(uint i = 0; i < nt.NumTokens(); i++)
		{
			if(!ParseNode(tree, nt.Token(i), node))
				return false;
		}
	}

	return true;
}

bool NewickIO::ParseTree(Tree<NodePhylo>& tree, const string& description)
{
	string::size_type lastP  = description.rfind(')');
	if(lastP == string::npos)
	{
		std::cerr << "NewickIO::ParseTree: no closing parenthesis found." << std::endl;
	}

	string::size_type firstP = description.find('(');
	if(lastP == string::npos)
	{
		std::cerr << "NewickIO::ParseTree: no opening parenthesis found." << std::endl;
	}

	string::size_type semi = description.rfind(';');
	if(lastP == string::npos)
	{
		std::cerr << "NewickIO::ParseTree: no semi-colon found." << std::endl;
	}

	if(lastP <= firstP)
	{
		std::cerr << "NewickIO::ParseTree: closing parenthesis before opening parenthesis." << std::endl;
	}


	string content = description.substr(firstP + 1, lastP - firstP - 1);
	string root_elements = description.substr(lastP + 1, semi - lastP - 1);

	// Quickly determine number of elements in the file.
	for(uint i = 0; i < content.size(); ++i)
	{
		if(content[i] == ',' || content[i] == ')')
			m_numElement++;
	}
	m_numElement++;

	// Track number of elements that have been processed
	m_processedElement = 0;

	// Create root node:
	NodePhylo* root(new NodePhylo(m_processedElement));
	tree.SetRootNode(root);
	root->SetDistanceToParent(NodePhylo::NO_DISTANCE);

	// Parse rest of tree
	NewickTokenizer nt(content);

	// The check for a parenthese is necessary as it is allowable to have
	// a tree rooted on a leaf in which case it should not be treated as a leaf.
	if(nt.NumTokens() == 1 && content.rfind("(") == string::npos)
	{    
		root->SetName(util::StringTools::RemoveSurroundingWhiteSpaces(nt.Token(0)));
	}
	else
	{
		// This is a node.
		Element elements = Elements(root_elements);
		root->SetName("");
		if(!util::StringTools::IsEmpty(elements.name))
		{
			root->SetName(elements.name);
		}

		for(uint i = 0; i < nt.NumTokens(); i++)
		{
			if(!ParseNode(tree, nt.Token(i), root))
				return false;
		}
	}

	return true;
}
