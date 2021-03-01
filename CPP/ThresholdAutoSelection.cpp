#include "ThresholdAutoSelection.h"

ThresholdAutoselection::ThresholdAutoselection(ImageEdge* detectors, int passId)
{
	max = 0;
	min = 9999;
	if (passId == 1) {
		for (int i = 0; i < (detectors->getImageRows() + 2); i++) {
			for (int j = 0; j < (detectors->getImageCols() + 2); j++) {
				if (max < detectors->edgeSum[i][j]) {
					max = detectors->edgeSum[i][j];
				}

				if (min > detectors->edgeSum[i][j]) {
					min = detectors->edgeSum[i][j];
				}
			}
		}

		initializeHistogram(1);
		computeHistogram(detectors->edgeSum, detectors->getImageRows(), detectors->getImageCols(), histogramArray);

	}
}

ThresholdAutoselection::~ThresholdAutoselection()
{
	for (int i = 0; i < max; i++) {
		delete[] histogramGraph[i];
		delete[] gaussianGraph[i];
	}

	delete[] histogramGraph;
	delete[] gaussianGraph;
	delete[] histogramArray;
	delete[] gaussianArray;
	delete[] neighbors2x2Array;
	delete[] neighbors3x3Array;
}

	void ThresholdAutoselection::initializeHistogram(int passId)
{
	if (passId == 1) {
		histogramArray = new int[max + 1];
		gaussianArray = new int[max + 1];

		for (int i = 0; i <= max; i++) {
			histogramArray[i] = 0;
			gaussianArray[i] = 0;
		}
	}

	else if (passId == 2) {

		for (int i = 0; i <= max; i++) {
			if (maxHeight < histogramArray[i]) {
				maxHeight = histogramArray[i];
			}
		}

		histogramGraph = new int*[max + 1];
		gaussianGraph = new int*[max + 1];

		for (int i = 0; i <= max; i++) {

			histogramGraph[i] = new int[maxHeight + 1];
			gaussianGraph[i] = new int[maxHeight + 1];

			for (int j = 0; j <= maxHeight; j++) {
				histogramGraph[i][j] = 0;
				gaussianGraph[i][j] = 0;
			}
		}
	}
}

	void ThresholdAutoselection::computeHistogram(int** array, int rows, int cols, int* histogram)
	{
		offset = (int)((max - min) / 10);
		thresholdValue = offset;
		maxHeight = -1;

		for (int i = 1; i < rows; i++) {
			for (int j = 1; j < cols; j++) {
				if (array[i][j] != 0) {
					histogram[array[i][j]]++;
				}
			}
		}

		initializeHistogram(2);
	}

	void ThresholdAutoselection::plotHistogramGraph(int* histogram)
	{
		for (int i = 1; i <= max; i++) {
			histogramGraph[i][histogram[i]] = 1;
		}
	}

int ThresholdAutoselection::biMeanGauss()
{
	double sum1 = 0.0; 
	double sum2 = 0.0; 
	double total = 0.0; 
	double minSumDiff = 999999.0;
	int bestThr = thresholdValue;

	while (thresholdValue < (max - offset)) {
		set1DZero(gaussianArray);
		set2DZero(gaussianGraph);

		sum1 = fitGauss(0, thresholdValue, gaussianArray, gaussianGraph);
		sum2 = fitGauss(thresholdValue, max, gaussianArray, gaussianGraph);
		total = sum1 + sum2;

		if (total < minSumDiff) {
			minSumDiff = total;
			bestThr = thresholdValue;
		}
		thresholdValue++;
	}
	return bestThr;
}

void ThresholdAutoselection::set1DZero(int* array1D)
{
	for (int i = 0; i <= max; i++) {
		array1D[i] = 0;
	}
}

void ThresholdAutoselection::set2DZero(int** array2D)
{
	for (int i = 0; i <= max; i++) {
		for (int j = 0; j <= maxHeight; j++) {
			array2D[i][j] = 0;
		}
	}
}

double ThresholdAutoselection::fitGauss(int leftIndex, int rightIndex, int* gaussAry, int** gaussGraph)
{
	double mean = 0.0;
	double var = 0.0;
	double sum = 0.0;
	double gVal = 0.0;

	mean = computeMean(leftIndex, rightIndex, maxHeight);
	var = computeVar(leftIndex, rightIndex, mean);

	int index = leftIndex;

	while (index <= rightIndex) {
		gVal = modifiedGauss(index, mean, var, maxHeight);
		sum += abs(gVal - (double)histogramArray[index]);
		gaussAry[index] = (int)gVal;
		gaussGraph[index][(int)gVal] = 1;
		index++;
	}
	return sum;
}

double ThresholdAutoselection::computeMean(int leftIndex, int rightIndex, int height)
{
	height = 0;
	int sum = 0;
	int numPixels = 0;
	int index = leftIndex;

	while (index < rightIndex) {
		sum += (histogramArray[index] * index);
		numPixels += histogramArray[index];

		if (histogramArray[index] > height) {
			height = histogramArray[index];
		}
		index++;
	}

	return ((double)sum / (double)numPixels);
}

double ThresholdAutoselection::computeVar(int leftIndex, int rightIndex, double mean)
{
	double sum = 0.0;
	int numPixels = 0;
	int index = leftIndex;

	while (index < rightIndex) {
		sum += (double)histogramArray[index] * ((double)index - mean) * ((double)index - mean);
		numPixels += histogramArray[index];
		index++;
	}

	return ((double)sum / (double)numPixels);
}

double ThresholdAutoselection::modifiedGauss(int x, double mean, double var, int maxHeight)
{
	return (double)(maxHeight * exp(-(((x - mean) * (x - mean)) / (2 * var))));
}

void ThresholdAutoselection::prettyPrint(int** array, int numRows, int numCols, ofstream& output, int id)
{
	if (id == 1) {
		try {
			for (int i = 0; i < numRows + 2; i++) {
				for (int j = 0; j < numCols + 2; j++) {
					output << array[i][j] << " ";
				}
				output << "\n";
			}
		}
		catch (exception e) {
			cout << "There is an error:" << e.what() << endl;
		}
	}
	if (id == 3) {
		try {
			for (int i = 0; i < max; i++) {
				for (int j = 0; j <= maxHeight; j++) {
					if (histogramGraph[i][j] <= 0) {
						output << " ";
					}
					else {
						output << "* ";
					}
				}
				output << "\n";
			}
			output << "\n";
		}
		catch (exception e) {
			cout << "There is an error:" << e.what() << endl;
		}
	}
}

void ThresholdAutoselection::thresholdPrint(int** array, int numRows, int numCols, ofstream& output, int thrVal)
{
	try {
		for (int i = 1; i < numRows + 1; i++) {
			for (int j = 1; j < numCols + 1; j++) {
				if (array[i][j] >= thrVal) {
					output << "1 ";
				}
				else {
					output << " ";
				}
			}
			output << "\n";
		}
	}
	catch (exception e) {
		cout << "There is an error:" << e.what() << endl;
	}
}

void ThresholdAutoselection::printHistogram(int* histAry, ofstream& output)
{
	try {
		for (int i = 0; i < max; i++) {
			output << i << "  (" << histAry[i] << "):";
			if (histAry[i] != 0) {
				int maxVal = histAry[i];
				if (maxVal > 30) {
					maxVal = 30;
				}
				for (int j = 0; j < maxVal; j++) {
					output << "+";
				}
			}
			output << "\n";
		}
	}
	catch (exception e) {
		cout << "There is an error:" << e.what() << endl;
	}
}
