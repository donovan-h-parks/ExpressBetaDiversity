//=======================================================================
// Author: Donovan Parks
//
// Copyright 2013 Donovan Parks
//
// This file is part of ProjectTree.
//
// ProjectTree is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ProjectTree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ProjectTree. If not, see <http://www.gnu.org/licenses/>.
//=======================================================================

#include "StringTools.hpp"

#include <ctype.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace util;
using namespace std;

bool StringTools::IsEmpty(const string& s)
{
  for(uint i = 0; i < s.size(); i++)
  {
    char c = s[i];
    if(c != ' ' && c != '\n' && c != '\t') return false;
  }
  return true;
}

string StringTools::ToUpper(const string& s)
{
	string upperStr = s;
	for (std::string::iterator p = upperStr.begin(); upperStr.end() != p; ++p)
       *p = toupper(*p);
	return upperStr;
}


string StringTools::ToLower(const string& s)
{
	string lowerStr = s;
	for (std::string::iterator p = lowerStr.begin(); lowerStr.end() != p; ++p)
       *p = tolower(*p);
	return lowerStr;
}

bool StringTools::IsWhiteSpaceCharacter(char c)
{
    return (c == ' ')
        || (c == '\t')
        || (c == '\n')
        || (c == '\r')
        || (c == '\f');  
}

string StringTools::RemoveWhiteSpaces(const string & s)
{
  // Copy sequence
  string st (s);
  
  // For all sequence's characters
  for (uint i = 0; i < st.size(); i++)
  {
    if(IsWhiteSpaceCharacter(st[i]))
    {
      st.erase(st.begin() + i); // Remove character
      i--;
    }
  }

  // Send result
  return st;
}

string StringTools::RemoveFirstWhiteSpaces(const string & s)
{
	std::string str = s;
	return str.erase(0 , str.find_first_not_of(" \r\n\t") );
}

string StringTools::RemoveLastWhiteSpaces(const string & s)
{
	std::string str = s;
	return str.erase( str.find_last_not_of(" \r\n\t") + 1);
}

string StringTools::RemoveSurroundingWhiteSpaces(const string & s)
{
	std::string str = s;
	str = str.erase(0 , str.find_first_not_of(" \r\n\t") );
	str = str.erase( str.find_last_not_of(" \r\n\t") + 1);
	return str;
}

string StringTools::ToString(int i)
{
	ostringstream oss;
  oss << i;
  return oss.str();
}

string StringTools::ToString(char c)
{
  ostringstream oss;
  oss << c;
  return oss.str();
}

string StringTools::ToString(double d, int precision)
{
  ostringstream oss;
	oss << setprecision(precision) << d;
  return oss.str();
}

int StringTools::ToInt(const string & s)
{
  istringstream iss(s);
  int i;
  iss >> i;
  return i;
}

double StringTools::ToDouble(const string & s) 
{
  istringstream iss(s);
  double d;
  iss >> d;
  return d;
}

long StringTools::ToLong(const string & s) 
{
  istringstream iss(s);
  long d;
  iss >> d;
  return d;
}

string StringTools::ResizeRight(const string & s, uint newSize, char fill) 
{
  if(s.size() > newSize) return s.substr(0, newSize);
  else return s + string(newSize - s.size(), fill);
}

string StringTools::ResizeLeft(const string & s, uint newSize, char fill)
{
  if(s.size() > newSize) return s.substr(s.size() - newSize);
  else return string(newSize - s.size(), fill) + s;
}

vector<string> StringTools::Split(const string & s, uint n)
{
  vector<string> v;
  string tmp = s;
  while(tmp.size() > n)
  {
    v.push_back(tmp.substr(0, n));
    tmp = tmp.substr(n);
  }
  v.push_back(tmp);
  return v;
}

string StringTools::RemoveSubstrings(const string & s, char blockBeginning, char blockEnding)
{
  string t = "";
  int blockCount = 0;
  int begPos = 0;
  for(uint i = 0; i < s.size(); i++)
  {
    char current = s[i];
    if(current == blockBeginning)
    {
      blockCount++;
      t += s.substr(begPos, i);
    }
    else if(current == blockEnding)
    {
      blockCount--;
      if(blockCount == 0) {
        begPos = i + 1;
      }
      else if(blockCount < 0)
			{
				blockCount = 0;
			}
    }
  }
  t += s.substr(begPos, s.npos);
  return t;
}

string StringTools::RemoveChar(const string & s, char c)
{
  // Copy sequence
  string st(s);

  // For all sequence's characters
  for (uint i = 0; i < st.size(); i++)
  {
    if (st[i] == c)
    {
      st.erase(st.begin() + i); // Remove character
      i--;
    }
  }

  // Send result
  return st;
}

string StringTools::ReplaceChar(const string & s, char c, char r)
{
  // Copy sequence
  string st(s);

  // For all sequence's characters
  for (uint i = 0; i < st.size(); i++)
  {
    if (st[i] == c)
    {
      st[i] = r; // replace character
    }
  }

  // Send result
  return st;
}

uint StringTools::Count(const string & s, const string & pattern)
{
  uint count = 0;
  string::size_type index = s.find(pattern);
  while(index != string::npos)
  {
    count++;
    index = s.find(pattern, index+1);
  }
  return count;
}

