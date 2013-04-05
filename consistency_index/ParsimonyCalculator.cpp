//=======================================================================
// Author: Donovan Parks
//
// Copyright 2009 Donovan Parks
//
// This file is part of Chameleon.
//
// Chameleon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Chameleon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Chameleon. If not, see <http://www.gnu.org/licenses/>.
//=======================================================================

#include "ParsimonyCalculator.hpp"

#include <iterator>

using namespace std;

float ParsimonyCalculator::Consistency() const 
{ 
	if(m_parsimonyScore == 0)
		return 1.0f;

	return float(m_uniqueCharacters.size() - 1) / m_parsimonyScore; 
}

bool ParsimonyCalculator::IsMissingData(const std::string& character)
{
	if(character.empty() || character == "-" || character == "NA" || character == "N/A" || character == "X")
		return true;

	return false;
}

void ParsimonyCalculator::Run(Tree<Node>* tree, map<string, string>& attributeMap)
{
	// get unique characters within this field
	m_uniqueCharacters.clear();
	map<string, string>::const_iterator it;
	for(it = attributeMap.begin(); it != attributeMap.end(); ++it)
	{
		if(!IsMissingData(it->second))
			m_uniqueCharacters.insert(it->second);
	}

	vector<string> characters;
	std::copy(m_uniqueCharacters.begin(), m_uniqueCharacters.end(), std::back_inserter(characters));

	// initialize leaf nodes
	vector<Node*> leaves = tree->GetLeaves();
	for(uint i = 0; i < leaves.size(); ++i)
	{
		Node* leaf = leaves.at(i);

		ParsimonyData parsimonyData;
		parsimonyData.nodeScore = 0;

		string character = attributeMap[leaf->GetName()];
		if(!IsMissingData(character))
		{
			for(uint i = 0; i < characters.size(); ++i)
			{
				parsimonyData.characterScores[characters.at(i)].score = INT_MAX;
				parsimonyData.characterScores[characters.at(i)].bPresent = false;
			}

			parsimonyData.characterScores[character].score = 0;
			parsimonyData.characterScores[character].bPresent = true;
		}
		else
		{
			// missing data is the same as allowing any character state at a leaf node
			for(uint i = 0; i < characters.size(); ++i)
			{
				parsimonyData.characterScores[characters.at(i)].score = 0;
				parsimonyData.characterScores[characters.at(i)].bPresent = true;
			}
		}

		m_parsimonyData[leaf->GetId()] = parsimonyData;
	}

	// Propagate parsimony scores up tree
	CalculateUp(tree, characters);

	// Propagate most parsimonious character states down tree.
	CalculateDown(tree, characters);

	m_parsimonyScore = m_parsimonyData[tree->GetRootNode()->GetId()].GetNodeScore();
}

void ParsimonyCalculator::CalculateUp(Tree<Node>* tree, const vector<string>& characters)
{
	vector<Node*> postOrderNodes = tree->PostOrder();
	for(uint i = 0; i < postOrderNodes.size(); ++i)
	{
		Node* node = postOrderNodes.at(i);

		if(node->IsLeaf())
			continue;

		// calculate parsimony data for internal node
		ParsimonyData parsimonyData;
		parsimonyData.nodeScore = INT_MAX;

		for(uint i = 0; i < characters.size(); ++i)
		{
			string character = characters.at(i);
			parsimonyData.characterScores[character].score = 0;
			parsimonyData.characterScores[character].bPresent = false;

			vector<Node*> children = node->GetChildren();
			for(uint j = 0; j < children.size(); ++j)
			{
				Node* child = children.at(j);

				uint nodeScore = m_parsimonyData[child->GetId()].GetNodeScore() + 1;
				uint characterScore = m_parsimonyData[child->GetId()].GetScore(character);

				if(nodeScore < characterScore)
					parsimonyData.characterScores[character].score += nodeScore;
				else
					parsimonyData.characterScores[character].score += characterScore;

				if(m_parsimonyData[child->GetId()].characterScores[character].bPresent)
					parsimonyData.characterScores[character].bPresent = true;
			}

			if(parsimonyData.characterScores[character].score < parsimonyData.nodeScore)
				parsimonyData.nodeScore = parsimonyData.characterScores[character].score;
		}

		m_parsimonyData[node->GetId()] = parsimonyData;
	}
}

void ParsimonyCalculator::CalculateDown(Tree<Node>* tree, const vector<string>& characters)
{
	// assign parsimonious characters to root node
	ParsimonyData& data = m_parsimonyData[tree->GetRootNode()->GetId()];
	for(uint i = 0; i < characters.size(); ++i)
	{
		if(data.GetScore(characters.at(i)) == data.GetNodeScore())
			data.parsimoniousCharacters.insert(characters.at(i));
	}

	// assign parsimonious characters to internal nodes in a breadth-first manner
	queue<Node*> q;
	q.push(tree->GetRootNode());
	while(!q.empty())
	{
		Node* node = q.front();
		q.pop();

		// assign parsimonious characters to node
		uint numCharacters = 0;
		ParsimonyData& dataNode = m_parsimonyData[node->GetId()];

		for(uint i = 0; i < characters.size(); ++i)
		{	
			if(dataNode.GetNodeScore() == dataNode.GetScore(characters[i]))
				dataNode.parsimoniousCharacters.insert(characters[i]);

			if(dataNode.characterScores[characters[i]].bPresent)
				numCharacters++;
		}

		if(dataNode.GetNodeScore() > 0)
			dataNode.consistencyIndex = float(numCharacters-1) / dataNode.GetNodeScore();
		else
			dataNode.consistencyIndex = 1.0;
		
		node->SetParsimonyData(&dataNode);
		
		// process children
		std::vector<Node*> children = node->GetChildren();
		for(uint i = 0; i < children.size(); ++i)
			q.push(children[i]);
	}
}
