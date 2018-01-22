#include "pch.h"
#include "VboParser.h"

VboParser::VboParser(Filesystem::path & directory) : m_directory(directory)
{
	Filesystem::create_directories(m_directory);
}
