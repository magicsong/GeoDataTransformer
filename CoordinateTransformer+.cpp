// CoordinateTransformer+.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VectorDataTransformer.h"
#include "CoordinateBuilder.h"
#include "RasterDataTransformer.h"
#include "PointTransformer.h"
int main(int argc, char *argv[])
{
    //For Test
	//-v /home/magicsong/Documents/TestData/Test.mdb Xian80 110.25 /home/magicsong/Documents/TestData/OutputMDB.shp CGCS2000 110.25 /home/magicsong/Documents/TestData/80points.txt 110.25 /home/magicsong/Documents/TestData/2000points.txt 110.25
	if (argc <= 1)
    {
		cout << "输入参数不足！程序退出" << endl;
		return 0;
    }
    if (strcmp("-v", argv[1]) == 0)
    {
	//矢量模式
	VectorDataTransformer *vdt = new VectorDataTransformer(argv[2], string(argv[3]), stod(argv[4]));
	CoordinateBuilder *cb = new CoordinateBuilder();
	OGRSpatialReference *sourceProj = cb->BulidGaussProjection(stod(argv[4]), argv[3]);
	OGRSpatialReference *desProj = cb->BulidGaussProjection(stod(argv[7]), argv[6]);
	if (sourceProj->IsSameGeogCS(desProj))
	    vdt->Transform(argv[5],desProj);
	else
	{
	    if (argc < 11)
	    {
		cout << "输入参数不足！无法计算！" << endl;
		return 0;
	    }
	    OGRSpatialReference *GCPFrom = cb->BulidGaussProjection(stod(argv[9]), argv[3]);
	    vdt->Transform(argv[5], desProj, GCPFrom,argv[8], argv[10]);
	}
    }
    else if (strcmp("-r", argv[1]) == 0)
    {
	RasterDataTransformer *rdt = new RasterDataTransformer(argv[2], string(argv[3]), stod(argv[4]));
	CoordinateBuilder *cb = new CoordinateBuilder();
	OGRSpatialReference *sourceProj = cb->BulidGaussProjection(stod(argv[4]), argv[3]);
	OGRSpatialReference *desProj = cb->BulidGaussProjection(stod(argv[7]), argv[6]);
	if (sourceProj->IsSameGeogCS(desProj))
	    rdt->Transform(argv[5], desProj);
	else
	{
	    if (argc < 11)
	    {
		cout << "输入参数不足！无法计算！" << endl;
		return 0;
	    }
	    OGRSpatialReference *GCPFrom = cb->BulidGaussProjection(stod(argv[9]), argv[3]);
	    rdt->Transform(argv[5], desProj, GCPFrom, argv[8], argv[10]);
	}
	system("pause");
    }
    return 0;
}
