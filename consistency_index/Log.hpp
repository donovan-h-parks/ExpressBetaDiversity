//=======================================================================
// Copyright 2008, Dalhousie University
// Author: Donovan Parks
//
// The contents of this file are licensed under the 
// Attribution-ShareAlike Creative Commons License:
// http://creativecommons.org/licenses/by-sa/3.0/
//=======================================================================

#ifndef _LOG_
#define _LOG_

#include <fstream>
#include <vector>
#include <string>

namespace error
{

/**
 * @brief Singleton class for holding project settings.
 */
class Log
{
public:
	/** Destructor */
	~Log()
	{
		for(unsigned int i = 0; i < m_logData.size(); ++i)
		{
			m_log << m_logData.at(i) << std::endl;
		}

		m_log.close();
	}

	/** Get the one and only instance of this class. */
	static Log& Inst()
	{
		static Log log;
		return log;
	}

	/** Add text to log file. */
	void Add(const char* text)
	{
		m_logData.push_back(text);
	}

	/** Add text to log file. */
	void Add(const std::string text)
	{
		m_logData.push_back(text);
	}

	/** Add text to log file. */
	void Write(const char* text)
	{
		m_log << text << std::endl;
	}

	/** Add text to log file. */
	void Write(const std::string text)
	{
		m_log << text << std::endl;
	}

protected:
	/** Private singleton constructor (must instantiate this class through Inst()). */
  Log()
	{
		m_log.open("Log.txt");
		m_logData.reserve(1000);	// allocate enough space to store a thousands entries
	}

	/** Private copy constructor (must instantiate this class through Inst()). */
  Log(const Log&);

	/** Private assignment constructor (must instantiate this class through Inst()). */
  Log& operator= (const Log&);


private:
	/** Log stream */
	std::ofstream m_log;

	/** Vector of log data. */
	std::vector<std::string> m_logData;
};

}

#endif