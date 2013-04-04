//=======================================================================
// Copyright 2008, Dalhousie University
// Author: Donovan Parks
//
// The contents of this file are licensed under the 
// Attribution-ShareAlike Creative Commons License:
// http://creativecommons.org/licenses/by-sa/3.0/
//=======================================================================

#ifndef _GENGIS_NEWICK_TOKENIZER_
#define _GENGIS_NEWICK_TOKENIZER_

#include <vector>
#include <string>

#include "ErrorHandling.hpp"

/**
 * @brief Class for tokenizing strings in Newick format.
 */
class NewickTokenizer
{
public:
	/**
	* @brief Build a new Newick tokenizer.
	*
	* @param description String to tokenize.
	* @throw IOException If tree can not be parsed into Newick tokens.
	*/
	NewickTokenizer(const std::string& description): m_tokens()
  {
    uint tokCount = 0;
    int parCount = 0;
    uint i;

    for(i = 0; i < description.size(); i++)
    {
      if(description[i] == '(') 
				parCount++; 

      if(description[i] == ')') 
				parCount--; 

			error::ErrorHandling::ThrowIO(parCount >= 0, 
								"NodeTokenizer: closing parenthesis with no opening parenthesis.",
								__FILE__, __LINE__);

      if(description[i] == ',' && parCount == 0)
      {
        // New token found:
        m_tokens.push_back(description.substr(tokCount, i - tokCount));
        tokCount = i + 1;
      }          
    }

    // Add last token:
    m_tokens.push_back(description.substr(tokCount));
  }
  
	/**
	* @brief Return a specific token
	*
	* @param index Position of token to return.
	* @return Token at the specified position.
	*/
	std::string Token(uint index) const { return m_tokens.at(index); }

	/** Number of tokens. */
	uint NumTokens() const { return m_tokens.size(); }

protected:
	/** Newick tokens. */
	std::vector<std::string> m_tokens;
};

#endif