#pragma once
class CoordinateBuilder
{
public:
	CoordinateBuilder(char* projFiles);
	CoordinateBuilder();
	~CoordinateBuilder();
	// 用于生成高斯投影字符串
	OGRSpatialReference* BulidGaussProjection(double cenLon, string geo, bool appendDegree=false);
	map<string, string> projMap;
private:
	char* projFiles;
};

