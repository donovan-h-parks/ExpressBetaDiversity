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

#include "DiversityCalculator.hpp"

#include "NewickIO.hpp"

std::set<std::string> DiversityCalculator::m_weightedCalculators;
std::set<std::string> DiversityCalculator::m_unweightedCalculators;

DataVectorizer DiversityCalculator::m_dataVec;

uint DiversityCalculator::m_numSamples;
bool DiversityCalculator::m_bWeighted;
double DiversityCalculator::m_totalBranchLen;
std::vector< std::vector<double> > DiversityCalculator::m_dataVecRows;
std::vector< std::vector<double> > DiversityCalculator::m_dataVecCols;
std::vector<double> DiversityCalculator::m_minExtent;
std::vector<double> DiversityCalculator::m_maxExtent;
std::vector<double> DiversityCalculator::m_colSum;
std::vector<double> DiversityCalculator::m_rowLeafSum;
std::vector<double> DiversityCalculator::m_rowLeafSumSqrd;
std::vector<double> DiversityCalculator::m_weightedRowSum;
std::vector<double> DiversityCalculator::m_branchWeight;

DiversityCalculator::DiversityCalculator(const std::string& seqCountFile, const std::string& treeFile, 
																				 const std::string& calcStr, uint maxDataVecs, bool bWeighted, 
																				 bool bMRCA, bool bStrictMRCA, bool bCount, bool bVerbose)
	: m_maxDataVecs(maxDataVecs), m_bMRCA(bMRCA), m_bStrictMRCA(bStrictMRCA), 
		m_bCount(bCount), m_bVerbose(bVerbose), m_bPhylogenetic(false), m_bGood(true), m_tree(NULL)
{
	std::clock_t divCalcStart = std::clock();

	InitWeightedCalculators();
	InitUnweightedCalculators();

	m_bWeighted = bWeighted;

	if(!ReadSeqCountFile(seqCountFile))
		m_bGood = false;

	if(m_bGood && !ReadTreeFile(treeFile))
		m_bGood = false;

	if(calcStr != "")
	{
		if(m_bGood && !InitDataVectorizer())
			m_bGood = false;

		if(m_bGood && !SetCalculator(calcStr))
			m_bGood = false;
	}

	std::clock_t divCalcEnd = std::clock();

	if(m_bVerbose)
	{
		std::cout << "  Total time to initialize diversity calculator: " << ( divCalcEnd - divCalcStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}
}

DiversityCalculator::~DiversityCalculator()
{
	if(m_tree)
		delete m_tree;
}

bool DiversityCalculator::ReadSeqCountFile(const std::string& seqCountFile)
{
	std::clock_t startSeqCount = std::clock();
	if(!m_seqCountIO.Read(seqCountFile))
		return false;

	m_numSamples = m_seqCountIO.GetNumSamples();

	std::clock_t endSeqCount = std::clock();

	if(m_bVerbose)
	{
		std::cout << "  Sequences in sequnce count file: " << m_seqCountIO.GetNumSeqs() << std::endl; 
		std::cout << "  Samples in sequnce count file: " << m_seqCountIO.GetNumSamples() << std::endl;
		std::cout << "  Time to complete first pass through sequence count file: " << ( endSeqCount - startSeqCount ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}

	return true;
}

bool DiversityCalculator::ReadTreeFile(const std::string& treeFile)
{
	if(!treeFile.empty())
	{		
		std::clock_t readTreeStart = std::clock();

		m_bPhylogenetic = true;
		m_tree = new Tree<Node>();
		NewickIO newickIO;
		if(!newickIO.Read(*m_tree, treeFile))
			return false;

		std::clock_t readTreeEnd = std::clock();

		if(m_bVerbose)
		{
			std::cout << "  Calculating a phylogenetic beta-diversity measure." << std::endl;
			std::cout << "  Nodes in input tree: " << m_tree->GetNumNodes() << std::endl;
			std::cout << "  Leaves in input tree: " << m_tree->GetLeaves(m_tree->GetRootNode()).size() << std::endl;
			std::cout << "  Time to read tree: " << ( readTreeEnd - readTreeStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl;
			std::cout << std::endl;
		}
	}
	else
	{
		m_bPhylogenetic = false;
		if(m_bVerbose)
		{
			std::cout << "  Calculating an OTU-based beta-diversity measure." << std::endl;
			std::cout << std::endl;
		}
	}

	return true;
}

bool DiversityCalculator::SetCalculator(const std::string& calcStr)
{
	using namespace std::tr1::placeholders;

	bool bNeedColumnExtents = false;
	bool bNeedColumnSums = false;
	bool bNeedRowLeafSums = false;
	bool bNeedRowLeafSumsSqrd = false;
	bool bNeedWeightedRowSums = false;
	bool bNeedTotalBranchLen = false;

	std::string standardCalcStr;

	if(calcStr == "Bray-Curtis" || calcStr == "BC" || calcStr == "BrayCurtis")
	{
		standardCalcStr = "Bray-Curtis";
		m_calculator = std::tr1::bind(&DiversityCalculator::BrayCurtis, _1, _2, _3, _4);
	}
	else if(calcStr == "Canberra")
	{
		standardCalcStr = "Canberra";
		m_calculator = std::tr1::bind(&DiversityCalculator::Canberra, _1, _2, _3, _4);
	}
	else if(calcStr == "Chi-squared")
	{
		standardCalcStr = "Chi-squared";
		bNeedColumnSums = true;
		bNeedRowLeafSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::ChiSquared, _1, _2, _3, _4);
	}
	else if(calcStr == "Coefficient of similarity" || calcStr == "CS" || calcStr == "CoefficientOfSimilarity")
	{
		standardCalcStr = "Coefficient of similarity";
		m_calculator = std::tr1::bind(&DiversityCalculator::CoefficientOfSimilarity, _1, _2, _3, _4);
	}
	else if(calcStr == "Complete tree" || calcStr == "CT" || calcStr == "CompleteTree" || calcStr == "Complete Tree")
	{
		standardCalcStr = "Complete tree";
		bNeedColumnExtents = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::CompleteTree, _1, _2, _3, _4);
	}
	else if(calcStr == "Euclidean")
	{
		standardCalcStr = "Euclidean";
		m_calculator = std::tr1::bind(&DiversityCalculator::Euclidean, _1, _2, _3, _4);
	}
	else if(calcStr == "Fst")
	{
		standardCalcStr = "Fst";
		m_calculator = std::tr1::bind(&DiversityCalculator::Fst, _1, _2, _3, _4);
	}
	else if(calcStr == "Gower")
	{
		standardCalcStr = "Gower";
		bNeedColumnExtents = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::Gower, _1, _2, _3, _4);
	}
	else if(calcStr == "Hellinger")
	{
		standardCalcStr = "Hellinger";
		bNeedRowLeafSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::Hellinger, _1, _2, _3, _4);
	}
	else if(calcStr == "Kulczynski")
	{
		standardCalcStr = "Kulczynski";
		bNeedWeightedRowSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::Kulczynski, _1, _2, _3, _4);
	}
	else if(calcStr == "Lennon compositional difference" || calcStr == "Lennon" || calcStr == "LCD")
	{
		standardCalcStr = "Lennon compositional difference";
		m_calculator = std::tr1::bind(&DiversityCalculator::LennonCD, _1, _2, _3, _4);
	}
	else if(calcStr == "Lennon local richness gradient" || calcStr == "LLRG")
	{
		standardCalcStr = "Lennon local richness gradient";
		m_calculator = std::tr1::bind(&DiversityCalculator::LennonLRG, _1, _2, _3, _4);
	}
	else if(calcStr == "Manhattan")
	{
		standardCalcStr = "Manhattan";
		m_calculator = std::tr1::bind(&DiversityCalculator::Manhattan, _1, _2, _3, _4);
	}
	else if(calcStr == "Mean nearest neighbour distance"|| calcStr == "MNND")
	{
		standardCalcStr = "MNND";
		m_calculator = std::tr1::bind(&DiversityCalculator::MNND, _1, _2, _3, _4);
	}
	else if(calcStr == "Mean phylogenetic distance"|| calcStr == "MPD")
	{
		standardCalcStr = "MPD";
		m_calculator = std::tr1::bind(&DiversityCalculator::MPD, _1, _2, _3, _4);
	}
	else if(calcStr == "Morisita-Horn"|| calcStr == "MH" || calcStr == "MorisitaHorn")
	{
		standardCalcStr = "Morisita-Horn";
		bNeedWeightedRowSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::MorisitaHorn, _1, _2, _3, _4);
	}
	else if(calcStr == "Normalized weighted UniFrac" || calcStr == "NWU" || calcStr == "NormalizedWeightedUniFrac" || calcStr == "Normalized Weighted UniFrac")
	{
		standardCalcStr = "Normalized Weighted UniFrac";
		m_calculator = std::tr1::bind(&DiversityCalculator::NormalizedWeightedUniFrac, _1, _2, _3, _4);
	}
	else if(calcStr == "Pearson")
	{
		standardCalcStr = "Pearson";
		bNeedWeightedRowSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::Pearson, _1, _2, _3, _4);
	}
	else if(calcStr == "Rao's Hp" || calcStr == "RHp" || calcStr == "RaoHp" || calcStr == "RD")
	{
		standardCalcStr = "Rao's Hp";
		m_calculator = std::tr1::bind(&DiversityCalculator::RaoHp, _1, _2, _3, _4);
	}
	else if(calcStr == "Soergel" || calcStr == "Ruzicka")
	{
		standardCalcStr = "Soergel";
		m_calculator = std::tr1::bind(&DiversityCalculator::Soergel, _1, _2, _3, _4);
	}
	else if(calcStr == "Species profile" || calcStr == "SP" || calcStr == "SpeciesProfile")
	{
		standardCalcStr = "Species profile";
		bNeedRowLeafSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::SpeciesProfile, _1, _2, _3, _4);
	}
	else if(calcStr == "Tamas coefficient" || calcStr == "TC" || calcStr == "TamasCoefficient")
	{
		standardCalcStr = "Tamas coefficient";
		bNeedColumnExtents = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::TamasCoefficient, _1, _2, _3, _4);
	}
	else if(calcStr == "Weighted correlation" || calcStr == "WC" || calcStr == "WeightedCorrelation")
	{
		standardCalcStr = "Weighted correlation";
		bNeedTotalBranchLen = true;
		bNeedWeightedRowSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::WeightedCorrelation, _1, _2, _3, _4);
	}
	else if(calcStr == "Whittaker")
	{
		standardCalcStr = "Whittaker";
		bNeedRowLeafSums = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::Whittaker, _1, _2, _3, _4);
	}
	else if(calcStr == "Yue-Clayton" || calcStr == "YC" || calcStr == "YueClayton")
	{
		standardCalcStr = "Yue-Clayton";
		m_calculator = std::tr1::bind(&DiversityCalculator::YueClayton, _1, _2, _3, _4);
	}
	else if(calcStr == "Unit")
	{
		standardCalcStr = "SPECIAL";
		m_calculator = std::tr1::bind(&DiversityCalculator::Unit, _1, _2, _3, _4);
	}
	else if(calcStr == "Sum")
	{
		standardCalcStr = "SPECIAL";
		m_calculator = std::tr1::bind(&DiversityCalculator::Sum, _1, _2, _3, _4);
	}
	else if(calcStr == "Extents")
	{
		standardCalcStr = "SPECIAL";
		bNeedColumnExtents = true;
		m_calculator = std::tr1::bind(&DiversityCalculator::Extents, _1, _2, _3, _4);
	}
	else
	{
		std::cerr << "Unknown calculator specified: " << calcStr << std::endl;
		return false;
	}

	// check that a valid unweighted or weighted calculator has been requested
	if(m_bWeighted && !IsWeighted(standardCalcStr) && standardCalcStr != "Normalized Weighted UniFrac" && standardCalcStr != "SPECIAL")
	{
		std::cout << std::endl;
		std::cout << "  [Error] There is no weighted (quantitative) variant of the '" << calcStr << "' calculator." << std::endl;
		return false;
	}

	if(!m_bWeighted && !IsUnweighted(standardCalcStr) && standardCalcStr != "SPECIAL")
	{
		std::cout << std::endl;
		std::cout << "  [Error] There is no unweighted (qualitative) variant of the '" << calcStr << "' calculator." << std::endl;
		return false;
	}
	

	// required to calculate intermediate terms
	GetBranchWeights();

	if(bNeedColumnExtents)
		CalculateColumnExtents();

	if(bNeedColumnSums)
		CalculateColumnSums();

	if(bNeedRowLeafSums)
		CalculateRowLeafSums(false);

	if(bNeedRowLeafSumsSqrd)
		CalculateRowLeafSums(true);

	if(bNeedWeightedRowSums)
		CalculateWeightedRowSums();

	if(bNeedTotalBranchLen)
	{
		m_totalBranchLen = 0;
		for(uint n = 0; n < m_branchWeight.size(); ++n)
			m_totalBranchLen += m_branchWeight[n];
	}

	return true;
}

void DiversityCalculator::CalculateDataVectors(uint startIndex, uint numSamples, std::vector< std::vector<double> >& dataVec)
{
	std::clock_t startDataVecs = std::clock();
	
	// calculate data vector for each sample
	dataVec.clear();
	dataVec.reserve(numSamples);
	for(uint i = startIndex; i < std::min<uint>(m_seqCountIO.GetNumSamples(), startIndex+numSamples); ++i)
	{		
		std::vector<double> count;
		double totalNumSeq;
		m_seqCountIO.GetData(i, count, totalNumSeq);

		std::vector<double> prop;
		m_dataVec.CalculateDataVector(count, false, totalNumSeq, prop);
		dataVec.push_back(prop);
	}

	std::clock_t endDataVecs = std::clock();
}

void DiversityCalculator::CalculateColumnExtents()
{
	std::clock_t extentsStart = std::clock();

	m_minExtent.clear();
	m_maxExtent.clear();
	m_minExtent.resize(m_dataVec.GetSize(), std::numeric_limits<double>::max());
	m_maxExtent.resize(m_dataVec.GetSize(), 0);

	for(uint i = 0; i < m_seqCountIO.GetNumSamples(); ++i)
	{
		std::vector<double> count;
		double totalNumSeq;
		m_seqCountIO.GetData(i, count, totalNumSeq);

		std::vector<double> prop;
		m_dataVec.CalculateDataVector(count, false, totalNumSeq, prop);
	
		for(uint j = 0; j < prop.size(); ++j)
		{
			if(prop[j] < m_minExtent[j])
				m_minExtent[j] = prop[j];

			if(prop[j] > m_maxExtent[j])
				m_maxExtent[j] = prop[j];
		}
	}

	std::clock_t extentsEnd = std::clock();

	if(m_bVerbose)
	{
		std::cout << "  Time to calculate column extents: " << ( extentsEnd - extentsStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}
}

void DiversityCalculator::CalculateColumnSums()
{
	std::clock_t colSumStart = std::clock();

	m_colSum.clear();
	m_colSum.resize(m_dataVec.GetSize(), 0);

	for(uint i = 0; i < m_seqCountIO.GetNumSamples(); ++i)
	{
		std::vector<double> count;
		double totalNumSeq;
		m_seqCountIO.GetData(i, count, totalNumSeq);

		std::vector<double> prop;
		m_dataVec.CalculateDataVector(count, false, totalNumSeq, prop);

		for(uint j = 0; j < prop.size(); ++j)
			m_colSum[j] += prop[j];
	}

	std::clock_t colSumEnd = std::clock();

	if(m_bVerbose)
	{
		std::cout << "  Time to calculate column sums: " << ( colSumEnd - colSumStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}
}

void DiversityCalculator::CalculateRowLeafSums(bool bSquared)
{
	std::clock_t rowSumStart = std::clock();

	if(!bSquared)
	{
		m_rowLeafSum.clear();
		m_rowLeafSum.resize(m_seqCountIO.GetNumSamples(), 0);
	}
	else
	{
		m_rowLeafSumSqrd.clear();
		m_rowLeafSumSqrd.resize(m_seqCountIO.GetNumSamples(), 0);
	}

	for(uint i = 0; i < m_seqCountIO.GetNumSamples(); ++i)
	{
		std::vector<double> count;
		double totalNumSeq;
		m_seqCountIO.GetData(i, count, totalNumSeq);

		std::vector<double> leafProp;
		m_dataVec.CalculateDataVector(count, true, totalNumSeq, leafProp);

		for(uint j = 0; j < leafProp.size(); ++j)
		{
			if(!bSquared)
				m_rowLeafSum[i] += leafProp[j];
			else
				m_rowLeafSumSqrd[i] += leafProp[j]*leafProp[j];
		}
	}

	std::clock_t rowSumEnd = std::clock();

	if(m_bVerbose)
	{
		std::cout << "  Time to calculate row sums: " << ( rowSumEnd - rowSumStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}
}

void DiversityCalculator::CalculateWeightedRowSums()
{
	std::clock_t weightedRowSumStart = std::clock();

	m_weightedRowSum.clear();
	m_weightedRowSum.resize(m_seqCountIO.GetNumSamples(), 0);

	for(uint i = 0; i < m_seqCountIO.GetNumSamples(); ++i)
	{
		std::vector<double> count;
		double totalNumSeq;
		m_seqCountIO.GetData(i, count, totalNumSeq);

		std::vector<double> prop;
		m_dataVec.CalculateDataVector(count, false, totalNumSeq, prop);

		for(uint j = 0; j < prop.size(); ++j)
			m_weightedRowSum[i] += m_branchWeight[j] * prop[j];
	}

	std::clock_t weightedRowSumEnd = std::clock();

	if(m_bVerbose)
	{
		std::cout << "  Time to calculate weighted row sums: " << ( weightedRowSumEnd - weightedRowSumStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}
}

void DiversityCalculator::GetBranchWeights()
{
	if(m_bPhylogenetic)
	{
		std::vector<Node*> postOrder = m_tree->PostOrder(m_tree->GetRootNode());

		m_branchWeight.clear();
		m_branchWeight.reserve(postOrder.size());

		std::vector<Node*>::const_iterator it;
		for(it = postOrder.begin(); it != postOrder.end(); ++it)
		{
			Node* curNode = *it;
			m_branchWeight.push_back(curNode->GetDistanceToParent());
		}
	}
	else
	{
		// create a star tree
		m_branchWeight.clear();
		m_branchWeight.resize(m_seqCountIO.GetNumSeqs(), 1);
	}
}

bool DiversityCalculator::InitDataVectorizer()
{
	// initialize object for creating vectorial representations of sequence data
	std::clock_t dataVecStart = std::clock();
	if(!m_dataVec.Init(m_tree, m_bPhylogenetic, m_bWeighted, !m_bCount, m_seqCountIO.GetSeqs()))
		return false;
	std::clock_t dataVecEnd = std::clock();

	if(m_bVerbose)
		std::cout << "  Time to initialize data vectoring object: " << ( dataVecEnd - dataVecStart ) / (double)CLOCKS_PER_SEC << " s" << std::endl; 

	return true;
}

bool DiversityCalculator::Dissimilarity(const std::string& dissFile)
{
	std::clock_t dissStart = std::clock();	

	// open dissimilarity file
	std::ofstream dissOut(dissFile.c_str());
	if(!dissOut.is_open())
	{
		std::cerr << "Unable to open dissimilarity matrix file: " << dissFile << std::endl;
		return false;
	}

	// get blocking information
	uint blockLen = m_maxDataVecs / 2;
	uint numBlocks = m_seqCountIO.GetNumSamples() / blockLen;
	if(numBlocks*blockLen != m_seqCountIO.GetNumSamples())
		++numBlocks;	// extra block if samples do not fit perfectly into blocks

	// calculate dissimilarity
	dissOut << m_seqCountIO.GetNumSamples() << std::endl;

	double* partialDissMatrix = new double[blockLen*m_seqCountIO.GetNumSamples()];

	double innerLoopTime = 0;
	for(uint row = 0; row < numBlocks; ++row)
	{
		CalculateDataVectors(row*blockLen, blockLen, m_dataVecRows);

		for(uint col = 0; col <= row; ++col)
		{
			CalculateDataVectors(col*blockLen, blockLen, m_dataVecCols);

			std::clock_t innerDissLoopStart = std::clock();	
			for(uint r = 0; r < m_dataVecRows.size(); ++r)
			{
				uint colStop = m_dataVecCols.size();
				if(row == 0)
					colStop = std::min<uint>(r, m_dataVecCols.size());

				for(uint c = 0; c < colStop; ++c)
				{
					double diss;
					if(m_bMRCA)
					{
						m_dataVec.ApplyWeightsMRCA(m_dataVecRows[r], m_dataVecCols[c], m_branchWeight);

						// Check if all MRCA weighted branches are zero. This is a degenerate case and
						// indicates both samples are contained in a single leaf node.
						double branchSum =std::accumulate(m_branchWeight.begin(), m_branchWeight.end(), 0.0);
						if(branchSum == 0)
							diss = 0;
						else
							diss = m_calculator(m_dataVecRows[r], m_dataVecCols[c], r, c);
					}
					else if(m_bStrictMRCA)
					{
						std::vector<double> MRCAi;
						std::vector<double> MRCAj;
						m_dataVec.RestrictToMRCA(m_dataVecRows[r], m_dataVecCols[c], MRCAi, MRCAj, m_branchWeight);
						diss = m_calculator(MRCAi, MRCAj, r, c);
					}
					else
						diss = m_calculator(m_dataVecRows[r], m_dataVecCols[c], r, c);

					partialDissMatrix[r*m_seqCountIO.GetNumSamples() + col*blockLen + c] = diss;
				}
			}

			std::clock_t innerDissLoopEnd = std::clock();	
			innerLoopTime += (innerDissLoopEnd - innerDissLoopStart);
		}

		// write out partial dissimilarity matrix to file
		for(uint r = 0; r < m_dataVecRows.size(); ++r)
		{
			dissOut << m_seqCountIO.GetSampleName(row*blockLen + r);

			for(uint c = 0; c < (row*blockLen + r); ++c)
				dissOut << '\t' << partialDissMatrix[r*m_seqCountIO.GetNumSamples() + c];

			dissOut << std::endl;
		}
	}

	dissOut.close();

	delete[] partialDissMatrix;

	std::clock_t dissEnd = std::clock();

	if(m_bVerbose)
	{
		std::cout << std::endl;
		std::cout << "  Total time to calculate inner loop of dissimilarity matrix: " << innerLoopTime / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << "  Total time to calculate dissimilarity matrix: " << (dissEnd - dissStart) / (double)CLOCKS_PER_SEC << " s" << std::endl; 
		std::cout << std::endl;
	}

	return true;
}

double DiversityCalculator::BrayCurtis(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double num = 0;
	double den = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		num += fabs(com1[n] - com2[n])*m_branchWeight[n];
		den += (com1[n] + com2[n])*m_branchWeight[n];
	}

	return num / den;
}

double DiversityCalculator::Canberra(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double den = com1[n] + com2[n];
		if(den != 0)
			diss += (fabs(com1[n] - com2[n]) / den)*m_branchWeight[n];
	}

	return diss;
}

double DiversityCalculator::ChiSquared(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		if(m_colSum[n] > 0)
		{
			double d = (com1[n]/m_rowLeafSum[i] - com2[n]/m_rowLeafSum[j]);
			diss += (m_branchWeight[n]*d*d / m_colSum[n]);
		}
	}

	return sqrt(diss);
}

double DiversityCalculator::CoefficientOfSimilarity(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double max = std::max<double>(com1[n],com2[n]);
		if(max > 0)
			diss += (fabs(com1[n]-com2[n])/max)*m_branchWeight[n];
	}

	return diss;
}

double DiversityCalculator::CompleteTree(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double num = 0;
	double den = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		num += fabs(com1[n] - com2[n])*m_branchWeight[n];
		den += (m_maxExtent[n] - m_minExtent[n])*m_branchWeight[n];
	}

	if(den == 0)
		return 1;

	return num / den;
}

double DiversityCalculator::Euclidean(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double d = com1[n] - com2[n];
		diss += m_branchWeight[n]*d*d;
	}

	return sqrt(diss);
}

double DiversityCalculator::Fst(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	std::vector<double> leafPropI;
	std::vector<double> leafPropJ;
	std::vector< std::vector<double> > pairedLeafDistances;
	m_dataVec.PairedLeafSetDistance(com1, com2, leafPropI, leafPropJ, pairedLeafDistances);

	double DT = 0;
	double DS_A = 0;
	double DS_B = 0;

	if(m_bWeighted)
	{
		for(uint x = 0; x < leafPropI.size(); ++x)
		{
			for(uint y = 0; y < leafPropI.size(); ++y)
			{
				DT += 0.5*(leafPropI[x] + leafPropJ[x]) * 0.5*(leafPropI[y] + leafPropJ[y]) * pairedLeafDistances[x][y];
				DS_A += leafPropI[x]*leafPropI[y]*pairedLeafDistances[x][y];
				DS_B += leafPropJ[x]*leafPropJ[y]*pairedLeafDistances[x][y];
			}
		}
	}
	else
	{
		uint compA = 0;
		uint compB = 0;
		uint compAB = 0;
		for(uint x = 0; x < leafPropI.size(); ++x)
		{
			for(uint y = 0; y < leafPropI.size(); ++y)
			{
				if(x == y)
					continue;

				DT += pairedLeafDistances[x][y];
				compAB++;

				if(leafPropI[x] == 1 && leafPropI[y] == 1)
				{
					DS_A += pairedLeafDistances[x][y];
					compA++;
				}

				if(leafPropJ[x] == 1 && leafPropJ[y] == 1)
				{
					DS_B += pairedLeafDistances[x][y];
					compB++;
				}
			}
		}

		DT /= compAB;
		if(compA != 0)
			DS_A /= compA;
		else
			DS_A = 0;

		if(compB != 0)
			DS_B /= compB;
		else
			DS_B = 0;
	}	

	double DS = 0.5*(DS_A + DS_B);

	return (DT - DS) / DT;
}

double DiversityCalculator::Gower(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double d = m_maxExtent[n] - m_minExtent[n];
		if(d > 0)
			diss += (fabs(com1[n] - com2[n]) / (m_maxExtent[n] - m_minExtent[n]))*m_branchWeight[n];
	}

	return diss;
}

double DiversityCalculator::Hellinger(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double d = sqrt(com1[n]/m_rowLeafSum[i]) - sqrt(com2[n]/m_rowLeafSum[j]);
		diss += m_branchWeight[n]*d*d;
	}

	return sqrt(diss);
}

double DiversityCalculator::Kulczynski(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double sumMin = 0;
	for(uint n = 0; n < com1.size(); ++n)
		sumMin += std::min<double>(com1[n], com2[n])*m_branchWeight[n];

	return 1 - 0.5*(sumMin/m_weightedRowSum[i] + sumMin/m_weightedRowSum[j]);
}

double DiversityCalculator::LennonCD(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double A = 0;
	double B = 0;
	double C = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		A += std::min<double>(com1[n], com2[n])*m_branchWeight[n];
		B += (std::max<double>(com1[n], com2[n]) - com2[n])*m_branchWeight[n];
		C += (std::max<double>(com1[n], com2[n]) - com1[n])*m_branchWeight[n];
	}

	return std::min<double>(B, C) / (std::min<double>(B, C) + A);
}

double DiversityCalculator::LennonLRG(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double A = 0;
	double B = 0;
	double C = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		A += std::min<double>(com1[n], com2[n])*m_branchWeight[n];
		B += (std::max<double>(com1[n], com2[n]) - com2[n])*m_branchWeight[n];
		C += (std::max<double>(com1[n], com2[n]) - com1[n])*m_branchWeight[n];
	}		

	return 2*fabs(B-C) / (2*A+B+C);
}

double DiversityCalculator::Manhattan(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
		diss += fabs(com1[n] - com2[n])*m_branchWeight[n];

	return diss;
}

double DiversityCalculator::MNND(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	std::vector<double> minLeafI;
	std::vector<double> minLeafJ;
	m_dataVec.LeafSetMinDistance(com1, com2, minLeafI, minLeafJ);

	double dissA = 0;
	for(uint i = 0; i < minLeafI.size(); ++i)
		dissA += minLeafI[i];

	double dissB = 0;
	for(uint i = 0; i < minLeafJ.size(); ++i)
		dissB += minLeafJ[i];

	if(!m_bWeighted)
	{
		dissA /= minLeafI.size();
		dissB /= minLeafJ.size();
	}

	return 0.5*(dissA + dissB);
}

double DiversityCalculator::MPD(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	std::vector<double> leafPropI;
	std::vector<double> leafPropJ;
	std::vector< std::vector<double> > leafDist;
	m_dataVec.LeafSetDistance(com1, com2, leafPropI, leafPropJ, leafDist);

	double diss = 0;
	double weight = 0;
	for(uint a = 0; a < leafPropI.size(); ++a)
	{
		for(uint b = 0; b < leafPropJ.size(); ++b)
		{
			diss += leafPropI[a]*leafPropJ[b]*leafDist[a][b];
			weight += leafPropI[a]*leafPropJ[b];
		}
	}

	return diss / weight;
}

double DiversityCalculator::MorisitaHorn(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double prodSum = 0;
	double com1SumSqrd = 0;
	double com2SumSqrd = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		prodSum += com1[n]*com2[n]*m_branchWeight[n];

		com1SumSqrd += com1[n]*com1[n]*m_branchWeight[n];
		com2SumSqrd += com2[n]*com2[n]*m_branchWeight[n];
	}

	double num = 2*prodSum;
	double den = ((com1SumSqrd/(m_weightedRowSum[i]*m_weightedRowSum[i])) + (com2SumSqrd/(m_weightedRowSum[j]*m_weightedRowSum[j])))*m_weightedRowSum[i]*m_weightedRowSum[j];

	return 1.0 - num / den;
}

double DiversityCalculator::NormalizedWeightedUniFrac(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	std::vector<double> rootDistI;
	std::vector<double> rootDistJ;
	m_dataVec.LeafSetRootDistance(com1, com2, rootDistI, rootDistJ);

	double num = 0;
	for(uint n = 0; n < com1.size(); ++n)
		num += fabs(com1[n] - com2[n])*m_branchWeight[n];
		
	double den = 0;
	for(uint l = 0; l < rootDistI.size(); ++l)
		den += (rootDistI[l] + rootDistJ[l]);

	if(den == 0)	// can occur if UniFrac is applied to OTU data
		return 0;

	return num / den;
}

double DiversityCalculator::Pearson(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double meanCol1 = m_weightedRowSum[i] / com1.size();
	double meanCol2 = m_weightedRowSum[j] / com2.size();

	double sumProdDiff = 0;
	double sumCom1DiffSqrd = 0;
	double sumCom2DiffSqrd = 0;

	for(uint n = 0; n < com1.size(); ++n)
	{
		double diff1 = com1[n]*m_branchWeight[n] - meanCol1;
		double diff2 = com2[n]*m_branchWeight[n] - meanCol2;

		sumProdDiff += diff1*diff2;

		sumCom1DiffSqrd += diff1*diff1;
		sumCom2DiffSqrd += diff2*diff2;
	}

	double denom = sqrt(sumCom1DiffSqrd*sumCom2DiffSqrd);
	if(denom == 0.0)
		return 0.0;

	double diss = 1 - sumProdDiff / denom;
	return diss;
}

double DiversityCalculator::WeightedCorrelation(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double meanCol1 = m_weightedRowSum[i] / m_totalBranchLen;
	double meanCol2 = m_weightedRowSum[j] / m_totalBranchLen;

	double covXY = 0;
	double covX = 0;
	double covY = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double diff1 = com1[n] - meanCol1;
		double diff2 = com2[n] - meanCol2;

		covXY += m_branchWeight[n]*diff1*diff2;
		covX += m_branchWeight[n]*diff1*diff1;
		covY += m_branchWeight[n]*diff2*diff2;
	}

	covXY /= m_totalBranchLen;
	covX /= m_totalBranchLen;
	covY /= m_totalBranchLen;
	
	double denom = sqrt(covX*covY);
	if(denom == 0.0)
		return 0.0;

	double diss = 1.0 - covXY / denom;
	return diss;
}

double DiversityCalculator::RaoHp(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	std::vector<double> leafPropI;
	std::vector<double> leafPropJ;
	std::vector< std::vector<double> > pairedLeafDistances;
	m_dataVec.PairedLeafSetDistance(com1, com2, leafPropI, leafPropJ, pairedLeafDistances);

	double dT = 0;
	double dA = 0;
	double dB = 0;
	for(uint a = 0; a < leafPropI.size(); ++a)
	{
		for(uint b = 0; b < leafPropJ.size(); ++b)
		{
			dT += 0.5*(leafPropI[a]+leafPropJ[a])*0.5*(leafPropI[b]+leafPropJ[b])*pairedLeafDistances[a][b];
			dA += leafPropI[a]*leafPropI[b]*pairedLeafDistances[a][b];
			dB += leafPropJ[a]*leafPropJ[b]*pairedLeafDistances[a][b];
		}
	}

	return dT - 0.5*(dA+dB);
}

double DiversityCalculator::Soergel(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double num = 0;
	double den = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		num += fabs(com1[n] - com2[n])*m_branchWeight[n];
		den += std::max<double>(com1[n], com2[n])*m_branchWeight[n];
	}

	return num / den;
}

double DiversityCalculator::SpeciesProfile(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		double d = com1[n]/m_rowLeafSum[i] - com2[n]/m_rowLeafSum[j];
		diss += m_branchWeight[n]*d*d;
	}

	return sqrt(diss);
}

double DiversityCalculator::TamasCoefficient(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double num = 0;
	double den = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		num += fabs(com1[n] - com2[n])*m_branchWeight[n];
		den += m_maxExtent[n]*m_branchWeight[n];
	}

	return num / den;
}

double DiversityCalculator::Whittaker(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double diss = 0;
	for(uint n = 0; n < com1.size(); ++n)
		diss += fabs(com1[n] / m_rowLeafSum[i] - com2[n] / m_rowLeafSum[j])*m_branchWeight[n];

	return 0.5 * diss;
}

double DiversityCalculator::YueClayton(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double num = 0;
	double den = 0;
	for(uint n = 0; n < com1.size(); ++n)
	{
		num += com1[n]*com2[n]*m_branchWeight[n];

		double d = com1[n] - com2[n];
		den += (d*d + com1[n]*com2[n])*m_branchWeight[n];
	}

	return 1.0 - num / den;
}

double DiversityCalculator::Unit(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	return 1.0;
}

double DiversityCalculator::Sum(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double sum = 0;
	for(uint n = 0; n < com1.size(); ++n)
		sum += (com1[n] + com2[n])*m_branchWeight[n];

	return sum;
}

double DiversityCalculator::Extents(const std::vector<double>& com1, const std::vector<double>& com2, uint i, uint j)
{
	double extents = 0;
	for(uint n = 0; n < com1.size(); ++n)
		extents += (m_maxExtent[n] - m_minExtent[n])*m_branchWeight[n];

	return extents;
}

bool DiversityCalculator::All(double threshold, const std::string& outputFile)
{
	std::vector<std::string> dissFiles;
	std::vector<std::string> calculatorLabels;

	// calculate all weighted (quantitative) measures
	m_dataVec.Init(m_tree, m_bPhylogenetic, true, !m_bCount, m_seqCountIO.GetSeqs());
	std::set<std::string>::iterator weightedIter;
	m_bWeighted = true;
	for(weightedIter = m_weightedCalculators.begin(); weightedIter != m_weightedCalculators.end(); ++weightedIter)
	{
		std::string calculatorStr = *weightedIter;
		std::cout << "Processing Weighted " << calculatorStr << " calculator..." << std::endl;

		SetCalculator(calculatorStr);
		if(!Dissimilarity("./" + calculatorStr + ".cluster.diss"))
			return false;

		dissFiles.push_back("./" + calculatorStr + ".cluster.diss");
		calculatorLabels.push_back(calculatorStr);
	}

	// calculate all unweighted (qualitative) measures
	m_dataVec.Init(m_tree, m_bPhylogenetic, false, !m_bCount, m_seqCountIO.GetSeqs());
	std::set<std::string>::iterator unweightedIter;
	m_bWeighted = false;
	for(unweightedIter = m_unweightedCalculators.begin(); unweightedIter != m_unweightedCalculators.end(); ++unweightedIter)
	{
		std::string calculatorStr = *unweightedIter;
		std::cout << "Processing Unweighted " << calculatorStr << " calculator..." << std::endl;

		SetCalculator(calculatorStr);
		if(!Dissimilarity("./u" + calculatorStr + ".cluster.diss"))
			return false;

		dissFiles.push_back("./u" + calculatorStr + ".cluster.diss");
		calculatorLabels.push_back("u" + calculatorStr);
	}

	// calculate correlation between all measures
	Matrix corrDissMatrix(dissFiles.size());
	for(uint i = 0; i < dissFiles.size(); ++i)
		corrDissMatrix[i].resize(dissFiles.size());

	for(uint i = 0; i < dissFiles.size(); ++i)
	{
		corrDissMatrix[i][i] = 0;
		for(uint j = 0; j < i; ++j)
		{
			double corrDiss = CorrelationDissimilarity(dissFiles.at(i), dissFiles.at(j));
			corrDissMatrix[i][j] = corrDissMatrix[j][i] = corrDiss;
		}
	}

	// calculate furthest neighbour hierarchical cluster tree
	if(m_bVerbose)
		std::cout << "Clustering calculators..." << std::endl;

	Tree<Node> clusterTree;
	Cluster::CompleteLinkage(corrDissMatrix, calculatorLabels, &clusterTree);

	// calculate clustering value
	std::vector<Node*> postOrder = clusterTree.PostOrder(clusterTree.GetRootNode());
	for(uint i = 0; i < postOrder.size(); ++i)
	{
		if(postOrder.at(i)->IsLeaf())
			postOrder.at(i)->SetValue(0);
		else
		{
			Node* child = postOrder.at(i)->GetChild(0);
			postOrder.at(i)->SetValue(child->GetValue() + child->GetDistanceToParent());
		}
	}

	// determine sets of calculators all within the specified threshold
	if(m_bVerbose)
		std::cout << "Writing results to " << outputFile << "..." << std::endl;

	std::ofstream fout(outputFile.c_str());
	if(!fout.is_open())
	{
		std::cout << "[Error] Failed to open file: " << outputFile << std::endl;
		return false;
	}
	fout << "Minimum r\tCalculators" << std::endl;

	std::stack<Node*> stack;
	stack.push(clusterTree.GetRootNode());
	while(!stack.empty())
	{
		Node* curNode = stack.top();
		stack.pop();

		if(curNode->GetValue() < (1 - threshold))
		{
			if(curNode->IsLeaf())
			{
				fout << "[1.0]\t";
				fout << curNode->GetName() << ";" << std::endl;
			}
			else
			{
				fout << "[" << 1 - curNode->GetValue() << "]\t";

				std::vector<Node*> leaves = curNode->GetLeaves();
				for(uint i = 0; i < leaves.size(); ++i)
					fout << leaves.at(i)->GetName() << ";";
				fout << std::endl;
			}
		}
		else
		{
			std::vector<Node*> children = curNode->GetChildren();
			for(uint i = 0; i < children.size(); ++i)
				stack.push(children.at(i));
		}
	}
	fout << std::endl;
	fout << "Complete linkage cluster tree (branch lengths are 1 - Pearson's correlation): " << std::endl;
	fout.close();

	// write out cluster tree
	NewickIO newickIO;
	newickIO.Write(clusterTree, outputFile, false);

	return true;
}

double DiversityCalculator::CorrelationDissimilarity(const std::string& dissFile1, const std::string dissFile2)
{
	// read first dissimilarity matrix
	std::ifstream fin1(dissFile1.c_str());
	if(!fin1.is_open())
		std::cout << "[Error] Failed to open file: " << dissFile1;

	std::vector<double> x;
	std::string label;
	uint size;
	double value;
	fin1 >> size;
	for(uint r = 0; r < size; ++r)
	{
		fin1 >> label;
		for(uint c = 0; c < r; ++c)
		{
			fin1 >> value;
			x.push_back(value);
		}
	}
	fin1.close();

	// read second dissimilarity matrix
	std::ifstream fin2(dissFile2.c_str());
	if(!fin2.is_open())
		std::cout << "[Error] Failed to open file: " << dissFile1;

	std::vector<double> y;
	fin2 >> size;
	for(uint r = 0; r < size; ++r)
	{
		fin2 >> label;
		for(uint c = 0; c < r; ++c)
		{
			fin2 >> value;
			y.push_back(value);
		}
	}
	fin2.close();

	// perform regression
	LinearRegression::RESULTS results;
	LinearRegression linreg;
	linreg.LeastSquaresEstimate(x, y, results);

	return 1 - results.r;
}

void DiversityCalculator::InitWeightedCalculators()
{
	if(m_weightedCalculators.empty())
	{
		m_weightedCalculators.insert("Bray-Curtis");
		m_weightedCalculators.insert("Canberra");
		m_weightedCalculators.insert("Chi-squared");
		m_weightedCalculators.insert("Coefficient of similarity");
		m_weightedCalculators.insert("Complete tree");
		m_weightedCalculators.insert("Euclidean");
		m_weightedCalculators.insert("Fst");
		m_weightedCalculators.insert("Gower");
		m_weightedCalculators.insert("Hellinger");
		m_weightedCalculators.insert("Kulczynski");
		m_weightedCalculators.insert("Lennon compositional difference");
		m_weightedCalculators.insert("Manhattan");
		m_weightedCalculators.insert("MNND");
		m_weightedCalculators.insert("MPD");
		m_weightedCalculators.insert("Morisita-Horn");
		m_weightedCalculators.insert("Pearson");
		m_weightedCalculators.insert("Rao's Hp");
		m_weightedCalculators.insert("Soergel");
		m_weightedCalculators.insert("Tamas coefficient");
		m_weightedCalculators.insert("Weighted correlation");
		m_weightedCalculators.insert("Whittaker");
		m_weightedCalculators.insert("Yue-Clayton");
	}
}

bool DiversityCalculator::IsWeighted(const std::string& name)
{
	if(m_weightedCalculators.empty())
		InitWeightedCalculators();

	return m_weightedCalculators.find(name) != m_weightedCalculators.end();
}

void DiversityCalculator::InitUnweightedCalculators()
{
	if(m_unweightedCalculators.empty())
	{
		m_unweightedCalculators.insert("Bray-Curtis");
		m_unweightedCalculators.insert("Canberra");
		m_unweightedCalculators.insert("Coefficient of similarity");
		m_unweightedCalculators.insert("Euclidean");
		m_unweightedCalculators.insert("Gower");
		m_unweightedCalculators.insert("Kulczynski");
		m_unweightedCalculators.insert("Lennon compositional difference");
		m_unweightedCalculators.insert("Manhattan");
		m_unweightedCalculators.insert("MNND");
		m_unweightedCalculators.insert("MPD");
		m_unweightedCalculators.insert("Pearson");
		m_unweightedCalculators.insert("Soergel");
		m_unweightedCalculators.insert("Tamas coefficient");
		m_unweightedCalculators.insert("Weighted correlation");
	}
}

bool DiversityCalculator::IsUnweighted(const std::string& name)
{
	if(m_unweightedCalculators.empty())
		InitUnweightedCalculators();

	return m_unweightedCalculators.find(name) != m_unweightedCalculators.end();
}
