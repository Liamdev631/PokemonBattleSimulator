#pragma once
//#define _ITERATOR_DEBUG_LEVEL 0
#include <fstream>
#include <vector>
#include <string>

class CSVReader
{
private:
	std::vector<std::string> m_data;
	std::ifstream* file;

public:
	CSVReader();
	~CSVReader();

	std::string const& operator[](std::size_t index) const;
	std::size_t size() const;
	void readNextRow();
	void open(const std::string& filename);
};
