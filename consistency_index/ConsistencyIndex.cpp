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
		return;
	}
	std::string inputTreeFile = argv[1];
	std::string leafFeatureMapFile = argv[2];
	std::string outputFile = argv[3];

	// read input tree
	Tree<Node>* inputTree(new Tree<Node>);
	NewickIO newickIO;
	newickIO.Read(inputTree, inputTreeFile);

	// read attribute map		
	std::set<std::string> uniqueAttributes;
	std::map<std::string, std::string> attributeMap;

	std::ifstream fin(leafFeatureMapFile.c_str());
	while(!fin.eof())
	{
		std::string leafName;
		std::string attribute;
		fin >> leafName >> attribute;

		if(leafName.empty() || attribute.empty())
			break;

		attributeMap[leafName] = attribute;
		uniqueAttributes.insert(attribute);
	}
	fin.close();

	// calculate consistency index
	ParsimonyCalculator pc;
	uint parsimony = pc.Calculate(inputTree, attributeMap);

	// output consistency
	std::ofstream fout(outputFile.c_str());
	fout <<	float(uniqueAttributes.size() - 1) / parsimony;
	fout.close();
}