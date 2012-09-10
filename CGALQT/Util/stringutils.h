#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "config.h"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

class StringUtils
{
private:
	StringUtils();
	~StringUtils();

public:
	struct lt_nocase:public std::binary_function<char, char, bool> 
	{
		bool operator()(char x, char y) const 
		{
			return std::toupper(static_cast<unsigned char>(x)) < std::toupper(static_cast<unsigned char>(y));
		}
	};

public:
	static int SplitString(const std::string& input, const std::string& delimiter, std::vector<std::string>& results,bool trimResults = true)
	{
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

		if(trimResults)
		{
			boost::char_separator<char> sep(delimiter.c_str());
			tokenizer tokens(input, sep);
			for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
			{
				std::string str;
				TrimString(*tok_iter,str);
				results.push_back(str);
			}
		}
		else
		{
			boost::char_separator<char> sep(delimiter.c_str());
			tokenizer tokens(input, sep);
			for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
			{
				results.push_back(*tok_iter);
			}
		}

		return static_cast<int>(results.size());
	}

	static void TrimString(const std::string& input, std::string& output)
	{
		if(input.empty())
			return;
		size_t start = input.find_first_not_of(" ");
		size_t end = input.find_last_not_of(" ") + 1;

		output = input.substr(start, end - start);
	}

	static void TrimString_Left(const std::string& input, std::string& output)
	{
		if(input.empty())
			return;
		size_t start = input.find_first_not_of(" ");
		size_t end = input.length();

		output = input.substr(start, end - start);
	}

	static void TrimString_Right(const std::string& input, std::string& output)
	{
		if(input.empty())
			return;
		size_t start = 0;
		size_t end = input.find_last_not_of(" ") + 1;

		output = input.substr(start, end - start);
	}

	//if str1 < str2 return -1
	//elseif str1 == str2 return 0
	//else str1 > str2 return 1
	static int CompareNoCase(const std::string& str1, const std::string& str2)
	{
		bool ret1 = std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), lt_nocase());
		bool ret2 = std::lexicographical_compare(str2.begin(), str2.end(), str1.begin(), str1.end(), lt_nocase());

		if(ret1)
			return -1;
		else if(ret2)
			return 1;
		else
			return 0;
	}

	template <typename T> 
	static std::string to_string(const T& arg)
	{
		try
		{
			return boost::lexical_cast<std::string>(arg);
		}
		catch(boost::bad_lexical_cast& e) 
		{
			std::string error_str = e.what();
			return "";
		}
	}
};

#endif
