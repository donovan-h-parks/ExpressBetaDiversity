#include "DataTypes.hpp"
#include "NewickIO.hpp"
#include "Node.hpp"
#include "Tree.hpp"
#include "TreeTools.hpp"
#include "StringTools.hpp"
#include "ParsimonyCalculator.hpp"

int main(int argc, char *argv[])
{
	if(argc < 4)
	{	
		std::cout << "ConsistencyIndex <input tree> <leaf feature map> <output file>" << std::endl;
		return 0;
	}

	std::string inputTreeFile = argv[1];
	std::string leafFeatureMapFile = argv[2];
	std::string outputFile = argv[3];

	// read input tree
	Tree<Node>* inputTree(new Tree<Node>);
	NewickIO newickIO;
	newickIO.Read(inputTree, inputTreeFile);

	// read attribute map		
	std::vector<std::set<std::string> > uniqueAttributes;
	std::vector<std::map<std::string, std::string> > attributeMaps;
	std::vector<std::string> attributes;

	std::ifstream fin(leafFeatureMapFile.c_str());
	std::string line;
	bool bHeader = true;
	while(std::getline(fin, line))
	{
		std::istringstream iss(line);
		std::string token;
		std::vector<std::string> tokens;
		while(std::getline(iss, token, '\t'))
			tokens.push_back(token);

		if(tokens.empty())
			break;

		if(bHeader)
		{
			bHeader = false;
			attributes.resize(tokens.size()-1);
			std::copy(tokens.begin()+1, tokens.end(), attributes.begin());
			continue;
		}

		if(uniqueAttributes.empty())
		{
			uniqueAttributes.resize(tokens.size()-1);
			attributeMaps.resize(tokens.size()-1);
		}

		for(uint i = 1; i < tokens.size(); ++i)
		{
			if(!tokens.at(i).empty() && tokens.at(i) != "-" && tokens.at(i) != "NA" && tokens.at(i) != "N/A" && tokens.at(i) != "X")
				uniqueAttributes.at(i-1).insert(tokens.at(i));

			attributeMaps.at(i-1)[tokens[0]] = tokens.at(i);
		}
	}
	fin.close();

	// calculate consistency index for each attribute
	std::ofstream fout(outputFile.c_str());
	for(uint i = 0; i < attributeMaps.size(); ++i)
	{
		ParsimonyCalculator pc;
		uint parsimony = pc.Calculate(inputTree, attributeMaps.at(i));

		// output consistency
		fout <<	attributes.at(i) << '\t' << float(uniqueAttributes.at(i).size() - 1) / parsimony << std::endl;
	}
	fout.close();

	return 0;
}