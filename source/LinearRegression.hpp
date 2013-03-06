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

#ifndef LINEAR_REGRESSION_HPP
#define LINEAR_REGRESSION_HPP

#include "Precompiled.hpp"

class LinearRegression
{
public:
	struct RESULTS
	{
		double slope;		// slope, m, of best fit line, y=mx+b
		double offset;	// offset, b, of best fit line, y=mx+b
		double sd;			// standard deviation of residuals, sqrt(SSE / dof)
		double r;				// correlation, R
    double cod;			// coefficent of determination, R^2, of best fit line
		double seos;		// standard error of slope of best fit line
		double seoi;		// standard error of intercept of best fit line
    double tStat;		// t-statistic of best fit line
		double dof;			// degrees of freedom
		int nPts;				// number of data points used to estimate line
		double xStart;	// start of interval dividing data (i.e. end point of line 1)
		double xEnd;		// end of interval dividing data (i.e. start point of line 2)
		double xMid;		// mid-point of interval dividing data
	};

public:
	LinearRegression() {}

	/**
	* Use linear regression (least squares fitting) to find the best line, y=mx+b,
	* through a subset of the data points.
	*
	* @param results Line parameters and statistics for the best fit line
	*/
	bool LeastSquaresEstimate(std::vector<double>& x, std::vector<double>& y, RESULTS& results);

private:
	double Error(int numPts, double sumX, double sumY, double sumXY, 
								double sumX2, double sumY2);
};

#endif