#include "CSVReader.h"
#include <sstream>

std::string const& CSVReader::operator[](std::size_t index) const
{
	return m_data[index];
}

std::size_t CSVReader::size() const
{
	return m_data.size();
}

CSVReader::CSVReader()
	: file (nullptr)
{

}

CSVReader::~CSVReader()
{
	if (file != nullptr && file->is_open())
		file->close();
}

void CSVReader::readNextRow()
{
	std::string line;
	std::getline(*file, line);

	std::stringstream lineStream(line);
	std::string cell;

	m_data.clear();
	while (std::getline(lineStream, cell, ','))
		m_data.push_back(cell);

	if (!lineStream && cell.empty()) // Check for training comma
		m_data.push_back("");
}

void CSVReader::open(const std::string& filename)
{
	std::string name = "csv/" + filename;
	if (file != nullptr && file->is_open())
		file->close();
	file = new std::ifstream(name);
	if (file->is_open())
		printf("CSVReader: File parsed: %s\n", filename.c_str());
	else
		printf("CSVReader: Could not open file for parsing: %s\n", filename.c_str());
}
