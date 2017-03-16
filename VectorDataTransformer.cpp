#include "stdafx.h"
#include "VectorDataTransformer.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "gdal_priv.h"
#include "PointTransformer.h"
VectorDataTransformer::VectorDataTransformer(string filename, string geo, double cenLon):DataTransformerBase(true)
{
	ReadFile(filename);
	InputProj = myCoordianteBuilder->BulidGaussProjection(cenLon, geo);
}


VectorDataTransformer::~VectorDataTransformer()
{
}

void VectorDataTransformer::ReProject(string sourceFile, string desFileName,OGRSpatialReference* sourceProj, OGRSpatialReference* desProj)
{
	//读取矢量数据
	GDALAllRegister();
	OGRRegisterAll();
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(formatMap[CPLGetExtension(sourceFile.c_str())].c_str());//"ESRI Shapefile"
	OGRDataSource* poDS = poDriver->Open(sourceFile.c_str(), 1);
	OGRSFDriver* desDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(formatMap[CPLGetExtension(desFileName.c_str())].c_str());
	OGRDataSource* outputDS = desDriver->CreateDataSource(desFileName.c_str(), NULL);
	if (poDS == NULL || outputDS == NULL)
	{
		printf("Open or create file failed.\n");
		system("pause");
		exit(1);
	}
	//创建转换器
	PointTransformer* pt = PointTransformer::CreateTransformer(sourceProj, desProj);
	if (pt == NULL)
	{
		system("pause");
		exit(1);
	}
	int laycount = poDS->GetLayerCount();//多个图层对应的数据
	for (int i = 0; i < laycount; i++)
	{
		OGRLayer  *poLayer = poDS->GetLayer(i);
		OGRLayer* newLayer = outputDS->CreateLayer(CPLGetFilename(desFileName.c_str()), desProj, poLayer->GetGeomType(), NULL);
		OGRFeature *poFeature;
		poLayer->ResetReading();
		while ((poFeature = poLayer->GetNextFeature()) != NULL)
		{
			OGRFeature* newFeature = OGRFeature::CreateFeature(poFeature->GetDefnRef());
			//复制字段
			vector<int> mapFields(poFeature->GetFieldCount(), -1);
			newFeature->SetFieldsFrom(poFeature, &mapFields[0]);
			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef();
			if (poGeometry != NULL
				&& wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
			{
				OGRPoint *poPoint = (OGRPoint *)poGeometry;
				poPoint->transform(pt->GetTransformer());
				newFeature->SetGeometryDirectly(poPoint);
			}
			else if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
			{
				//处理线段
				OGRLineString* poLine = (OGRLineString*)poGeometry;
				/*int count = poLine->getNumPoints();
				vector<double> vecx(count), vecy(count);
				poLine->getPoints(&vecx[0], sizeof(double), &vecy[0], sizeof(double));
				pt->Project(&vecx[0], &vecy[0], vecy.size());
				OGRLineString* newLine = new OGRLineString();
				newLine->setPoints(count, &vecx[0], &vecx[0]);*/
				poLine->transform(pt->GetTransformer());
				newFeature->SetGeometry(poLine);
			}
			else if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
			{
				OGRPolygon* poPolygon = (OGRPolygon*)poGeometry;
				poPolygon->transform(pt->GetTransformer());
				newFeature->SetGeometry(poPolygon);
			}
			else
			{
				cout << "不支持的几何格式！" << endl;
				return;
			}
			newLayer->CreateFeature(newFeature);
			OGRFeature::DestroyFeature(poFeature);
			//OGRFeature::DestroyFeature(newFeature);
		}
	}
	GDALClose(poDS);
	GDALClose(outputDS);
}

int VectorDataTransformer::Transform(string outputFile, OGRSpatialReference * To, OGRSpatialReference * GCPFrom /*= nullptr*/, OGRSpatialReference * GCPTo /* =nullptr*/, _Matrix * M /*= nullptr*/)
{
	return 0;
}
