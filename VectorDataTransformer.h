#pragma once
#include "DataTransformerBase.h"
class VectorDataTransformer:DataTransformerBase
{
public:
	VectorDataTransformer(string filename, string geo, double cenLon);
	~VectorDataTransformer();
	void ReProject(string sourceFile, string desFileName, OGRSpatialReference* sourceProj, OGRSpatialReference* desProj);

	// 通过 DataTransformerBase 继承
	virtual int Transform(string outputFile, OGRSpatialReference * To, OGRSpatialReference * GCPFrom = nullptr, OGRSpatialReference * GCPTo = nullptr, _Matrix * M = nullptr) override;
};

