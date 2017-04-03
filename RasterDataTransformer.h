#pragma once
#include "DataTransformerBase.h"
class RasterDataTransformer:DataTransformerBase
{
public:
	RasterDataTransformer(const char* filename, string geo, double cenLon);
	~RasterDataTransformer();
	int TransformEllipsod(GDALDataset* sourceDs, const char* pszFormat, const char* outputFileName, GDALResampleAlg resample, OGRSpatialReference * From, OGRSpatialReference* To, OGRSpatialReference * GCPFrom, OGRSpatialReference * GCPTo, _Matrix * M);

	// 通过 DataTransformerBase 继承
	virtual int Transform(string outputFile, OGRSpatialReference * To, OGRSpatialReference * GCPFrom = nullptr, OGRSpatialReference * GCPTo = nullptr, _Matrix * M = nullptr) override;
    virtual void ReadFile(const char* filename);
	public:
	GDALDataset*InputFile;
};

