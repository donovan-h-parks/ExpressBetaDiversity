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

		if(attributeMaps.empty())
			attributeMaps.resize(tokens.size()-1);

		for(uint i = 1; i < tokens.size(); ++i)
			attributeMaps.at(i-1)[tokens[0]] = tokens.at(i);
	}
	fin.close();

	// calculate consistency index for each attribute
	std::ofstream fout(outputFile.c_str());
	fout << "Attribute\tConsistency Score\tUnique States\tConsistency Index" << std::endl;
	for(uint i = 0; i < attributeMaps.size(); ++i)
	{
		ParsimonyCalculator pc;
		pc.Run(inputTree, attributeMaps.at(i));

		// output consistency
		fout <<	attributes.at(i) << '\t' << pc.Score() << '\t' << pc.NumUniqueCharacters() << '\t' << pc.Consistency() << std::endl;
	}
	fout.close();

	return 0;
}