// CoordinateTransformer+.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VectorDataTransformer.h"
#include "CoordinateBuilder.h"
#include "RasterDataTransformer.h"
#include "PointTransformer.h"
int main(int argc, char* argv[])
{
	if(argc<=1)
	{
		cout<<"输入参数不足！程序退出"<<endl;
		return 0;
	}
	if (strcmp("-v", argv[1])==0)
	{
		//矢量模式
		VectorDataTransformer* vdt = new VectorDataTransformer(string(argv[2]), string(argv[3]), stod(argv[4]));
		CoordinateBuilder* cb = new CoordinateBuilder();
		OGRSpatialReference* sourceProj = cb->BulidGaussProjection(stod(argv[4]), argv[3]);
		OGRSpatialReference* desProj = cb->BulidGaussProjection(stod(argv[7]), argv[6]);
		vdt->ReProject(argv[2], argv[5], sourceProj, desProj);		
	}
	else if (strcmp("-r", argv[1])==0)
	{
		RasterDataTransformer* rdt = new RasterDataTransformer(string(argv[2]),string(argv[3]),stod(argv[4]));
		CoordinateBuilder* cb = new CoordinateBuilder();
		OGRSpatialReference* sourceProj = cb->BulidGaussProjection(stod(argv[4]), argv[3]);
		OGRSpatialReference* desProj = cb->BulidGaussProjection(stod(argv[7]), argv[6]);
		if (sourceProj->IsSameGeogCS(desProj))
			rdt->Transform(argv[5], desProj);
		else
		{
			if (argc < 11)
			{
				cout << "输入参数不足！无法计算！" << endl;
				return 0;
			}
			OGRSpatialReference* GCPFrom = cb->BulidGaussProjection(stod(argv[9]), argv[3]);
			OGRSpatialReference* GCPTo = cb->BulidGaussProjection(stod(argv[11]), argv[6]);
			_Matrix* m = PointTransformer::GetTransMatrix(argv[8], argv[10]);
			rdt->Transform(argv[5],desProj,GCPFrom,GCPTo,m);
		}
		system("pause");
	}
	return 0;
}

