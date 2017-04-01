#pragma once
#include "ogr_geometry.h"
class PointTransformer
{
public:
	static PointTransformer* CreateTransformer(char* FromWKT, char* ToWKT);
    static PointTransformer* CreateTransformer(OGRSpatialReference* From, OGRSpatialReference* To);
	static PointTransformer* CreateTransformer(OGRSpatialReference* From, OGRSpatialReference* To, OGRSpatialReference* GCPFrom, OGRSpatialReference* GCPTo, _Matrix* m);
	static _Matrix* GetTransMatrix(string sourceFile, string desFile);

	int Project(double* x, double* y, int count);
	void ProjectLine(OGRLineString* line);
	OGRCoordinateTransformation* GetTransformer()
	{
		return projTransformer;
	}
private:
	PointTransformer();
	~PointTransformer();
	int GCPTransformer(double* x, double* y, int count);
	OGRCoordinateTransformation *projTransformer;
	OGRCoordinateTransformation *fromTransformer;
	OGRCoordinateTransformation *toTransformer;
	_Matrix *M=nullptr;
};

