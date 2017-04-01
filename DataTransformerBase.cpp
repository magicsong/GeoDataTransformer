#include "stdafx.h"
#include "DataTransformerBase.h"
#include "UsefulKit.h"




DataTransformerBase::DataTransformerBase()
{
	myCoordianteBuilder = new CoordinateBuilder();
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	//读取格式文件配置
	ifstream in("formats.txt");//换行符有可能是/r/n
	while (!in.eof())
	{
		string line;
		getline(in, line);
		int index = line.find_first_of('~');
		formatMap.insert(make_pair(line.substr(0, index), UsefulKit::TrimEnd(line.substr(index + 1),'\r')));
	}
	in.close();
}



DataTransformerBase::~DataTransformerBase()
{

}

