#include "stdafx.h"
#include "VectorDataTransformer.h"
#include "ogr_api.h"
#include "PointTransformer.h"
#include "UsefulKit.h"
VectorDataTransformer::VectorDataTransformer(const char *filename, string geo, double cenLon) : DataTransformerBase()
{
    ReadFile(filename);
    InputProj = myCoordianteBuilder->BulidGaussProjection(cenLon, geo);
}

VectorDataTransformer::~VectorDataTransformer()
{
}
void VectorDataTransformer::ReadFile(const char *filename)
{
    inputFileName = string(filename);
    string ext(CPLGetExtension(filename));
    transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(formatMap[ext].c_str()); //"ESRI Shapefile"
    InputFile = poDriver->Open(filename);
    if (InputFile == nullptr)
    {
	cout << "打开文件失败！" << endl;
	throw("File Open Failed");
    }
}
//过时的函数
void VectorDataTransformer::ReProject(string sourceFile, string desFileName, OGRSpatialReference *sourceProj, OGRSpatialReference *desProj)
{
    //读取矢量数据
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(formatMap[CPLGetExtension(sourceFile.c_str())].c_str()); //"ESRI Shapefile"
    OGRDataSource *poDS = poDriver->Open(sourceFile.c_str(), 1);
    OGRSFDriver *desDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(formatMap[CPLGetExtension(desFileName.c_str())].c_str());
    OGRDataSource *outputDS = desDriver->CreateDataSource(desFileName.c_str(), NULL);
    if (poDS == NULL || outputDS == NULL)
    {
	printf("Open or create file failed.\n");
	exit(1);
    }
    //创建转换器
    PointTransformer *pt = PointTransformer::CreateTransformer(sourceProj, desProj);
    if (pt == NULL)
    {
	cout << "Failed to generate coordinate transformer" << endl;
	exit(1);
    }
    int laycount = poDS->GetLayerCount(); //多个图层对应的数据
    for (int i = 0; i < laycount; i++)
    {
	OGRLayer *poLayer = poDS->GetLayer(i);
	OGRLayer *newLayer = outputDS->CreateLayer(CPLGetFilename(desFileName.c_str()), desProj, poLayer->GetGeomType(), NULL);
	OGRFeature *poFeature;
	poLayer->ResetReading();
	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
	    OGRFeature *newFeature = OGRFeature::CreateFeature(poFeature->GetDefnRef());
	    //复制字段
	    vector<int> mapFields(poFeature->GetFieldCount(), -1);
	    newFeature->SetFieldsFrom(poFeature, &mapFields[0]);
	    OGRGeometry *poGeometry;
	    poGeometry = poFeature->GetGeometryRef();
	    if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
	    {
		OGRPoint *poPoint = (OGRPoint *)poGeometry;
		poPoint->transform(pt->GetTransformer());
		newFeature->SetGeometryDirectly(poPoint);
	    }
	    else if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
	    {
		//处理线段
		OGRLineString *poLine = (OGRLineString *)poGeometry;
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
		OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
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
	}
    }
    GDALClose(poDS);
    GDALClose(outputDS);
}

int VectorDataTransformer::Transform(string outputFile, OGRSpatialReference *To, OGRSpatialReference *GCPFrom /*= nullptr*/, string sourceFile/*=nullptr*/, string desFile/*=nullptr*/)
{
    //获取输出数据驱动
    string ext(CPLGetExtension(outputFile.c_str()));
    transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(formatMap[ext].c_str());
    OGRDataSource *outputDS = poDriver->CreateDataSource(outputFile.c_str(), NULL);
    if (outputDS == NULL)
    {
		printf("Open or create file failed.\n");
		exit(1);
    }
    PointTransformer *pt;
    if (GCPFrom == NULL)
		pt = PointTransformer::CreateTransformer(InputProj, To);
    else
		pt = PointTransformer::CreateTransformer(InputProj, To, GCPFrom,sourceFile,desFile);
    if (pt == NULL)
    {
		cout << "创建控制点转换器失败" << endl;
		exit(1);
    }
    int laycount = InputFile->GetLayerCount(); //多个图层对应的数据
    for (int i = 0; i < laycount; i++)
    {
	OGRLayer *poLayer = InputFile->GetLayer(i);
	OGRLayer *newLayer = outputDS->CreateLayer(poLayer->GetName(), To, poLayer->GetGeomType(), NULL);
	OGRFeature *poFeature;
	poLayer->ResetReading();
	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
	    OGRFeature *newFeature = OGRFeature::CreateFeature(poFeature->GetDefnRef());
	    //复制字段
	    vector<int> mapFields(poFeature->GetFieldCount(), -1);
	    newFeature->SetFieldsFrom(poFeature, &mapFields[0]);
	    OGRGeometry *poGeometry;
	    poGeometry = poFeature->GetGeometryRef();
	    if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
	    {
		OGRPoint *poPoint = (OGRPoint *)poGeometry;
		double x = poPoint->getX();
		double y = poPoint->getY();
		pt->Project(&x, &y, 1);
		poPoint->setX(x);
		poPoint->setY(y);
		newFeature->SetGeometryDirectly(poPoint);
	    }
	    else if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
	    {
		//处理线段//换行符有可能是/r/n
		OGRLineString *poLine = (OGRLineString *)poGeometry;
		pt->ProjectLine(poLine);
		newFeature->SetGeometryDirectly(poLine);
	    }
	    else if (wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString)
	    {
		OGRMultiLineString *poMultiLineString = (OGRMultiLineString *)poGeometry;
		OGRMultiLineString *newMultiLineString = new OGRMultiLineString();
		int num = poMultiLineString->getNumGeometries();
		for (int i = 0; i < num; i++)
		{
		    OGRLineString *line = (OGRLineString *)poMultiLineString->getGeometryRef(i)->clone();
		    pt->ProjectLine(line);
		    newMultiLineString->addGeometryDirectly(line);
		}
		newFeature->SetGeometryDirectly(newMultiLineString);
	    }
	    else if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
	    {
		OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
		//处理外环
		OGRLinearRing *exRing = poPolygon->getExteriorRing();
		OGRPolygon *newPolygon = new OGRPolygon();
		pt->ProjectLine(exRing);
		newPolygon->addRing(exRing);
		//处理内环
		int iRing = poPolygon->getNumInteriorRings();
		for (int i = 0; i < iRing; i++)
		{
		    OGRLinearRing *inRing = poPolygon->getInteriorRing(i);
		    pt->ProjectLine(inRing);
		    newPolygon->addRing(inRing);
		}
		newFeature->SetGeometryDirectly(newPolygon);
	    }
	    else if (wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon)
	    {
		OGRMultiPolygon *poMultiPolygon = (OGRMultiPolygon *)poGeometry;
		OGRMultiPolygon *newMultiPolygon = new OGRMultiPolygon();
		int num = poMultiPolygon->getNumGeometries();
		for (int i = 0; i < num; i++)
		{
		    OGRPolygon *poPolygon = (OGRPolygon *)poMultiPolygon->getGeometryRef(i)->clone();
		    //转换Polygon
		    OGRLinearRing *exRing = poPolygon->getExteriorRing();
		    OGRPolygon *newPolygon = new OGRPolygon();
		    pt->ProjectLine(exRing);
		    newPolygon->addRing(exRing);
		    //处理内环
		    int iRing = poPolygon->getNumInteriorRings();
		    for (int j = 0; j < iRing; j++)
		    {
			OGRLinearRing *inRing = poPolygon->getInteriorRing(j);
			pt->ProjectLine(inRing);
			newPolygon->addRing(inRing);
		    }
		    newMultiPolygon->addGeometryDirectly(newPolygon);
		}
		newFeature->SetGeometryDirectly(newMultiPolygon);
	    }
	    else
	    {
		cout << "暂不支持的几何格式！" << endl;
		return -1;
	    }
	    newLayer->CreateFeature(newFeature);
	    OGRFeature::DestroyFeature(poFeature);
	}
    }
    GDALClose(InputFile);
    GDALClose(outputDS);
    return 0;
}
