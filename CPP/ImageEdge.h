#pragma once
#include <iostream>
#include <fstream>

using namespace std;

class ImageEdge
{

private:
	int imageRows;
	int imageCols;
	int imageMin;
	int imageMax;

public:
	int** mirrorFramedArray;
	int** maskRobertRightDiagonal;
	int** maskRobertLeftDiagonal;
	int** maskSobelRightDiagonal;
	int** maskSobelLeftDiagonal;

	int** robertRightDiagonal;
	int** robertLeftDiagonal;
	int** sobelRightDiagonal;
	int** sobelLeftDiagonal;
	int** gradientEdge;
	int** edgeSum;

	int** histogramGraph;
	int** gaussianGraph;
	int* histogramArray;
	int* gaussianArray;

public:

	ImageEdge(ifstream&);

	~ImageEdge();

	void initializeArrays();

	void loadImage(ifstream&, int**);

	void mirrorFraming(int**);

	void identifyEdges(int**, int**, int**, int**, int**);

	int robertConvolution(int, int, int**);

	int sobelConvolution(int, int, int**);

	int computeGradient(int, int);

	int addTwoArrays(int**, int**, int**, int);

	void prettyPrint(int**, ofstream&, int);

	int getImageRows();

	int getImageCols();
};
