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

#ifndef _UNIT_TESTS_

#include "Precompiled.hpp"

/**
 * @brief Execute unit tests.
 */
class UnitTests
{
public:		
	bool Execute();

private:
	/** Test all unweighted measures over a simple data matrix. Ground truth determined by hand. */
	bool UnweightedSimpleDataMatrix();

	/** Test all unweighted measures over a simple tree. Ground truth determined by hand. */
	bool UnweightedSimpleTree();

	/** Test unweighted measures provided within mothur. Ground truth determined by mothur. */
	bool UnweightedDataMatrixMothur();

	/** Test Weighted measures provided within mothur. Ground truth determined by mothur. */
	bool WeightedDataMatrixMothur();

	/** Test multifuricating tree. Ground truth determined by Chameleon and Fast UniFrac. */
	bool Multifurcating();

	/** Test tree with shared sequences. Ground truth determined by Chameleon and Fast UniFrac. */
	bool SharedSeqs();

	bool ReadDissMatrix(const std::string& dissMatrixFile, std::vector< std::vector<double> >& dissMatrix);
	bool Compare(double actual, double expected);
};

#endif

