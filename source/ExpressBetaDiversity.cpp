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

#include "getopt_pp.hpp"

#include "DiversityCalculator.hpp"

#include "SeqCountIO.hpp"

#include "UnitTests.hpp"

bool ParseCommandLine(int argc, char* argv[], std::string& treeFile, std::string& seqCountFile, std::string& outputPrefix, 
											std::string& clusteringMethod, uint& jackknifeRep, uint& seqToDraw, bool& bSampleSize,
											std::string& calcStr, uint& maxDataVecs, bool& bWeighted, bool& bMRCA, bool& bStrictMRCA, bool& bCount, 
											bool& bAll, double& threshold, std::string& outputFile, bool& bVerbose)
{
	bool bShowHelp, bShowCalc, bUnitTests;
	std::string maxDataVecsStr;
	std::string thresholdStr;
	std::string jackknifeRepStr;
	std::string seqToDrawStr;
	GetOpt::GetOpt_pp opts(argc, argv);
	opts >> GetOpt::OptionPresent('h', "help", bShowHelp);
	opts >> GetOpt::OptionPresent('l', "list-calc", bShowCalc);
	opts >> GetOpt::OptionPresent('v', "verbose", bVerbose);
	opts >> GetOpt::OptionPresent('u', "unit-tests", bUnitTests);
	opts >> GetOpt::Option('t', "tree-file", treeFile);
	opts >> GetOpt::Option('s', "seq-count-file", seqCountFile);
	opts >> GetOpt::Option('p', "output-prefix", outputPrefix, "output");
	opts >> GetOpt::Option('g', "clustering", clusteringMethod, "UPGMA");
	opts >> GetOpt::Option('j', "jackknife", jackknifeRepStr, "0");
	opts >> GetOpt::Option('d', "seqs-to-draw", seqToDrawStr, "0");
	opts >> GetOpt::OptionPresent('z', "sample-size", bSampleSize);
	opts >> GetOpt::Option('c', "calculator", calcStr);
	opts >> GetOpt::Option('x', "max-data-vecs", maxDataVecsStr, "1000");
	opts >> GetOpt::OptionPresent('w', "weighted", bWeighted);
	opts >> GetOpt::OptionPresent('m', "mrca", bMRCA);
	opts >> GetOpt::OptionPresent('r', "strict-mrca", bStrictMRCA);
	opts >> GetOpt::OptionPresent('y', "count", bCount);
	opts >> GetOpt::OptionPresent('a', "all", bAll);
	opts >> GetOpt::Option('b', "threshold", thresholdStr, "0.8");
	opts >> GetOpt::Option('o', "output-file", outputFile, "clusters.txt");

	maxDataVecs = atoi(maxDataVecsStr.c_str());
	threshold = atof(thresholdStr.c_str());
	jackknifeRep = atoi(jackknifeRepStr.c_str());
	seqToDraw = atoi(seqToDrawStr.c_str());

	if(bShowHelp || argc <= 1) 
	{		
		std::cout << std::endl;
		std::cout << "Express Beta Diversity (EBD) v1.0.6 (October 5, 2014)" << std::endl;
		std::cout << "  by Donovan Parks (donovan.parks@gmail.com) and Rob Beiko (beiko@cs.dal.ca)" << std::endl;
		std::cout << std::endl;
		std::cout << " Usage: " << opts.app_name() << " -t <tree file> -s <seq file> -c <calculator>" << std::endl;
		std::cout << "  -h, --help           Produce help message." << std::endl;
		std::cout << "  -l, --list-calc      List all supported calculators." << std::endl;
		std::cout << "  -u, --unit-tests     Execute unit tests." << std::endl;
		std::cout << std::endl;
		std::cout << "  -t, --tree-file      Tree in Newick format (if phylogenetic beta-diversity is desired)." << std::endl;
		std::cout << "  -s, --seq-count-file Sequence count file." << std::endl;
		std::cout << "  -p, --output-prefix  Output prefix (default = output)." << std::endl;
		std::cout << std::endl;
		std::cout << "  -g, --clustering     Hierarchical clustering method: UPGMA, SingleLinkage, CompleteLinkage, NJ (default = UPGMA)." << std::endl;
		std::cout << std::endl;
		std::cout << "  -j, --jackknife      Number of jackknife replicates to perform (default = 0)." << std::endl;
		std::cout << "  -d, --seqs-to-draw   Number of sequence to draw for jackknife replicates." << std::endl;
		std::cout << "  -z, --sample-size    Print number of sequences in each sample." << std::endl;
		std::cout << std::endl;
		std::cout << "  -c, --calculator     Desired calculator (e.g., Bray-Curtis, Canberra)." << std::endl;
		std::cout << "  -w, --weighted       Indicates if sequence abundance data should be used." << std::endl;
		std::cout << "  -m, --mrca           Apply 'MRCA weightings' to each branch (experimental)." << std::endl;
		std::cout << "  -r, --strict-mrca    Restrict calculator to MRCA subtree." << std::endl;
		std::cout << "  -y, --count          Use count data as opposed to relative proportions." << std::endl;
		std::cout << std::endl;
		std::cout << "  -x, --max-data-vecs  Maximum number of profiles (data vectors) to have in memory at once (default = 1000)." << std::endl;
		std::cout << std::endl;
		std::cout << "  -a, --all            Apply all calculators and cluster calculators at the specified threshold." << std::endl;
		std::cout << "  -b, --threshold      Correlation threshold for clustering calculators (default = 0.8)." << std::endl;
		std::cout << "  -o, --output-file    Output file for cluster of calculators (default = clusters.txt)." << std::endl;
		std::cout << std::endl;
		std::cout << "  -v, --verbose        Provide additional information on program execution." << std::endl;
							
		return false;
	}
	else if(bShowCalc)
	{
		std::cout << std::endl;
		std::cout << "Supported calculators: " << std::endl;
		std::cout << "  Bray-Curtis (aka: Sorensen, Steinhaus, Odum, percentage difference, pairwise Whittaker)" << std::endl;
		std::cout << "  Canberra" << std::endl;
		std::cout << "  Chi-squared" << std::endl;
		std::cout << "  Coefficient of similarity (CS)" << std::endl;
		std::cout << "  Complete tree (CT)" << std::endl;
		std::cout << "  Euclidean" << std::endl;
		std::cout << "  Fst (aka: Pst)" << std::endl;
		std::cout << "  Gower" << std::endl;
		std::cout << "  Hellinger" << std::endl;
		std::cout << "  Kulczynski (aka: Kulczynski-Cody, Sokal-Sneath)" << std::endl;
		std::cout << "  Lennon compositional difference (Lennon)" << std::endl;		
		std::cout << "  Manhattan (aka: taxicab, city-block, unnormalized weighted UniFrac)" << std::endl;
		std::cout << "  Mean nearest neighbour distance (MNND)" << std::endl;
		std::cout << "  Mean phylogenetic distance (MPD)" << std::endl;
		std::cout << "  Morisita-Horn" << std::endl;
		std::cout << "  Normalized weighted UniFrac (NWU) (Identical to, but slower, than the Bray-Curtis calculator)" << std::endl;
		std::cout << "  Pearson (i.e., Pearson product-moment correlation coefficient)" << std::endl;		
		std::cout << "  Rao's Hp (RaoHp)" << std::endl;
		std::cout << "  Soergel (aka: Jaccard, unweighted UniFrac, Ruzicka, Marczewski-Steinhaus, percentage remoteness)" << std::endl;
		std::cout << "  Tamas coefficient (TC) (aka: simple matching coefficent)" << std::endl;
		std::cout << "  Weighted correlation (WC)" << std::endl;
		std::cout << "  Whittaker (aka: Whittaker index of association)" << std::endl;	
		std::cout << "  Yue-Clayton (aka: similarity ratio)" << std::endl;
		std::cout << std::endl;

		return false;
	}

	if(bUnitTests)
	{
		std::cout << std::endl;

		UnitTests unitTests;
		if(unitTests.Execute())
			std::cout << "Passed all unit tests." << std::endl;
		else
			std::cerr << "Failed unit test." << std::endl;

		return false;
	}

	if(seqCountFile.empty())
	{
		std::cout << std::endl;
		std::cout << "  [Error] The --seq-count-file (-s) flag must be specified." << std::endl;
		return false;
	}

	if(bSampleSize)
	{
		return true;
	}

	if(!seqCountFile.empty() && !bAll && calcStr.empty())
	{
		std::cout << std::endl;
		std::cout << "  [Error] Either the --calculator (-c) or --all (-a) flag must be specified along with a sequence and (optional) tree file." << std::endl;
		return false;
	}

	if(jackknifeRep != 0 && seqToDraw == 0)
	{
		std::cout << std::endl;
		std::cout << "  [Error] The --seqs-to-draw (-d) flag must be set along with the --jackknife (-j) flag." << std::endl;
		return false;
	}

	if(bMRCA && bStrictMRCA)
	{
		std::cout << std::endl;
		std::cout << "  [Error] The --mrca (-m) and --strict-mrca (-r) flags cannot be specified at the same time." << std::endl;
		return false;
	}

	if((bMRCA || bStrictMRCA) && treeFile.empty())
	{
		std::cout << std::endl;
		std::cout << "  [Error] A tree file must be specified when using the --mrca (-m) or --strict-mrca (-r) flag." << std::endl;
		return false;
	}

	if(bStrictMRCA && (calcStr == "Normalized weighted UniFrac" || calcStr == "NWU" || calcStr == "NormalizedWeightedUniFrac" || calcStr == "Normalized Weighted UniFrac"))
	{
		std::cout << std::endl;
		std::cout << "  [Error] The --strict-mrca (-r) flag cannot be used with the normalized weighted UniFrac calculator." << std::endl;
		std::cout << "  [Error] Use the Bray-Curtis calculator. It is equivalent to normalized weighted UniFrac." << std::endl;
		return false;
	}

	if(bMRCA && (calcStr == "Normalized weighted UniFrac" || calcStr == "NWU" || calcStr == "NormalizedWeightedUniFrac" || calcStr == "Normalized Weighted UniFrac"))
	{
		std::cout << std::endl;
		std::cout << "  [Error] The --mrca (-m) flag cannot be used with the normalized weighted UniFrac calculator." << std::endl;
		std::cout << "  [Error] Use the Bray-Curtis calculator. It is equivalent to normalized weighted UniFrac." << std::endl;
		return false;
	}

	if(calcStr == "Normalized weighted UniFrac" || calcStr == "NWU" || calcStr == "NormalizedWeightedUniFrac" || calcStr == "Normalized Weighted UniFrac")
	{
		std::cout << "  [Warning] The Normalized weighted UniFrac calculator is equivalent to the Bray-Curtis calculator, but more computationally expensive." << std::endl;
	}

	if(maxDataVecs % 2 != 0)
	{
		std::cout << std::endl;
		std::cout << "  [Error] The --max-data-vecs (-v) parameter must be a multiple of 2." << std::endl;
		return false;
	}

	if(calcStr == "Complete tree" || calcStr == "CompleteTree" || calcStr == "CT" 
		|| calcStr == "Mean nearest neighbour distance" || calcStr == "MNND" 
		|| calcStr == "Mean phylogenetic distance" || calcStr == "MPD" 
		|| calcStr == "Normalized weighted UniFrac" || calcStr == "NWU" 
		|| calcStr == "NormalizedWeightedUniFrac" || calcStr == "Normalized Weighted UniFrac")
	{
		std::cout << std::endl;
		std::cout << "  [Error] A tree file must be specified when using the specified calculator." << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	std::clock_t timeStart = std::clock();

	srand((uint)time(NULL));

	// parse command line arguments
	std::string treeFile;
	std::string seqCountFile;
	std::string outputPrefix;
	std::string clusteringMethod;
	std::string calcStr;
	uint jackknifeRep;
	uint seqToDraw;
	bool bSampleSize;
	uint maxDataVecs;
	bool bWeighted;
	bool bMRCA;
	bool bStrictMRCA;
	bool bCount;
	bool bVerbose;
	bool bAll;
	double threshold;
	std::string outputFile;
	if(!ParseCommandLine(argc, argv, treeFile, seqCountFile, outputPrefix, clusteringMethod, 
												jackknifeRep, seqToDraw, bSampleSize, 
												calcStr, maxDataVecs, bWeighted, bMRCA, bStrictMRCA, 
												bCount, bAll, threshold, outputFile, bVerbose))
	{
		return 0;
	}

	if(bAll)
	{
		DiversityCalculator calculator(seqCountFile, treeFile, "", maxDataVecs, false, false, bStrictMRCA, bCount, bVerbose);

		if(!calculator.IsGood())
			return -1;

		calculator.All(threshold, outputFile, clusteringMethod);
		return 0;
	}

	if(bSampleSize)
	{
		SeqCountIO sampleCountIO;
		sampleCountIO.Read(seqCountFile);

		std::string sampleWithMinSeqs;
		double minSeqs = std::numeric_limits<double>::max();
		
		std::cout << "Sample Id" << '\t' << "Number of Sequences" << std::endl;
		for(uint i = 0; i < sampleCountIO.GetNumSamples(); ++i)
		{
			std::vector<double> count;
			double totalNumSeqs;
			sampleCountIO.GetData(i, count, totalNumSeqs);

			std::cout << sampleCountIO.GetSampleName(i) << '\t' << totalNumSeqs << std::endl;

			if(totalNumSeqs < minSeqs)
			{
				minSeqs = totalNumSeqs;
				sampleWithMinSeqs = sampleCountIO.GetSampleName(i);
			}
		}

		std::cout << std::endl;
		std::cout << "Sample with minimum number of sequences is " << sampleWithMinSeqs << " with " << minSeqs << " sequences." << std::endl;

		return 0;
	}

	if(bVerbose)
		std::cout << "Express Beta Diversity:" << std::endl << std::endl;

	// set diversity calculator
	DiversityCalculator calculator(seqCountFile, treeFile, calcStr, maxDataVecs, bWeighted, bMRCA, bStrictMRCA, bCount, bVerbose);
	if(!calculator.IsGood())
		return -1;

	// compute dissimilarity between all pairs of samples
	if(!calculator.Dissimilarity(outputPrefix, clusteringMethod, jackknifeRep, seqToDraw))
		return -1;
	
	std::clock_t timeEnd = std::clock();

	if(bVerbose)
	{
		std::cout << "Total running time: " << ( timeEnd - timeStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << "Done." << std::endl;
	}

	return 0;
}

