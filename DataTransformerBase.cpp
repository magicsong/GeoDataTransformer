#include "stdafx.h"
#include "DataTransformerBase.h"





DataTransformerBase::DataTransformerBase(bool isVector) :IsVector(isVector)
{
	myCoordianteBuilder = new CoordinateBuilder();
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	//读取格式文件配置
	ifstream in("formats.txt");
	while (!in.eof())
	{
		string line;
		getline(in, line);
		int index = line.find_first_of('~');
		formatMap.insert(make_pair(line.substr(0, index - 1), line.substr(index + 1)));
	}
	in.close();
}



DataTransformerBase::~DataTransformerBase()
{

}

void DataTransformerBase::ReadFile(string fileName)
{
	inputFileName = fileName;
	InputFile = (GDALDataset*)GDALOpen(fileName.c_str(), GA_ReadOnly);
	if (InputFile == nullptr)
	{
		cout << "打开文件失败！" << endl;
		throw("File Open Failed");
	}
}
