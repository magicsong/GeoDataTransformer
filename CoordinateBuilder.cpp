#include "stdafx.h"
#include "CoordinateBuilder.h"
#include <map>
//根据投影数据文件读取支持的中国投影
CoordinateBuilder::CoordinateBuilder(char * projFiles)
{
	this->projFiles = projFiles;
	ifstream infile(projFiles);
	while (!infile.eof())
	{
		string title, projString;
		getline(infile, title);
		getline(infile, projString);
		projMap.insert(make_pair(title, projString));
	}
	infile.close();
	cout << "读取" << projMap.size() << "条支持的投影文件" << endl;
}
CoordinateBuilder::CoordinateBuilder()
{
	projMap.insert(make_pair("Beijing54", "2431"));
	projMap.insert(make_pair("Xian80", "2348"));
	projMap.insert(make_pair("CGCS2000", "4512"));
}

CoordinateBuilder::~CoordinateBuilder()
{
}
// 用于生成高斯投影
OGRSpatialReference* CoordinateBuilder::BulidGaussProjection(double cenLon, string geo, bool appendDegree)
{
	OGRSpatialReference* oSRS = new OGRSpatialReference();
	oSRS->importFromEPSG(stod(projMap[geo]));//4512 2000,2348 xian80
	/*string wkt = projMap[geo];
	if (wkt.length() < 1)
	{
		cout << "不支持的投影！！！" << endl;
		return NULL;
	}
	vector<char> v(wkt.begin(), wkt.end());
	v.push_back(0);
	char *p = &v[0];
	oSRS->importFromWkt(&p);
	char* proj4;
	oSRS->exportToProj4(&proj4);
	cout << proj4 << endl;*/
	if (cenLon < 0)
		return oSRS->CloneGeogCS();
	oSRS->SetProjParm("Central_Meridian", cenLon);
	return oSRS;
}
