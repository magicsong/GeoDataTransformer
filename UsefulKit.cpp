#include "stdafx.h"
#include "UsefulKit.h"

UsefulKit::UsefulKit()
{
}

UsefulKit::~UsefulKit()
{
}

string UsefulKit::FindFileName(string path, bool hasExtention)
{
    int pos = path.find_last_of('/');
    string filename = (path.substr(pos + 1));
    if (hasExtention)
    {
	return filename;
    }
    else
    {
	pos = filename.find_last_of('.');
	return filename.substr(0, pos);
    }
}
string UsefulKit::TrimEnd(string line, char ch)
{
    int i;
    for (i = line.length() - 1; i >= 0; i--)
    {
	if (line[i] != ch)
	    break;
    }
    return (i == line.length()- 1) ? line :line.substr(0, i + 1);
}
