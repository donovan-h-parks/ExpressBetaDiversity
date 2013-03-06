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


// Reference additional headers your program requires here

#ifndef PRECOMPILED_HPP
#define PRECOMIPLED_HPP

// disable checked iterators as this produces a large performance hit
// (see: http://msdn.microsoft.com/en-us/library/aa985896(VS.80).aspx and google search for SECURE_SCL)
#ifdef NDEBUG
	#define _SECURE_SCL 0
#endif

#include <stdio.h>
#include <assert.h>

#include <cstring>
#include <ctime>
#include <cfloat>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <queue>
#include <numeric>

#ifdef WIN32
	#include <functional>
#else
	#include <tr1/functional>
#endif

#include "DataTypes.hpp"

#endif


