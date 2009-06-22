/* 
 * Copyright 2008, 2009 Nicolas Maingot
 * 
 * This file is part of CSSMatch.
 * 
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#include "ConfigurationFile.h"

#include <fstream>
#include <sstream>

using namespace cssmatch;

void ConfigurationFile::removeEndLine(std::string & line)
{
	size_t strSize = line.size();

	size_t iEndLine = line.find("\r");
	if (iEndLine != std::string::npos)
		line = line.substr(0,iEndLine);
}

void ConfigurationFile::removeComments(std::string & line)
{
	size_t strSize = line.size();

	size_t iComment = line.find("//");

	if (iComment != std::string::npos)
		line = line.substr(0,iComment);
}

void ConfigurationFile::strip(std::string & line)
{
	size_t strSize = line.size();
	size_t iDataBegin = 0;
	size_t iDataEnd = strSize;

	// Strip front
	std::string::const_iterator itChar = line.begin();
	std::string::const_iterator lastChar = line.end();
	bool allFound = false;
	while((itChar != lastChar) && (! allFound))
	{
		if ((*itChar == ' ') || (*itChar == '\t'))
		{
			iDataBegin++;
			itChar++;
		}
		else
			allFound = true;
	}

	// Strip back
	std::string::const_reverse_iterator itRChar = line.rbegin();
	std::string::const_reverse_iterator lastRChar = line.rend();
	allFound = false;
	while((itRChar != lastRChar) && (! allFound))
	{
		if ((*itRChar == ' ') || (*itRChar == '\t'))
		{
			iDataEnd--;
			itRChar++;
		}
		else
			allFound = true;
	}

	line = line.substr(iDataBegin,iDataEnd-iDataBegin);
}

ConfigurationFile::ConfigurationFile(const std::string & path) throw (ConfigurationFileException) : filePath(path)
{
	std::ifstream file(filePath.c_str());
	if (file.fail())
		throw ConfigurationFileException("The file " + filePath + " cannot be found");
}

std::string ConfigurationFile::getFileName() const
{
	std::string fileName;

	size_t iPathSeparator = filePath.find_last_of("/");
	if ((iPathSeparator != std::string::npos) && (iPathSeparator+1 < filePath.length()))
		fileName = filePath.substr(iPathSeparator+1);
	else
		fileName = filePath;

	return fileName;
}

std::list<std::string> ConfigurationFile::getLines()
{
	std::list<std::string> lines;

	std::ifstream file(filePath.c_str());

	if (! file.fail())
	{
		std::string line;

		while(std::getline(file,line))
		{
			removeComments(line);
			removeEndLine(line);
			strip(line);

			if (line.length() > 0)
				lines.push_back(line);
		}
	}

	return lines;
}
