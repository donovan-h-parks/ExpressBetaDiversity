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

#ifndef _EXCEPTIONS_
#define _EXCEPTIONS_

#include <stdexcept>
#include <string>

namespace error
{

/**
 * @brief Exception class.
 */
	class Exception: public std::exception
{
public:
  /**
   * @brief Generate a new Exception.
   *
   * @param text A message indicating reason for the exception.
   */
  Exception(const char* text): m_message(std::string(text)) {}

  /**
   * @brief Generate a new Exception.
   *
   * @param text A message indicating reason for the exception.
   */
  Exception(const std::string& text): m_message(text) {}

  virtual ~Exception() throw() {}
  
  /**
   * @brief Get the message attached to the exception.
   *
   * @return A message indicating why the exception was thrown.
   */
  const char* what() const throw() 
	{ 
		return m_message.c_str(); 
	}

  protected:
    std::string m_message;
};

/**
 * @brief Input/output (IO) exception.
 */
class IOException: public Exception
{
public:     
	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	IOException(const char* text): Exception(text) {}

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	IOException(const std::string& text): Exception(text) {} 

	virtual ~IOException() throw() {}

};


/**
 * @brief NULL pointer exception.
 */
class NullPointerException:
  public Exception
{
public:

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	NullPointerException(const char* text): Exception(text) {}

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	NullPointerException(const std::string& text): Exception(text) {}

	virtual ~NullPointerException() throw() {}
};


/**
 * @brief Division by zero exception.
 */
class ZeroDivisionException: public Exception
{
public:

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	ZeroDivisionException(const char* text): Exception(text) {}

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	ZeroDivisionException(const std::string& text): Exception(text) {}

	virtual ~ZeroDivisionException() throw() {}
};

  
/**
 * @brief Invalid number exception.
 */
class InvalidNumberException: public Exception
{ 
public:
  
	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	InvalidNumberException(const char* text): Exception(text) {}

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	InvalidNumberException(const std::string& text): Exception(text) {}

  virtual ~InvalidNumberException() throw() {} 
};


/**
 * @brief Out of bounds exception.
 */
class OutOfBoundsException: public Exception
{
public:
    
	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	OutOfBoundsException(const char* text): Exception(text) {}

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	OutOfBoundsException(const std::string& text): Exception(text) {}

	virtual ~OutOfBoundsException() throw() {}
};

/**
 * @brief OpenGL exception.
 */
class GLException: public Exception
{
public:
    
	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	GLException(const char* text): Exception(text) {}

	/**
	 * @brief Generate a new Exception.
	 *
	 * @param text A message indicating reason for the exception.
	 */
	GLException(const std::string& text): Exception(text) {}

	virtual ~GLException() throw() {}
};

} //end of namespace phylo.

#endif

