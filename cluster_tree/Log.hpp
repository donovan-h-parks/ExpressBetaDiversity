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