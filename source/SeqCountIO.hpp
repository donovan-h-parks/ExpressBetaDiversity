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

#ifndef _SEQ_COUNT_IO_
#define _SEQ_COUNT_IO_

#include "Precompiled.hpp"

/**
 * @brief Read individual sample data from a sequence count file.
 */
class SeqCountIO
{
public:		
	/** Constructor. */
	SeqCountIO();

	/** Destructor. */
	~SeqCountIO();

	/**
	* @brief Open sequence count file.
	*
	* @param filename Path to sequence count file.
	* @return True if file opened successfully, else false.
	*/
	bool Read(const std::string& filename);

	/** Get number of samples. */
	uint GetNumSamples() const { return m_sampleNames.size(); }

	/** Get sample name. */
	std::string GetSampleName(uint index) const { return m_sampleNames.at(index); }

	/** Get number of sequences. */
	uint GetNumSeqs() const { return m_seqs.size(); }

	/** Get sequence name. */
	std::string GetSeqName(uint index) const { return m_seqs.at(index); }

	/** Get sequences. */
	const std::vector<std::string>& GetSeqs() const { return m_seqs; }

	/** Get count data for specified sample. */
	void GetData(uint index, std::vector<double>& count, double& totalNumSeq, uint seqsToDraw = 0);

private:
	/** File stream. */
	std::ifstream m_file;

	/** Start of each sample in sample count file. */
	std::vector<std::streampos> m_sampleStreamPos;

	/** Name of sequences. */
	std::vector<std::string> m_seqs;

	/** Name of samples. */
	std::vector<std::string> m_sampleNames;

	/** Temporary buffer for reading sample data. */
	char* m_buffer;
};

#endif

