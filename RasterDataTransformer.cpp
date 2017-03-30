#include "stdafx.h"
#include "RasterDataTransformer.h"
#include "PointTransformer.h"
#include "UsefulKit.h"


RasterDataTransformer::RasterDataTransformer(string filename, string geo, double cenLon) :DataTransformerBase(false)
{
	ReadFile(filename);
	InputProj = myCoordianteBuilder->BulidGaussProjection(cenLon, geo);
}


RasterDataTransformer::~RasterDataTransformer()
{
}

int RasterDataTransformer::TransformEllipsod(GDALDataset * sourceDs, const char* pszFormat, const char* outputFileName, GDALResampleAlg resample, OGRSpatialReference * From, OGRSpatialReference* To, OGRSpatialReference * GCPFrom, OGRSpatialReference * GCPTo, _Matrix * M)
{
	double adfDstGeoTransform[6];
	double dbX[4];
	double dbY[4];
	double dbZ[4] = { 0,0,0,0 };
	int nXsize = sourceDs->GetRasterXSize();
	int nYsize = sourceDs->GetRasterYSize();
	sourceDs->GetGeoTransform(adfDstGeoTransform);
	dbX[0] = adfDstGeoTransform[0];    //左上角点坐标
	dbY[0] = adfDstGeoTransform[3];

	//右上角坐标
	dbX[1] = adfDstGeoTransform[0] + nXsize*adfDstGeoTransform[1];
	dbY[1] = adfDstGeoTransform[3];

	//右下角点坐标
	dbX[2] = adfDstGeoTransform[0] + nXsize*adfDstGeoTransform[1] + nYsize*adfDstGeoTransform[2];
	dbY[2] = adfDstGeoTransform[3] + nXsize*adfDstGeoTransform[4] + nYsize*adfDstGeoTransform[5];

	//左下角坐标
	dbX[3] = adfDstGeoTransform[0];
	dbY[3] = adfDstGeoTransform[3] + nYsize*adfDstGeoTransform[5];

	PointTransformer* gcppt = PointTransformer::CreateTransfromer(From, To, GCPFrom, GCPTo, M);
	UsefulKit* uk = new UsefulKit();
	//cout << "X........." << endl;
	//uk->PrintArray(dbX, 4);
	//cout << "Y........." << endl;
	//uk->PrintArray(dbY, 4);
	gcppt->Project(dbX, dbY, 4);
	//cout << "After X........." << endl; 
	//uk->PrintArray(dbX, 4);
	//cout << "After Y........." << endl;
	//uk->PrintArray(dbY, 4);
	int nGCPCount = 4;
	GDAL_GCP gcpPoints[4];
	stringstream ss;
	for (int i = 0; i < nGCPCount; i++)
	{
		gcpPoints[i].dfGCPX = dbX[i];
		gcpPoints[i].dfGCPY = dbY[i];
		gcpPoints[i].dfGCPZ = 0;
		char* id = new char[2];
		ss << i;
		ss >> id;
		ss.clear();
		gcpPoints[i].pszId = id;
		gcpPoints[i].pszInfo = "";
	}
	gcpPoints[0].dfGCPLine = 0; gcpPoints[0].dfGCPPixel = 0;
	gcpPoints[1].dfGCPLine = 0; gcpPoints[1].dfGCPPixel = nXsize - 1;
	gcpPoints[2].dfGCPLine = nYsize - 1; gcpPoints[2].dfGCPPixel = nXsize - 1;
	gcpPoints[3].dfGCPLine = nYsize - 1; gcpPoints[3].dfGCPPixel = 0;
	GDALGCPsToGeoTransform(4, gcpPoints, adfDstGeoTransform, TRUE);
	// 建立变换选项 
	GDALWarpOptions* psWarpOptions = GDALCreateWarpOptions();
	psWarpOptions->hSrcDS = sourceDs;

	int nBandCount = GDALGetRasterCount(sourceDs);
	psWarpOptions->nBandCount = nBandCount;
	psWarpOptions->panSrcBands =
		(int *)CPLMalloc(sizeof(int) * psWarpOptions->nBandCount);
	for (int i = 0; i < nBandCount; i++)
	{
		psWarpOptions->panSrcBands[i] = i + 1;
	}
	psWarpOptions->panDstBands =
		(int *)CPLMalloc(sizeof(int) * psWarpOptions->nBandCount);
	for (int i = 0; i < nBandCount; i++)
	{
		psWarpOptions->panDstBands[i] = i + 1;
	}
	psWarpOptions->pfnProgress = GDALTermProgress;
	psWarpOptions->eResampleAlg = resample;
	psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
	GDALDriver* outputDriver = (GDALDriver*)GDALGetDriverByName(pszFormat);
	GDALDataset* hDstDS = outputDriver->CreateCopy(outputFileName, sourceDs, false, NULL,NULL, NULL);

	// 创建重投影变换函数    
	psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer(sourceDs, "",hDstDS, "", 0, 100, 0);
	if (psWarpOptions->pTransformerArg == NULL)
	{
		cout << "Error when create Transform Arg!" << endl;
	}
	if (hDstDS == NULL)
		{
			cout << "创建输出文件失败" << endl;
			system("pause");
			exit(1);
		}
	// 写入投影
	psWarpOptions->hDstDS = hDstDS;
	char* proj;
	GCPTo->exportToWkt(&proj);
	hDstDS->SetProjection(proj);
	GDALSetGeoTransform(hDstDS, adfDstGeoTransform);
	// 复制颜色表，如果有的话    
	GDALColorTableH hCT;
	hCT = GDALGetRasterColorTable(GDALGetRasterBand(sourceDs, 1));
	if (hCT != NULL)
		GDALSetRasterColorTable(GDALGetRasterBand(hDstDS, 1), hCT);


	// 初始化并且执行变换操作    
	GDALWarpOperation oOperation;
	oOperation.Initialize(psWarpOptions);
	oOperation.ChunkAndWarpImage(0, 0, GDALGetRasterXSize(hDstDS), GDALGetRasterYSize(hDstDS));
	GDALDestroyGenImgProjTransformer(psWarpOptions->pTransformerArg);
	GDALDestroyWarpOptions(psWarpOptions);
	GDALClose(hDstDS);
	GDALClose(sourceDs);
	return 0;
}

int RasterDataTransformer::Transform(string outputFile, OGRSpatialReference * To, OGRSpatialReference * GCPFrom /*= nullptr*/, OGRSpatialReference * GCPTo /*= nullptr*/, _Matrix * M /*= nullptr*/)
{
	char* fromProj, *toProj;
	InputProj->exportToProj4(&fromProj);
	To->exportToProj4(&toProj);
	if (InputProj->IsSameGeogCS(To))
	{
		string cmd = "gdalwarp -s_srs \"" + string(fromProj) + "\" -t_srs \"" + string(toProj) + "\" -overwrite  --config GDAL_FILENAME_IS_UTF8 NO \"" + inputFileName + "\" \"" + outputFile + "\"";
		//cout << cmd << endl;
		system(cmd.c_str());
	}
	else
	{
		if (M == NULL)
		{
			cout << "不同的椭球体转换需要控制点！" << endl;
			return -1;
		}
		
		string ext(CPLGetExtension(outputFile.c_str()));
		transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
		TransformEllipsod(InputFile, formatMap[ext].c_str(), outputFile.c_str(), GRA_Bilinear,InputProj, To, GCPFrom, GCPTo, M);
	}
	return 0;
}

