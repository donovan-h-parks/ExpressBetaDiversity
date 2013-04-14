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

#include "LinearRegression.hpp"

#include <cmath>
#include <algorithm>

bool LinearRegression::LeastSquaresEstimate(std::vector<double>& x, std::vector<double>& y, RESULTS& results)
{
	int numPts = x.size();
	if(numPts < 2)
		return false;

	double sumX = 0;
	double sumY = 0;
	double sumXY = 0;
	double sumX2 = 0;
	double sumY2 = 0;
	for(int i=0; i < numPts; ++i) 
	{
		sumX += x.at(i);
		sumY += y.at(i);
		sumXY	+= x.at(i) * y.at(i);
		sumX2	+= x.at(i) * x.at(i);
		sumY2	+= y.at(i) * y.at(i);
	}

	double SSxy = sumXY - (sumX*sumY) / numPts;
	double SSxx = sumX2 - (sumX*sumX) / numPts;
	double SSyy = sumY2 - (sumY*sumY) / numPts;

	if(SSxx != 0 && SSyy != 0)
	{
		results.slope = SSxy / SSxx;

		results.cod = (SSxy*SSxy) / (SSxx*SSyy);
		results.r = SSxy / sqrt(SSxx*SSyy);
	}
	else if(SSxx == 0 && SSyy == 0)
	{
		results.slope = DBL_MAX;
		results.cod = DBL_MAX;
		results.r = 1;
	}
	else
	{
		results.slope = DBL_MAX;
		
		results.cod = 1;
		results.r = -1;
	}

	results.offset = (sumY - results.slope*sumX) / numPts;
	results.dof = numPts-2;
	results.nPts = numPts;

	double SSE = SSyy-results.slope*SSxy;
	results.sd = sqrt(SSE / results.dof);
	results.seos = results.sd / sqrt(SSxx);
	results.seoi = results.sd*sqrt((1/numPts) + ((sumX/numPts)*(sumX/numPts))/SSxx);
	
	results.tStat = results.slope / results.seos;

	return true;
}

/**
 * Calculate the error of a given best fit line.
 */
double LinearRegression::Error(int numPts, double sumX, double sumY, double sumXY, 
											double sumX2, double sumY2)
{
	double SSxx = sumX2 - (sumX*sumX) / numPts;
	double SSyy = sumY2 - (sumY*sumY) / numPts;
	double SSxy = sumXY - (sumX*sumY) / numPts;

	double slope = SSxy / SSxx;
	double offset = (sumY - slope*sumX) / numPts;

	return SSyy - slope*SSxy;
}
