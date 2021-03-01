#include <iostream>
#include <fstream>
#include <string>

#include "ImageEdge.h"
#include "ThresholdAutoSelection.h"

using namespace std;

int main(int argc, char** argv) {

	string imageFileName;
	string robertEdgeFileName;
	string sobelEdgeFileName;
	string gradientEdgeFileName;
	string printingFileName;
	string debugingFileName;
	int imageMax;
	int bestThreshold;

	if (argc != 7) {
		cout << "Invalid number of arguments." << endl;
		cout << "There should be image file, structuring element file, dilationOutput file, erosionOutput file, openingOutput file, and closingOutputFile." << endl;
		exit(0);
	}

	try {

		imageMax = -1;
		bestThreshold = -1;

		imageFileName = argv[1];
		robertEdgeFileName = argv[2];
		sobelEdgeFileName = argv[3];
		gradientEdgeFileName = argv[4];
		printingFileName = argv[5];
		debugingFileName = argv[6];

		ifstream image;
		ofstream robert;
		ofstream sobel;
		ofstream gradient;
		ofstream printing;
		ofstream debuging;

		image.open(imageFileName);
		
		robert.open(robertEdgeFileName);
		sobel.open(sobelEdgeFileName);
		gradient.open(gradientEdgeFileName);
		printing.open(printingFileName);
		debuging.open(debugingFileName);


		ImageEdge* edgeDetection = new ImageEdge(image);
		edgeDetection->loadImage(image, edgeDetection->mirrorFramedArray);
		edgeDetection->mirrorFraming(edgeDetection->mirrorFramedArray);

		edgeDetection->identifyEdges(edgeDetection->robertRightDiagonal, edgeDetection->robertLeftDiagonal,
			edgeDetection->sobelRightDiagonal, edgeDetection->sobelLeftDiagonal, edgeDetection->gradientEdge);

		imageMax = edgeDetection->addTwoArrays(edgeDetection->robertRightDiagonal, edgeDetection->robertLeftDiagonal, edgeDetection->edgeSum, 1);
		debuging << "Content of Robert Right Diagonal:\n";
		edgeDetection->prettyPrint(edgeDetection->robertRightDiagonal, debuging, 1);
		debuging << "\nContent of Robert Left Diagonal:\n";
		edgeDetection->prettyPrint(edgeDetection->robertLeftDiagonal, debuging, 1);
		robert << "Sum of Two Robert's Edge Diagonals:\n";
		edgeDetection->prettyPrint(edgeDetection->edgeSum, robert, 1);
		ThresholdAutoselection* selection = new ThresholdAutoselection(edgeDetection, 1);
		robert << "\nHistogram of Robert Edge Detection:\n";
		selection->printHistogram(selection->histogramArray, robert);

		selection->plotHistogramGraph(selection->histogramArray);
		robert << "\nBi-Gaussian Curve Fitting of the Graph:\n";
		selection->prettyPrint(selection->histogramGraph, edgeDetection->getImageRows(), edgeDetection->getImageCols(), robert, 3);
		bestThreshold = selection->biMeanGauss();
		printing << "Best Threshold Value of Robert's Edge Detector: " + bestThreshold << ". The Result:\n";
		selection->thresholdPrint(edgeDetection->edgeSum, edgeDetection->getImageRows(), edgeDetection->getImageCols(), printing, bestThreshold);



		imageMax = edgeDetection->addTwoArrays(edgeDetection->sobelRightDiagonal, edgeDetection->sobelLeftDiagonal, edgeDetection->edgeSum, 1);
		debuging << "Content of Sobel Right Diagonal:\n";
		edgeDetection->prettyPrint(edgeDetection->sobelRightDiagonal, debuging, 1);
		debuging << "\nContent of Sobel Left Diagonal:\n";
		edgeDetection->prettyPrint(edgeDetection->sobelLeftDiagonal, debuging, 1);
		sobel << "Sum of Two Sobel's Edge Diagonals:\n";
		edgeDetection->prettyPrint(edgeDetection->edgeSum, sobel, 1);
		selection = new ThresholdAutoselection(edgeDetection, 1);
		sobel << "\nHistogram of Sobel Edge Detection:\n";
		selection->printHistogram(selection->histogramArray, sobel);

		selection->plotHistogramGraph(selection->histogramArray);
		sobel << "\nBi-Gaussian Curve Fitting of the Graph:\n";
		selection->prettyPrint(selection->histogramGraph, edgeDetection->getImageRows(), edgeDetection->getImageCols(), sobel, 3);
		bestThreshold = selection->biMeanGauss();
		printing << "\n\nBest Threshold Value of Sobel's Edge Detector: " << bestThreshold << ". The Result:\n";
		selection->thresholdPrint(edgeDetection->edgeSum, edgeDetection->getImageRows(), edgeDetection->getImageCols(), printing, bestThreshold);


		imageMax = edgeDetection->addTwoArrays(edgeDetection->gradientEdge, edgeDetection->gradientEdge, edgeDetection->edgeSum, 2);
		debuging << "Content of Gradient Detector:\n";
		edgeDetection->prettyPrint(edgeDetection->gradientEdge, debuging, 1);
		gradient << "Content of the Gradient Detector:\n";
		edgeDetection->prettyPrint(edgeDetection->gradientEdge, gradient, 1);
		selection = new ThresholdAutoselection(edgeDetection, 2);
		gradient << "\nHistogram of Gradient Detection:\n";
		selection->printHistogram(selection->histogramArray, gradient);

		selection->plotHistogramGraph(selection->histogramArray);
		gradient << "\nBi-Gaussian Curve Fitting of the Graph:\n";
		selection->prettyPrint(selection->histogramGraph, edgeDetection->getImageRows(), edgeDetection->getImageCols(), gradient, 3);
		bestThreshold = selection->biMeanGauss();
		printing << "\n\nBest Threshold Value of Gradient Detector: " << bestThreshold << ". The Result:\n";
		selection->thresholdPrint(edgeDetection->gradientEdge, edgeDetection->getImageRows(), edgeDetection->getImageCols(), printing, bestThreshold);

		image.close();
		robert.close();
		sobel.close();
		gradient.close();
		printing.close();
		debuging.close();

	}
	catch (exception e) {
		cout << "There is an error:" << e.what() << endl;
	}

	return 0;
}