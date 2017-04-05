#pragma once
#include "DataTransformerBase.h"
#include "ogrsf_frmts.h"
class VectorDataTransformer:DataTransformerBase
{
public:
	VectorDataTransformer(const char* filename, string geo, double cenLon);
	~VectorDataTransformer();
	//不需要控制点的转换
	void ReProject(string sourceFile, string desFileName, OGRSpatialReference* sourceProj, OGRSpatialReference* desProj);

	// 通过 DataTransformerBase 继承
	virtual int Transform(string outputFile, OGRSpatialReference* To, OGRSpatialReference * GCPFrom = nullptr, string sourceFile=nullptr, string desFile=nullptr) override;
    virtual void ReadFile(const char* filename);
	OGRDataSource* InputFile;
};

