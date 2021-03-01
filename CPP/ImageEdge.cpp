#include "ImageEdge.h"

ImageEdge::ImageEdge(ifstream& input)
{
	if (input.good()) {
		input >> imageRows;
		input >> imageCols;
		input >> imageMin;
		input >> imageMax;
	}

	initializeArrays();
}

ImageEdge::~ImageEdge()
{
	for (int i = 0; i < (imageRows + 2); i++) {

		delete[] mirrorFramedArray[i];
		delete[] robertRightDiagonal[i];
		delete[] robertLeftDiagonal[i];
		delete[] sobelRightDiagonal[i];
		delete[] sobelLeftDiagonal[i];
		delete[] gradientEdge[i];
		delete[] edgeSum[i];
	}

	delete[] mirrorFramedArray;
	delete[] robertRightDiagonal;
	delete[] robertLeftDiagonal;
	delete[] sobelRightDiagonal;
	delete[] sobelLeftDiagonal;
	delete[] gradientEdge;
	delete[] edgeSum;
}

void ImageEdge::initializeArrays()
{
	maskRobertRightDiagonal = new int* [2];
	maskRobertRightDiagonal[0] = new int[2] { 1, -1};
	maskRobertRightDiagonal[1] = new int[2] {-1, 1};

	maskRobertLeftDiagonal = new int*[2];
	maskRobertLeftDiagonal[0] = new int[2] {-1, 1};
	maskRobertLeftDiagonal[1] = new int[2] { 1, -1};

	maskSobelRightDiagonal = new int*[3];
	maskSobelRightDiagonal[0] = new int[3] { 2, 1, 0 };
	maskSobelRightDiagonal[1] = new int[3] { 1, 0, -1 };
	maskSobelRightDiagonal[2] = new int[3] { 0, -1, -2 };

	maskSobelLeftDiagonal = new int*[3];
	maskSobelLeftDiagonal[0] = new int[3] {  0, 1, 2 };
	maskSobelLeftDiagonal[1] = new int[3] { -1, 0, 1 };
	maskSobelLeftDiagonal[2] = new int[3] { -2, -1, 0 };

	mirrorFramedArray = new int*[imageRows + 2];
	robertRightDiagonal = new int*[imageRows + 2];
	robertLeftDiagonal = new int*[imageRows + 2];
	sobelRightDiagonal = new int*[imageRows + 2];
	sobelLeftDiagonal = new int*[imageRows + 2];
	gradientEdge = new int*[imageRows + 2];
	edgeSum = new int*[imageRows + 2];

	for (int i = 0; i < (imageRows + 2); i++) {

		mirrorFramedArray[i] = new int[imageCols + 2];
		robertRightDiagonal[i] = new int[imageCols + 2];
		robertLeftDiagonal[i] = new int[imageCols + 2];
		sobelRightDiagonal[i] = new int[imageCols + 2];
		sobelLeftDiagonal[i] = new int[imageCols + 2];
		gradientEdge[i] = new int[imageCols + 2];
		edgeSum[i] = new int[imageCols + 2];

		for (int j = 0; j < (imageCols + 2); j++) {
			mirrorFramedArray[i][j] = 0;
			robertRightDiagonal[i][j] = 0;
			robertLeftDiagonal[i][j] = 0;
			sobelRightDiagonal[i][j] = 0;
			sobelLeftDiagonal[i][j] = 0;
			gradientEdge[i][j] = 0;
			edgeSum[i][j] = 0;
		}
	}

}

void ImageEdge::loadImage(ifstream& image, int** mirrorFramedAry)
{
	while (!image.eof()) {
		for (int i = 1; i < imageRows + 1; i++) {
			for (int j = 1; j < imageCols + 1; j++) {
				image >> mirrorFramedAry[i][j];
			}
		}
	}
}

void ImageEdge::mirrorFraming(int** mirrorFramedArray)
{
	for (int i = 0; i < (imageRows + 2); i++) {
		mirrorFramedArray[i][0] = mirrorFramedArray[i][1];
		mirrorFramedArray[i][imageCols + 1] = mirrorFramedArray[i][imageCols];
	}

	for (int i = 0; i < (imageCols + 2); i++) {
		mirrorFramedArray[0][i] = mirrorFramedArray[1][i];
		mirrorFramedArray[imageRows + 1][i] = mirrorFramedArray[imageRows][i];
	}
}

void ImageEdge::identifyEdges(int** robertRightDiagonal, int** robertLeftDiagonal, int** sobelRightDiagonal, int** sobelLeftDiagonal, int** gradientEdge)
{
	for (int i = 1; i < (imageRows + 1); i++) {
		for (int j = 1; j < (imageCols + 1); j++) {
			robertRightDiagonal[i][j] = abs(robertConvolution(i, j, maskRobertRightDiagonal));
			robertLeftDiagonal[i][j] = abs(robertConvolution(i, j, maskRobertLeftDiagonal));
			sobelRightDiagonal[i][j] = abs(sobelConvolution(i, j, maskSobelRightDiagonal));
			sobelLeftDiagonal[i][j] = abs(sobelConvolution(i, j, maskSobelLeftDiagonal));
			gradientEdge[i][j] = abs(computeGradient(i, j));
		}
	}
}

int ImageEdge::robertConvolution(int i, int j, int** mask)
{
	int sum = 0;

	for (int r = -1; r < 1; r++) {
		for (int c = -1; c < 1; c++) {
			sum += (mirrorFramedArray[r + i][c + j] * mask[r + 1][c + 1]);
		}
	}

	return sum;
}

int ImageEdge::sobelConvolution(int i, int j, int** mask)
{
	int sum = 0;

	for (int r = -1; r < 2; r++) {
		for (int c = -1; c < 2; c++) {
			sum += (mirrorFramedArray[r + i][c + j] * mask[r + 1][c + 1]);
		}
	}

	return sum;
}

int ImageEdge::computeGradient(int i, int j)
{
	double answer = 0.0;
	double square1 = pow((mirrorFramedArray[i][j] - mirrorFramedArray[i + 1][j]), 2);
	double square2 = pow((mirrorFramedArray[i][j] - mirrorFramedArray[i][j + 1]), 2);
	answer = sqrt(square1 + square2);
	return (int)answer;
}

int ImageEdge::addTwoArrays(int** array1, int** array2, int** array3, int passId)
{
	int valueMax = -1;

	if (passId == 1) {

		for (int i = 0; i < (imageRows + 2); i++) {
			for (int j = 0; j < (imageCols + 2); j++) {
				array3[i][j] = array1[i][j] + array2[i][j];

				if (array3[i][j] > valueMax) {
					valueMax = array3[i][j];
				}
			}
		}
	}

	else if (passId == 2) {

		for (int i = 0; i < (imageRows + 2); i++) {
			for (int j = 0; j < (imageCols + 2); j++) {
				if (array3[i][j] > valueMax) {
					valueMax = array3[i][j];
				}
			}
		}
	}
	return valueMax;
}

void ImageEdge::prettyPrint(int** array, ofstream& output, int id)
{
	if (id == 1) {

		try {
			for (int i = 0; i < (imageRows + 2); i++) {
				for (int j = 0; j < (imageCols + 2); j++) {
					output << array[i][j] << " ";
				}
				output << endl;
			}
		}
		catch (exception e) {
			cout << "There is an error:" << e.what() << endl;
		}
	}
}

int ImageEdge::getImageRows()
{
	return this->imageRows;
}

int ImageEdge::getImageCols()
{
	return this->imageCols;
}
