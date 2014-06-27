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
#ifndef _STRINGTOOLS_H_
#define _STRINGTOOLS_H_

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "DataTypes.hpp"

/**
 * @brief Some utilitary functions that work on strings.
 */
class StringTools
{
public:

	/**
	 * @brief Tell if a std::string is empty.
	 *
	 * A std::string is considered to be 'empty' if it is only made of white spaces.
	 *
	 * @param s The std::string to check.
	 * @return True if the std::string has only white characters.
	 */
	static bool IsEmpty(const std::string & s);

	/**
	 * @brief Tell if a std::string is a decimal number.
	 *
	 * @param s The std::string to check.
	 * @return True if the std::string is an decimal number.
	 */
	static bool IsDecimalNumber(const std::string & s)
	{
		std::istringstream iss( s );
		double dTestSink;
		iss >> dTestSink;

		// was any input successfully consumed/converted?
		if ( ! iss )
			return false;
	 
		// was all the input successfully consumed/converted?
		return ( iss.rdbuf()->in_avail() == 0 );
	}

	/**
	 * @brief Make the std::string uppercase.
	 *
	 * @param s The std::string to analyse.
	 * @return The std::string with all chars changed to uppercase.
	 */
	static std::string ToUpper(const std::string & s);

	/**
	 * @brief Make the std::string lowercase.
	 *
	 * @param s The std::string to analyse.
	 * @return The std::string with all chars changed to lowercase.
	 */
	static std::string ToLower(const std::string & s);

	/**
	 * @brief Tell if a character is a white space or not.
	 *
	 * @param c The character to check.
	 * @return True if c is one of the following: ' ', '\\t', '\\n', '\\r' or '\\f'.
	 */
	static bool IsWhiteSpaceCharacter(char c);

	/**
	 * @brief Remove all white spaces characters in a std::string.
	 *
	 * @param s The std::string to parse.
	 * @return A copy of 's' without white spaces characters.
	 */
	static std::string RemoveWhiteSpaces (const std::string & s);

	/**
	 * @brief Remove all white spaces characters at the beginning of a std::string.
	 *
	 * @param s The std::string to parse.
	 * @return The std::string beginning with the first non-white character.
	 */
	static std::string RemoveFirstWhiteSpaces (const std::string & s);

	/**
	 * @brief Remove all white spaces characters at the end of a std::string.
	 *
	 * @param s The std::string to parse.
	 * @return The std::string ending with the last non-white character.
	 */
	static std::string RemoveLastWhiteSpaces (const std::string & s);

	/**
	 * @brief Remove all white spaces characters at the beginning and the
	 * end of a std::string.
	 *
	 * @param s The std::string to parse.
	 * @return The std::string beginning with the first non-white character
	 * and ending with the last one.
	 */
	static std::string RemoveSurroundingWhiteSpaces(const std::string & s);

  /**
   * @brief General template method to convert to a std::string.
   *
   * @param t The object to convert.
   * @return A std::string equal to t.
   */
  template<class T> static std::string ToString(T t)
	{
		std::ostringstream oss;
    oss << t;
    return oss.str();
  }

	/**
	 * @brief Template std::string conversion.
	 * 
	 * @param t The object to convert.
	 * @param precision To use (for numbers).
	 * @return A std::string equal to t.
	 */
	template<class T>
	static std::string ToString(T t, int precision)
	{
		std::ostringstream oss;
		oss << std::setprecision(precision) << t;
		return oss.str();
	}

  /**
   * @brief General template method to convert from std::string.
   *
   * @param s The std::string to convert.
   * @return An object from std::string t.
   */
  template<class T> static T FromString(const std::string & s)
	{
    std::istringstream iss(s);
    T obj;
    iss >> obj;
    return obj;
  }

  /**
	 * @brief Convert from int to std::string.
	 *
	 * @param i The integer to convert.
	 * @return A std::string equal to i.
	 */
	static std::string ToString(int i);

  /**
	 * @brief Convert from char to std::string.
	 *
	 * @param c The character to convert.
	 * @return A std::string equal to c.
	 */
	static std::string ToString(char c);

	/**
	 * @brief Convert from double to std::string.
	 *
	 * @param d The double to convert.
	 * @param precision To use (for numbers).
	 * @return A std::string equal to d.
	 */
	static std::string ToString(double d, int precision = 6);

	/**
	 * @brief Convert from std::string to int.
	 *
	 * @param s The std::string to parse.
	 * @return The integer corresponding to s.
	 */
	static int ToInt(const std::string & s);

	/**
	 * @brief Convert from std::string to double.
	 *
	 * @param s The std::string to parse.
	 * @return The double corresponding to s.
	 */
	static double ToDouble(const char *p);

	/**
	 * @brief Convert from std::string to long.
	 *
	 * @param s The std::string to parse.
	 * @return The long corresponding to s.
	 */
	static long ToLong(const std::string & s);

	/**
	 * @brief Template to std::string conversion.
	 * 
	 * @param s The std::string to parse.
	 * @return An object of class R corresponding to s.
	 */
	template<class T>
	static T To(const std::string & s)
	{
		std::istringstream iss(s);
		T t;
		iss >> t;
		return t;
	}


	/**
	 * @brief Send a std::string of size 'newSize', which is a copy of 's' truncated or
	 * filled with character 'fill' at the end.
	 *
	 * @param s       The std::string to parse.
	 * @param newSize The new std::string size.
	 * @param fill    The character to use to fill the std::string id length < newSize.
	 * @return A std::string of size newsize which is a copy from the left of s.
	 */
	static std::string ResizeRight(const std::string & s, uint newSize, char fill = ' ');

	/**
	 * @brief Send a std::string of size 'newSize', which is a copy of 's' truncated or
	 * filled with character 'fill' at the beginning.
	 *
	 * @param s       The std::string to parse.
	 * @param newSize The new std::string size.
	 * @param fill    The character to use to fill the std::string id length < newSize.
	 * @return A std::string of size newsize which is a copy from the right of s.
	 */
	static std::string ResizeLeft(const std::string & s, uint newSize, char fill = ' ');

	/**
	 * @brief Split a std::string into parts of size 'n'.
	 *
	 * The last part may contain < n chars.
	 *
	 * @param s The std::string to parse.
	 * @param n The number of tokens.
	 * @return A vector of strings with all tokens.
	 */
	static std::vector<std::string> Split(const std::string & s, uint n);

	/**
	 * @brief Remove substrings from a std::string.
	 *
	 * All substrings beginning with blockBeginning
	 * and ending with blockEnding will be removed.
	 * Nesting blocks are allowed, the most extern block will be removed.
	 *
	 * @param s The std::string to parse.
	 * @param blockBeginning The character specifying the beginning of each block.
	 * @param blockEnding    The character specifying the end of each block.
	 * @return The std::string with all blocks removed.
	 */
	static std::string RemoveSubstrings(const std::string & s, char blockBeginning, char blockEnding);

  /**
   * @brief Remove all occurences of a character in a std::string.
   *
   * @param s The std::string to parse.
   * @param c The character to remove.
   * @return The std::string with all specified chars removed.
   */
  static std::string RemoveChar(const std::string & s, char c);

	/**
   * @brief Replace all occurences of a character in a std::string with another character.
   *
   * @param s The std::string to parse.
   * @param c The character to replace.
	 * @param r The replacement character.
   * @return The std::string with all specified chars removed.
   */
  static std::string ReplaceChar(const std::string & s, char c, char r);

  /**
   * @brief Count the occurences of a given pattern in a std::string.
   *
   * @param s The std::string to search.
   * @param pattern The pattern to use.
   * @return The number of occurences of 'pattern' in 's'.
   */
  static uint Count(const std::string & s, const std::string & pattern);
};

#endif

