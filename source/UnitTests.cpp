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

#include "UnitTests.hpp"

#include "DiversityCalculator.hpp"

bool UnitTests::Execute()
{
	if(!UnweightedSimpleDataMatrix())
	{
		std::cout << "Failed unweighted simple data matrix unit test." << std::endl;
		return false;
	}

	if(!UnweightedSimpleTree())
	{
		std::cout << "Failed unweighted simple data matrix unit test." << std::endl;
		return false;
	}

	if(!UnweightedDataMatrixMothur())
	{
		std::cout << "Unweighted mothur data matrix test failed." << std::endl;
		return false;
	}

	if(!WeightedDataMatrixMothur())
	{
		std::cout << "Weighted mothur data matrix test failed." << std::endl;
		return false;
	}

	if(!Multifurcating())
	{
		std::cout << "Multifurcating test failed." << std::endl;
		return false;
	}

	if(!SharedSeqs())
	{
		std::cout << "Shared sequences test failed." << std::endl;
		return false;
	}

	return true;
}


bool UnitTests::ReadDissMatrix(const std::string& dissMatrixFile, std::vector< std::vector<double> >& dissMatrix)
{
	dissMatrix.clear();

	// open dissimilarity file
	std::ifstream dissIn(dissMatrixFile.c_str());
	if(!dissIn.is_open())
	{
		std::cerr << "Unable to open dissimilarity matrix file: " << dissMatrixFile << std::endl;
		return false;
	}

	uint size;
	dissIn >> size;

	for(uint i = 0; i < size; ++i)
	{
		std::string name;
		dissIn >> name;

		std::vector<double> row;
		for(uint j = 0; j < i; ++j)
		{
			double v;
			dissIn >> v;
			row.push_back(v);
		}

		dissMatrix.push_back(row);
	}

	return true;
}

bool UnitTests::Compare(double actual, double expected)
{
	return fabs(actual - expected) < 0.00001;
}

bool UnitTests::UnweightedSimpleDataMatrix()
{
	std::string seqCountFile = "../unit-tests/SimpleDataMatrix.env";
	std::vector< std::vector<double> > dissMatrix;

	// unweighted Bray-Curtis (Sorensen)
	DiversityCalculator uBC(seqCountFile, "", "Bray-Curtis", 1000, false, false, false, false, false);
	uBC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1))
		return false;
	if(!Compare(dissMatrix[2][0], 1))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/4.0))
		return false;

	// unweighted Canberra
	DiversityCalculator uCanberra(seqCountFile, "", "Canberra", 1000, false, false, false, false, false);
	uCanberra.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted Euclidean
	DiversityCalculator uEuclidean(seqCountFile, "", "Euclidean", 1000, false, false, false, false, false);
	uEuclidean.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], sqrt(3.0)))
		return false;
	if(!Compare(dissMatrix[2][0], sqrt(3.0)))
		return false;
	if(!Compare(dissMatrix[2][1], sqrt(2.0)))
		return false;

	// unweighted Gower
	DiversityCalculator uGower(seqCountFile, "", "Gower", 1000, false, false, false, false, false);
	uGower.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted Soergel
	DiversityCalculator uSoergel(seqCountFile, "", "Soergel", 1000, false, false, false, false, false);
	uSoergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1))
		return false;
	if(!Compare(dissMatrix[2][0], 1))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/3.0))
		return false;

	// unweighted Kulczynski
	DiversityCalculator uKulczynski(seqCountFile, "", "Kulczynski", 1000, false, false, false, false, false);
	uKulczynski.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1))
		return false;
	if(!Compare(dissMatrix[2][0], 1))
		return false;
	if(!Compare(dissMatrix[2][1], 0.5))
		return false;

	// unweighted Manhattan
	DiversityCalculator uManhattan(seqCountFile, "", "Manhattan", 1000, false, false, false, false, false);
	uManhattan.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted Pearson
	DiversityCalculator uPearson(seqCountFile, "", "Pearson", 1000, false, false, false, false, false);
	uPearson.Dissimilarity("../unit-tests/temp", "UPGMA");
	
	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.57735))
		return false;
	if(!Compare(dissMatrix[2][0], 1.57735))
		return false;
	if(!Compare(dissMatrix[2][1], 1.0))
		return false;

	// unweighted Tamas coefficent
	DiversityCalculator uTC(seqCountFile, "", "Tamas coefficient", 1000, false, false, false, false, false);
	uTC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3.0/4.0))
		return false;
	if(!Compare(dissMatrix[2][0], 3.0/4.0))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/4.0))
		return false;

	return true;
}

bool UnitTests::UnweightedSimpleTree()
{
	std::string seqCountFile = "../unit-tests/SimpleTree.env";
	std::string treeFile = "../unit-tests/SimpleTree.tre";

	std::vector< std::vector<double> > dissMatrix;

	// unweighted Bray-Curtis (Sorensen)
	DiversityCalculator uBC(seqCountFile, treeFile, "Bray-Curtis", 1000, false, false, false, false, false);
	uBC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1))
		return false;
	if(!Compare(dissMatrix[2][0], 1))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/4.0))
		return false;

	// unweighted Canberra
	DiversityCalculator uCanberra(seqCountFile, treeFile, "Canberra", 1000, false, false, false, false, false);
	uCanberra.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted Euclidean
	DiversityCalculator uEuclidean(seqCountFile, treeFile, "Euclidean", 1000, false, false, false, false, false);
	uEuclidean.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], sqrt(3.0)))
		return false;
	if(!Compare(dissMatrix[2][0], sqrt(3.0)))
		return false;
	if(!Compare(dissMatrix[2][1], sqrt(2.0)))
		return false;

	// unweighted Gower
	DiversityCalculator uGower(seqCountFile, treeFile, "Gower", 1000, false, false, false, false, false);
	uGower.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted Soergel
	DiversityCalculator uSoergel(seqCountFile, treeFile, "Soergel", 1000, false, false, false, false, false);
	uSoergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1))
		return false;
	if(!Compare(dissMatrix[2][0], 1))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/3.0))
		return false;

	// unweighted Kulczynski
	DiversityCalculator uKulczynski(seqCountFile, treeFile, "Kulczynski", 1000, false, false, false, false, false);
	uKulczynski.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1))
		return false;
	if(!Compare(dissMatrix[2][0], 1))
		return false;
	if(!Compare(dissMatrix[2][1], 0.5))
		return false;

	// unweighted Manhattan
	DiversityCalculator uManhattan(seqCountFile, treeFile, "Manhattan", 1000, false, false, false, false, false);
	uManhattan.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted Pearson
	DiversityCalculator uPearson(seqCountFile, treeFile, "Pearson", 1000, false, false, false, false, false);
	uPearson.Dissimilarity("../unit-tests/temp", "UPGMA");
	
	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.57735))
		return false;
	if(!Compare(dissMatrix[2][0], 1.57735))
		return false;
	if(!Compare(dissMatrix[2][1], 1.0))
		return false;

	// unweighted Tamas coefficent
	DiversityCalculator uTC(seqCountFile, treeFile, "Tamas coefficient", 1000, false, false, false, false, false);
	uTC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3.0/4.0))
		return false;
	if(!Compare(dissMatrix[2][0], 3.0/4.0))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/4.0))
		return false;

	// unweighted MNND
	DiversityCalculator uMNND(seqCountFile, treeFile, "MNND", 1000, false, false, false, false, false);
	uMNND.Dissimilarity("../unit-tests/temp", "UPGMA");
	
	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	// unweighted MPD
	DiversityCalculator uMPD(seqCountFile, treeFile, "MPD", 1000, false, false, false, false, false);
	uMPD.Dissimilarity("../unit-tests/temp", "UPGMA");
	
	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 3))
		return false;
	if(!Compare(dissMatrix[2][0], 3))
		return false;
	if(!Compare(dissMatrix[2][1], 2))
		return false;

	return true;
}

bool UnitTests::UnweightedDataMatrixMothur()
{
	std::vector< std::vector<double> > dissMatrix;

	// unweighted Soergel
	DiversityCalculator uSoergel("../unit-tests/DataMatrixMothur.env", "", "Soergel", 1000, false, false, false, false, false);
	uSoergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.625))
		return false;
	if(!Compare(dissMatrix[2][0], 0.888889))
		return false;
	if(!Compare(dissMatrix[2][1], 0.714286))
		return false;

	// unweighted Kulczynski
	DiversityCalculator uKulczynski("../unit-tests/DataMatrixMothur.env", "", "Kulczynski", 1000, false, false, false, false, false);
	uKulczynski.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.45))
		return false;
	if(!Compare(dissMatrix[2][0], 0.791667))
		return false;
	if(!Compare(dissMatrix[2][1], 0.55))
		return false;

	// unweighted Euclidean
	DiversityCalculator uEuclidean("../unit-tests/DataMatrixMothur.env", "", "Euclidean", 1000, false, false, false, false, false);
	uEuclidean.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 2.23607))
		return false;
	if(!Compare(dissMatrix[2][0], 2.82843))
		return false;
	if(!Compare(dissMatrix[2][1], 2.23607))
		return false;

	// unweighted Pearson
	DiversityCalculator uPearson("../unit-tests/DataMatrixMothur.env", "", "Pearson", 1000, false, false, false, false, false);
	uPearson.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.0))
		return false;
	if(!Compare(dissMatrix[2][0], 1.58333))
		return false;
	if(!Compare(dissMatrix[2][1], 1.0))
		return false;

	// unweighted Bray-Curtis (aka, Sorensen)
	DiversityCalculator uBC("../unit-tests/DataMatrixMothur.env", "", "Bray-Curtis", 1000, false, false, false, false, false);
	uBC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.454545))
		return false;
	if(!Compare(dissMatrix[2][0], 0.8))
		return false;
	if(!Compare(dissMatrix[2][1], 0.555556))
		return false;

	return true;
}

bool UnitTests::WeightedDataMatrixMothur()
{
	std::vector< std::vector<double> > dissMatrix;

	// weighted Bray-Curtis
	DiversityCalculator BC("../unit-tests/DataMatrixMothur.env", "", "Bray-Curtis", 1000, true, false, false, false, false);
	BC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.8))
		return false;
	if(!Compare(dissMatrix[2][0], 0.6))
		return false;
	if(!Compare(dissMatrix[2][1], 0.8))
		return false;

	// weighted Canberra
	DiversityCalculator Canberra("../unit-tests/DataMatrixMothur.env", "", "Canberra", 1000, true, false, false, false, false);
	Canberra.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 6.35152))
		return false;
	if(!Compare(dissMatrix[2][0], 8.11111))
		return false;
	if(!Compare(dissMatrix[2][1], 5.92063))
		return false;

	// weighted Gower
	DiversityCalculator Gower("../unit-tests/DataMatrixMothur.env", "", "Gower", 1000, true, false, false, false, false);
	Gower.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 6.58333))
		return false;
	if(!Compare(dissMatrix[2][0], 7.88889))
		return false;
	if(!Compare(dissMatrix[2][1], 5.52778))
		return false;

	// weighted Hellinger
	DiversityCalculator Hellinger("../unit-tests/DataMatrixMothur.env", "", "Hellinger", 1000, true, false, false, false, false);
	Hellinger.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.13904))
		return false;
	if(!Compare(dissMatrix[2][0], 1.05146))
		return false;
	if(!Compare(dissMatrix[2][1], 1.17079))
		return false;

	// weighted Manhattan
	DiversityCalculator Manhattan("../unit-tests/DataMatrixMothur.env", "", "Manhattan", 1000, true, false, false, false, false);
	Manhattan.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.6))
		return false;
	if(!Compare(dissMatrix[2][0], 1.2))
		return false;
	if(!Compare(dissMatrix[2][1], 1.6))
		return false;

	// weighted Morisita-Horn
	DiversityCalculator MH("../unit-tests/DataMatrixMothur.env", "", "Morisita-Horn", 1000, true, false, false, false, false);
	MH.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.873239))
		return false;
	if(!Compare(dissMatrix[2][0], 0.333333))
		return false;
	if(!Compare(dissMatrix[2][1], 0.859155))
		return false;

	// weighted Soergel
	DiversityCalculator Soergel("../unit-tests/DataMatrixMothur.env", "", "Soergel", 1000, true, false, false, false, false);
	Soergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.88888901))
		return false;
	if(!Compare(dissMatrix[2][0], 0.75))
		return false;
	if(!Compare(dissMatrix[2][1], 0.88888901))
		return false;

	// weighted species profile
	/*
	DiversityCalculator SP("../unit-tests/DataMatrixMothur.env", "", "Species profile", 1000, true, false, false, false, false);
	SP.Dissimilarity("../unit-tests/temp.diss");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.78740102))
		return false;
	if(!Compare(dissMatrix[2][0], 0.44721401))
		return false;
	if(!Compare(dissMatrix[2][1], 0.78102499))
		return false;
	*/

	// weighted Chi-squared
	// Note: EBD uses a slightly different form of the Chi-squared measure as suggested in Numerical Ecology by Legendre adn Legendre
	// Nonetheless, it is easy to verify this using mothur. Simply divide by sqrt(N), N is the total number of sequences.
	DiversityCalculator ChiSquared("../unit-tests/DataMatrixMothur.env", "", "Chi-squared", 1000, true, false, false, false, false);
	ChiSquared.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.0973200))
		return false;
	if(!Compare(dissMatrix[2][0], 0.96513098))
		return false;
	if(!Compare(dissMatrix[2][1], 1.1147900))
		return false;

	// weighted Euclidean
	DiversityCalculator Euclidean("../unit-tests/DataMatrixMothur.env", "", "Euclidean", 1000, true, false, false, false, false);
	Euclidean.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.78740102))
		return false;
	if(!Compare(dissMatrix[2][0], 0.44721401))
		return false;
	if(!Compare(dissMatrix[2][1], 0.78102499))
		return false;

	// weighted Kulczynski
	DiversityCalculator Kulczynski("../unit-tests/DataMatrixMothur.env", "", "Kulczynski", 1000, true, false, false, false, false);
	Kulczynski.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.8))
		return false;
	if(!Compare(dissMatrix[2][0], 0.6))
		return false;
	if(!Compare(dissMatrix[2][1], 0.8))
		return false;

	// weighted Pearson
	DiversityCalculator uPearson("../unit-tests/DataMatrixMothur.env", "", "Pearson", 1000, true, false, false, false, false);
	uPearson.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 1.22089))
		return false;
	if(!Compare(dissMatrix[2][0], 0.5))
		return false;
	if(!Compare(dissMatrix[2][1], 1.2008))
		return false;

	// weighted Yue-Clayton
	DiversityCalculator YC("../unit-tests/DataMatrixMothur.env", "", "YueClayton", 1000, true, false, false, false, false);
	YC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.93233103))
		return false;
	if(!Compare(dissMatrix[2][0], 0.5))
		return false;
	if(!Compare(dissMatrix[2][1], 0.92424202))
		return false;

	return true;
}

bool UnitTests::Multifurcating()
{
	std::vector< std::vector<double> > dissMatrix;

	// unweighted Soergel
	DiversityCalculator Soergel("../unit-tests/Multifurcating.env", "../unit-tests/Multifurcating.tre", "Soergel", 1000, false, false, false, false, false);
	Soergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.855070))
		return false;

	// weighted Bray-Curtis
	DiversityCalculator BC("../unit-tests/Multifurcating.env", "../unit-tests/Multifurcating.tre", "Bray-Curtis", 1000, true, false, false, false, false);
	BC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.75457500))
		return false;

	// weighted normalized UniFrac
	DiversityCalculator NWU("../unit-tests/Multifurcating.env", "../unit-tests/Multifurcating.tre", "NormalizedWeightedUniFrac", 1000, true, false, false, false, false);
	NWU.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.75457500))
		return false;

	// MRCA restricted weighted normalized UniFrac
	DiversityCalculator MRCA_NWU("../unit-tests/Multifurcating.env", "../unit-tests/Multifurcating.tre", "NormalizedWeightedUniFrac", 1000, true, false, true, false, false);
	MRCA_NWU.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.75457500))
		return false;

	return true;
}

bool UnitTests::SharedSeqs()
{
	std::vector< std::vector<double> > dissMatrix;

	// unweighted MRCA (= MRCA restricted Soergel)
	DiversityCalculator uMRCA("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Soergel", 1000, false, false, true, false, false);
	uMRCA.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 8.0/13.0))
		return false;
	if(!Compare(dissMatrix[2][0], 8.0/13.0))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/6.0))
		return false;

	// unweighted Tamas coefficient
	DiversityCalculator uTC("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Tamas coefficient", 1000, false, false, false, false, false);
	uTC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 8.0/13.0))
		return false;
	if(!Compare(dissMatrix[2][0], 8.0/13.0))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/13.0))
		return false;

	// unweighted Soergel
	DiversityCalculator uSoergel("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Soergel", 1000, false, false, false, false, false);
	uSoergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 8.0/13.0))
		return false;
	if(!Compare(dissMatrix[2][0], 8.0/13.0))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0/9.0))
		return false;

	// unweighted Canberra
	DiversityCalculator uCanberra("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Canberra", 1000, false, false, false, false, false);
	uCanberra.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 8.0))
		return false;
	if(!Compare(dissMatrix[2][0], 8.0))
		return false;
	if(!Compare(dissMatrix[2][1], 2.0))
		return false;

	// weighted Soergel
	DiversityCalculator Soergel("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Soergel", 1000, true, false, false, false, false);
	Soergel.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.55))
		return false;
	if(!Compare(dissMatrix[2][0], 0.615385))
		return false;
	if(!Compare(dissMatrix[2][1], 0.16981))
		return false;

	// weighted Bray-Curtis
	DiversityCalculator BC("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Bray-Curtis", 1000, true, false, false, false, false);
	BC.Dissimilarity("../unit-tests/temp", "UPGMA");

	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.37931000))
		return false;
	if(!Compare(dissMatrix[2][0], 0.444440))
		return false;
	if(!Compare(dissMatrix[2][1], 0.092783500))
		return false;

	// weighted Fst
	DiversityCalculator Fst("../unit-tests/SharedSeqs.env", "../unit-tests/SharedSeqs.tre", "Fst", 1000, true, false, false, false, false);
	Fst.Dissimilarity("../unit-tests/temp", "UPGMA");
	
	ReadDissMatrix("../unit-tests/temp.diss", dissMatrix);
	if(!Compare(dissMatrix[1][0], 0.139560))
		return false;
	if(!Compare(dissMatrix[2][0], 0.18378390))
		return false;
	if(!Compare(dissMatrix[2][1], 0.043478190))
		return false;

	return true;
}