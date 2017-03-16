#pragma once
class PointTransformer
{
public:
	static PointTransformer* CreateTransformer(char* FromWKT, char* ToWKT);
    static PointTransformer* CreateTransformer(OGRSpatialReference* From, OGRSpatialReference* To);
	static PointTransformer* CreateTransfromer(OGRSpatialReference* From, OGRSpatialReference* To, OGRSpatialReference* GCPFrom, OGRSpatialReference* GCPTo, _Matrix*m);
	static _Matrix* GetTransMatrix(string sourceFile, string desFile);

	// 用于相同椭球体转换
	int Project(double* x, double* y, int count);
	// 用控制点来转换参数
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

