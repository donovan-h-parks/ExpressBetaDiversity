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

#ifndef _DIVERSITY_CALCULATOR_
#define _DIVERSITY_CALCULATOR_

#include "Precompiled.hpp"

#include "Tree.hpp"
#include "Node.hpp"
#include "NewickIO.hpp"
#include "SeqCountIO.hpp"
#include "DataVectorizer.hpp"
#include "LinearRegression.hpp"
#include "Cluster.hpp"

/**
 * @brief Measure beta-diversity with a variety of calculators.
 */
class DiversityCalculator
{
public:		
	/** Constructor. */
	DiversityCalculator(const std::string& seqCountFile, const std::string& treeFile, const std::string& calcStr, 
												uint maxProfiles, bool bWeighted, bool bMRCA, bool bStrictMRCA, bool bCount, bool bVerbose);

	/** Destructor. */
	~DiversityCalculator();

	/** Check good flag. */
	bool IsGood() const { return m_bGood; }

	/** Calculate dissimilarity between all pairs of samples. */
	bool Dissimilarity(const std::string& outputPrefix, const std::string& clusteringMethod, uint jackknifeRep = 0, uint seqsToDraw = 0);

	/** Apply all calculators. */
	bool All(double threshold, const std::string& outputFile, const std::string& clusteringMethod);

	/** Check if a measure is weighted. */
	static bool IsWeighted(const std::string& name);

	/** Check if a measure is unweighted. */
	static bool IsUnweighted(const std::string& name);

private:
	/** Read the sequence count file. */
	bool ReadSeqCountFile(const std::string& seqCountFile);

	/** Read tree file.*/
	bool ReadTreeFile(const std::string& treeFile);

	/** Set desired calculator. */
	bool SetCalculator(const std::string& calcStr);

	/** Initialize object for vectorizing data in difference manners. */
	bool InitDataVectorizer();

	/** Calculate data vectors . */
	void CalculateDataVectors(uint startIndex, uint numSamples, std::vector< std::vector<double> >& dataVec, uint seqsToDraw);

	/** Calculate minimum and maximum value in each column. */
	void CalculateColumnExtents();

	/** Calculate sum of each column in the data matrix. */
	void CalculateColumnSums();

	/** Calculate sum of each row in the data matrix. */
	void CalculateRowLeafSums(bool bSquared);

	/** Calculate branch length weighted sum for each row in the data matrix. */
	void CalculateWeightedRowSums();

	/** Get length or weight of each branch. */
	void GetBranchWeights();

	/** Calculate correlation between two dissimilarity matrices. */
	double CorrelationDissimilarity(const std::string& dissFile1, const std::string dissFile2);

	/** Initialize list of weighted calculators. */
	static void InitWeightedCalculators();

	/** Initialize list of unweighted calculators. */
	static void InitUnweightedCalculators();

	/** Read dissimilarity matrix. */
	bool ReadMatrix(const std::string& file, Matrix& dissMatrix, std::vector<std::string>& labels);

	/** Create dissimilarity matrix. */
	bool CreateDissimilarityMatrix(const std::string& dissFile, Tree<Node>* tree, const std::string& clusteringMethod, uint seqsToDraw = 0);

	/** Create jackknife tree.*/
	bool JackknifeTree(Tree<Node>* inputTree, const std::vector<Tree<Node>*>& jackknifeTrees);

	static double BrayCurtis(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Canberra(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double ChiSquared(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double CoefficientOfSimilarity(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double CompleteTree(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Euclidean(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Fst(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Gower(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Hellinger(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Kulczynski(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double LennonCD(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double LennonLRG(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Manhattan(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double MNND(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double MPD(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double MorisitaHorn(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double NormalizedWeightedUniFrac(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Pearson(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double RaoHp(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Soergel(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double SpeciesProfile(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double TamasCoefficient(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double WeightedCorrelation(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Whittaker(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double YueClayton(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);

	static double Unit(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Sum(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	static double Extents(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j);
	
private:
	typedef std::tr1::function<double (const std::vector<double>&, const std::vector<double>&, uint, uint)> CalculatorFunc;

	/** Function object indicating calculator to use. */
	CalculatorFunc m_calculator;

	/** Provides access to data in sequence count file. */
	SeqCountIO m_seqCountIO;

	/** Provides functionality for vectorize sample profiles in different ways. */
	static DataVectorizer m_dataVec;

	/** Flag indicating if all is good in the world. */
	bool m_bGood;

	/** Maximum number of data vectors to have in memory at once. */
	uint m_maxDataVecs;

	/** Flag indicating if weighted vectors are to be generated. */
	static bool m_bWeighted;

	/** Flag indicating if 'MRCA weightings' should be restricted to each branch. */
	bool m_bMRCA;

	/** Flag indicating if calculator should be restricted to the MRCA subtree. */
	bool m_bStrictMRCA;

	/** Flag indicating if count data should be used or if it should be normalized to relative proportions. */
	bool m_bCount;

	/** Flag indicating if phylogenetic vectors are to be generated. */
	bool m_bPhylogenetic;

	/** Flag indicating if program execution information should be printed. */
	bool m_bVerbose;

	/** Tree for phylogenetic beta-diversity calculations. */
	Tree<Node>* m_tree;

	/** Number of samples. */
	static uint m_numSamples;

	/** Total branch length in tree. */
	static double m_totalBranchLen;

	/** Branch length/weight associated with each column. */
	static std::vector<double> m_branchWeight;

	/** Data vectors for current rows in dissimilarity matrix being processed. */ 
	static std::vector< std::vector<double> > m_dataVecRows;

	/** Data vectors for current columns in dissimilarity matrix being processed. */ 
	static std::vector< std::vector<double> > m_dataVecCols;

	/** Minimum value in each column of data matrix. */
	static std::vector<double> m_minExtent;

	/** Maximum value in each column of data matrix. */
	static std::vector<double> m_maxExtent;

	/** Sum of each column in the data matrix. */
	static std::vector<double> m_colSum;

	/** Sum of leaf node proportions along each row of the data matrix. */
	static std::vector<double> m_rowLeafSum;

	/** Sum of squarded leaf node proportions along each row of the data matrix. */
	static std::vector<double> m_rowLeafSumSqrd;

	/** Sum of each row weighted by branch length in the data matrix. */
	static std::vector<double> m_weightedRowSum;

	/** List of all weighted measures. */
	static std::set<std::string> m_weightedCalculators;

	/** List of all unweighted measures. */
	static std::set<std::string> m_unweightedCalculators;
};

#endif

