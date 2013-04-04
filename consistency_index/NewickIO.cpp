//=======================================================================
// Copyright 2008, Dalhousie University
// Author: Donovan Parks
//
// The contents of this file are licensed under the 
// Attribution-ShareAlike Creative Commons License:
// http://creativecommons.org/licenses/by-sa/3.0/
//=======================================================================

#include "NewickIO.hpp"

#include "ErrorHandling.hpp"
#include "Log.hpp"
#include "StringTools.hpp"
#include "NewickTokenizer.hpp"

#include <vector>
#include <string>

using namespace std;

bool NewickIO::Read(Tree<Node>* tree, istream & in)
{	
	m_numElement = 0;
	m_processedElement = 0;

	// Checking the existence of specified input stream
	if(!in)
	{
		error::Log::Inst().Write("NewickIO::Read: invalid input stream.");
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
    else description += temp;
	}

	description = util::StringTools::RemoveSubstrings(description, '[', ']');

	return ParseTree(tree, description);
}

bool NewickIO::Read(Tree<Node>* tree, const string& filename)
{
	// Set name of tree to the filename
	string file = util::StringTools::ReplaceChar(filename, '\\', '/');
	string::size_type slashPos = file.rfind('/');
	string::size_type dotPos = file.rfind('.');
	if(dotPos > slashPos)
		tree->SetName(file.substr(slashPos+1, dotPos-slashPos-1));
	else
		tree->SetName("");

	// Parse Newick file
	ifstream input(file.c_str(), std::ios::in);
	bool bLoaded = Read(tree, input);
	input.close();

	return bLoaded;
}

void NewickIO::Write(Tree<Node>* tree, ostream& out) const
{
	// Checking the existence of specified file, and possibility to open it in write mode
	error::ErrorHandling::ThrowIO(out != NULL, "Newick::Write: invalid output stream.",__FILE__, __LINE__);

	out << "(";

	Node* root = tree->GetRootNode();
	
  if(tree->GetNumberOfLeaves() == 0)
  {
    out << root->GetName();

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
	if(!util::StringTools::IsEmpty(root->GetName()))
		out << root->GetName();
	
	out << ";" << endl;
}

void NewickIO::WriteNodes(Tree<Node>* tree, ostream& out, Node* parent) const
{
	vector<unsigned int> childrenId = parent->GetChildrenIds();
	error::ErrorHandling::ThrowIO(childrenId.size() > 0, "NewickIO::Write: non-leaf node has no children.",__FILE__, __LINE__);

	WriteElements(tree, out, parent, parent->GetChild(0));
	for(uint i = 1; i < childrenId.size(); i++)
	{
		out << ",";
		WriteElements(tree, out, parent, parent->GetChild(i));
	}
}

void NewickIO::WriteElements(Tree<Node>* tree, ostream& out, Node* parent, Node* child) const
{	
  if(child->GetNumberOfChildren() != 0)
  {
    out << "(";
		WriteNodes(tree, out, child); 
    out << ")";
  }

	if(!util::StringTools::IsEmpty(child->GetName()))
		out << child->GetName();

	if(child->GetDistanceToParent() != Node::NO_DISTANCE)
		out << ":" << child->GetDistanceToParent();
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
		error::ErrorHandling::ThrowIO(lastP > firstP, "NewickIO::Elements: invalid tree format.",__FILE__, __LINE__);

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
			// If the remaining description is a decimal number it is assumed it is a bootstrap
			// value, otherwise it is assumed it is a name
			if(util::StringTools::IsDecimalNumber(desc))
			{
				element.bootstrap = desc;
			}
			else
			{
				element.name = desc;
			}
		}
	}
 
  return element;
}  

bool NewickIO::ParseNode(Tree<Node>* tree, const string& description, Node* parent)
{
	Element element = Elements(description);
	m_processedElement++;

	// New node:
	Node* node(new Node(m_processedElement));
	parent->AddChild(node);

	node->SetName("");
	if(!util::StringTools::IsEmpty(element.name))
	{
		node->SetName(element.name);
	}

	node->SetDistanceToParent(Node::NO_DISTANCE);
	if(!util::StringTools::IsEmpty(element.length))
	{
		node->SetDistanceToParent(util::StringTools::ToDouble(element.length));
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

bool NewickIO::ParseTree(Tree<Node>* tree, const string& description)
{
  string::size_type lastP  = description.rfind(')');
	error::ErrorHandling::ThrowIO(lastP != string::npos, "NewickIO::ParseTree: no closing parenthesis found.",__FILE__, __LINE__);

  string::size_type firstP = description.find('(');
	error::ErrorHandling::ThrowIO(firstP != string::npos, "NewickIO::ParseTree: no opening parenthesis found.",__FILE__, __LINE__);

  string::size_type semi = description.rfind(';');
	error::ErrorHandling::ThrowIO(semi != string::npos, "NewickIO::ParseTree: no semi-colon found.",__FILE__, __LINE__);

	error::ErrorHandling::ThrowIO(lastP > firstP, 
																"NewickIO::ParseTree: closing parenthesis before opening parenthesis.",__FILE__, __LINE__);

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
	Node* root(new Node(m_processedElement));
	tree->SetRootNode(root);
	root->SetDistanceToParent(Node::NO_DISTANCE);

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
