//=======================================================================
// Author: Donovan Parks
//
// Copyright 2013 Donovan Parks
//
// This file is part of ClusterTree.
//
// ClusterTree is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ClusterTree is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ClusterTree. If not, see <http://www.gnu.org/licenses/>.
//=======================================================================

#ifndef _ERROR_HANDLING_
#define _ERROR_HANDLING_

#include "Exceptions.hpp"

#include <stdlib.h>
#include <string>


namespace error
{

/**
 * @brief Error handling functions.
 */
class ErrorHandling
{
public:
	/**
	 * @brief Assert that a boolean expression is true.
	 * @param b Results of expression.
	 * @param error Error message to display when assert fails.
	 */
	static void Assert(bool b, const char* error, const char* file, int line) throw (Exception)
	{
		#ifndef NDEBUG
			if(!b)
			{
				throw Exception("Assert failed. \nFile: " + std::string(file) + "\n" + std::string(error));
			}
		#endif
	}

	/**
	 * @brief Throw a general exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void Throw(bool b, const char* error, const char* file, int line) throw (Exception)
	{
		if(!b)
		{
			throw Exception("Exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

	/**
	 * @brief Throw an IO exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void ThrowIO(bool b, const char* error, const char* file, int line) throw (IOException)
	{
		if(!b)
		{
			throw IOException("IO exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

	/**
	 * @brief Throw a NULL pointer exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void ThrowNull(bool b, const char* error, const char* file, int line) throw (NullPointerException)
	{
		if(!b)
		{
			throw NullPointerException("Null pointer exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

	/**
	 * @brief Throw a division by zero exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void ThrowDivByZero(bool b, const char* error, const char* file, int line) throw (ZeroDivisionException)
	{
		if(!b)
		{
			throw ZeroDivisionException("Division by zero exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

	/**
	 * @brief Throw an invalid number exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void ThrowInvalidNum(bool b, const char* error, const char* file, int line) throw (InvalidNumberException)
	{
		if(!b)
		{
			throw InvalidNumberException("Invalid number exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

	/**
	 * @brief Throw an out of bounds exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void ThrowOutOfBounds(bool b, const char* error, const char* file, int line) throw (OutOfBoundsException)
	{
		if(!b)
		{
			throw OutOfBoundsException("Out of bounds exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

	/**
	 * @brief Throw an OpenGL exception if a boolean expression is false.
	 * @param b Results of expression.
	 * @param error Error message to display with exception.
	 */
	static void ThrowGLException(bool b, const char* error, const char* file, int line) throw (GLException)
	{
		if(!b)
		{
			throw GLException("OpenGL exception. \nFile: " + std::string(file) + "\n" + std::string(error));
		}
	}

};

}

#endif