#pragma once
#include "CoordinateBuilder.h"
class DataTransformerBase
{
public:
	DataTransformerBase(bool isVector);
	virtual ~DataTransformerBase();
	void ReadFile(string fileName);
private:
	virtual int Transform(string outputFile,OGRSpatialReference* To, OGRSpatialReference * GCPFrom = nullptr, OGRSpatialReference * GCPTo = nullptr, _Matrix* M = nullptr) = 0;
public:
	OGRSpatialReference* InputProj;
	GDALDataset* InputFile;
	const bool IsVector;
protected:
	CoordinateBuilder* myCoordianteBuilder;
	unordered_map<string, string>formatMap;
	string inputFileName;
};

