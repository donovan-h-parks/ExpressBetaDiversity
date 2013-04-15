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

#include "SeqCountIO.hpp"
#include "StringTools.hpp"

SeqCountIO::SeqCountIO() 
{
	m_buffer = NULL;
}

SeqCountIO::~SeqCountIO() 
{ 
	if(m_buffer)
		delete[] m_buffer;

	if(m_file.is_open())
		m_file.close(); 
}

bool SeqCountIO::Read(const std::string& filename)
{
	m_file.open(filename.c_str());
	if(!m_file.is_open())
	{
		std::cerr << "Unable to open sequence file: " << filename << std::endl;
		return false;
	}

	// check if file ends with a end-of-line character(s)
	char c;
	bool bEndOfLineTerminator = true;
	m_file.seekg(-1, std::ios::end);
	m_file.read(&c, 1);
	if(c != '\n')
		bEndOfLineTerminator = false;
	m_file.seekg(0, std::ios::beg);

	// parse header line to get order of sequences
	std::string line, token;
	std::getline(m_file, line);
	std::stringstream ss(line);
	while(std::getline(ss, token, '\t'))
	{
		if(!token.empty())
			m_seqs.push_back(StringTools::RemoveSurroundingWhiteSpaces(token));
	}

	// get number of samples and starting index of each sample line
	std::streamsize longestLine = 0;
	do
	{
		if(!line.empty())
		{
			if(!m_sampleStreamPos.empty())
			{
				uint pos = line.find('\t');
				m_sampleNames.push_back(line.substr(0, pos));

				std::streamsize lineLen = m_file.tellg() - m_sampleStreamPos[m_sampleStreamPos.size()-1] + 2;	// +2 is for end-of-line character
				if(lineLen > longestLine)
					longestLine = lineLen;
			}

			m_sampleStreamPos.push_back(m_file.tellg());
		}
	} while(std::getline(m_file, line));
	m_file.clear();

	// allocate temporary buffer for reading lines
	m_buffer = new char[(uint)longestLine];

	// ensure we read the entire last line of the file
	if(!bEndOfLineTerminator)
	{
		int endOfLineLen = 1;
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
			endOfLineLen = 2;
		#endif
	
		m_file.seekg(endOfLineLen, std::ios::end);
		m_sampleStreamPos[m_sampleStreamPos.size()-1] = m_file.tellg();
	}

	return true;
}

void SeqCountIO::GetData(uint index, std::vector<double>& count, double& totalNumSeq, uint seqsToDraw)
{
	// read the ith sample from file
	m_file.seekg(m_sampleStreamPos[index]);
	
	std::streamsize charsInLine = m_sampleStreamPos[index+1] - m_sampleStreamPos[index] - 1;
	m_file.read(m_buffer, charsInLine);
	m_buffer[charsInLine] = 0;
	
	// read sample name
	char* curPos = (char *)memchr(m_buffer, '\t', (size_t)charsInLine);
	++curPos;
	charsInLine -= (curPos - m_buffer);

	// read count data
	totalNumSeq = 0;
	count.clear();
	count.reserve(m_seqs.size());
	do
	{
		char* tabPos = (char *)memchr(curPos, '\t', (size_t)charsInLine);
		if(tabPos != NULL)
			tabPos[0] = 0;

		double numSeq = (double)StringTools::ToDouble(curPos);
		count.push_back(numSeq);
		totalNumSeq += numSeq;		

		charsInLine -= (tabPos - curPos) + 1;
		curPos = tabPos + 1;
	}while(count.size() != m_seqs.size());

	// jackknife data vector
	if(seqsToDraw != 0)
	{	
		std::vector<double> jackknife(m_seqs.size(), 0);
		for(uint i = 0; i < seqsToDraw; ++i)
		{
			double r = rand()/(RAND_MAX/totalNumSeq);

			double seqCount = 0;
			for(uint j = 0; j < count.size(); ++j)
			{
				seqCount += count.at(j);
				if(r <= seqCount)
				{
					jackknife.at(j) += 1;
					break;
				}
			}
		}

		totalNumSeq = seqsToDraw;
		count = jackknife;
	}
}