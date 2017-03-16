#pragma once
#include "DataTransformerBase.h"
class RasterDataTransformer:DataTransformerBase
{
public:
	RasterDataTransformer(string filename, string geo, double cenLon);
	~RasterDataTransformer();
	int TransformEllipsod(GDALDataset * sourceDs, const char* pszFormat, const char* outputFileName, GDALResampleAlg resample, OGRSpatialReference * From, OGRSpatialReference* To, OGRSpatialReference * GCPFrom, OGRSpatialReference * GCPTo, _Matrix * M);

	// Í¨¹ý DataTransformerBase ¼Ì³Ð

	virtual int Transform(string outputFile, OGRSpatialReference * To, OGRSpatialReference * GCPFrom = nullptr, OGRSpatialReference * GCPTo = nullptr, _Matrix * M = nullptr) override;

};

