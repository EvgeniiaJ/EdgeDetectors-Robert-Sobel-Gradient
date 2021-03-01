#pragma once
#include <iostream>
#include <fstream>

#include "ImageEdge.h"

using namespace std;

class ThresholdAutoselection
{

private:
	int offset;
	int thresholdValue;
	int maxHeight;
	int max;
	int min;

public:
	int** histogramGraph;
	int** gaussianGraph;
	int* histogramArray;
	int* gaussianArray;
	int* neighbors2x2Array;
	int* neighbors3x3Array;

	
public:

	ThresholdAutoselection(ImageEdge*, int);

	~ThresholdAutoselection();

	void initializeHistogram(int);

	void computeHistogram(int**, int, int, int*);

	void plotHistogramGraph(int*);

	int biMeanGauss();

	void set1DZero(int*);

	void set2DZero(int**);

	double fitGauss(int, int, int*, int**);

	double computeMean(int, int, int);

	double computeVar(int, int, double);

	double modifiedGauss(int, double, double, int);

	void prettyPrint(int**, int, int, ofstream&, int);

	void thresholdPrint(int**, int, int, ofstream&, int);

	void printHistogram(int*, ofstream&);
};

