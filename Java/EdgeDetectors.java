import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class EdgeDetectors {

	public static void main(String[] args) {
		
		String imageFileName;
		String robertEdgeFileName;
		String sobelEdgeFileName;
		String gradientEdgeFileName;
		String printingFileName;
		String debugingFileName;
		int imageMax;
		int bestThreshold;

		if (args.length != 6) {
			System.out.println("Invalid number of arguments.");
			System.out.println(
					"There should be image file, structuring element file, dilationOutput file, erosionOutput file, openingOutput file, and closingOutputFile.");
			System.exit(0);
		}
		try {

			imageMax = -1;
			bestThreshold = -1;
			
			imageFileName = args[0];
			robertEdgeFileName = args[1];
			sobelEdgeFileName = args[2];
			gradientEdgeFileName = args[3];
			printingFileName = args[4];
			debugingFileName = args[5];
			
			Scanner image = new Scanner(new File(imageFileName));
			FileWriter robert = new FileWriter(new File(robertEdgeFileName));
			FileWriter sobel = new FileWriter(new File(sobelEdgeFileName));
			FileWriter gradient = new FileWriter(new File(gradientEdgeFileName));
			FileWriter printing = new FileWriter(new File(printingFileName));
			FileWriter debuging = new FileWriter(new File(debugingFileName));

			
			ImageEdge edgeDetection = new ImageEdge(image);
			edgeDetection.loadImage(image, edgeDetection.mirrorFramedArray);
			edgeDetection.mirrorFraming(edgeDetection.mirrorFramedArray);
			
			edgeDetection.identifyEdges(edgeDetection.robertRightDiagonal, edgeDetection.robertLeftDiagonal,
					edgeDetection.sobelRightDiagonal, edgeDetection.sobelLeftDiagonal, edgeDetection.gradientEdge);
			
			imageMax = edgeDetection.addTwoArrays(edgeDetection.robertRightDiagonal, edgeDetection.robertLeftDiagonal,edgeDetection.edgeSum, 1);
			debuging.write("Content of Robert Right Diagonal:\n");
			edgeDetection.prettyPrint(edgeDetection.robertRightDiagonal, debuging, 1);
			debuging.write("\nContent of Robert Left Diagonal:\n");
			edgeDetection.prettyPrint(edgeDetection.robertLeftDiagonal, debuging, 1);
			robert.write("Sum of Two Robert's Edge Diagonals:\n");
			edgeDetection.prettyPrint(edgeDetection.edgeSum, robert, 1);
			ThresholdAutoselection selection = new ThresholdAutoselection(edgeDetection, 1);
			robert.write("\nHistogram of Robert Edge Detection:\n");
			selection.printHistogram(selection.histogramArray, robert);
			
			selection.plotHistogramGraph(selection.histogramArray);
			robert.write("\nBi-Gaussian Curve Fitting of the Graph:\n");
			selection.prettyPrint(selection.histogramGraph, edgeDetection.getImageRows(), edgeDetection.getImageCols(), robert, 3);
			bestThreshold = selection.biMeanGauss();
			printing.write("Best Threshold Value of Robert's Edge Detector: " + bestThreshold + ". The Result:\n");
			selection.thresholdPrint(edgeDetection.edgeSum, edgeDetection.getImageRows(), edgeDetection.getImageCols(), printing, bestThreshold);
			
			
			
			imageMax = edgeDetection.addTwoArrays(edgeDetection.sobelRightDiagonal, edgeDetection.sobelLeftDiagonal,edgeDetection.edgeSum, 1);
			debuging.write("Content of Sobel Right Diagonal:\n");
			edgeDetection.prettyPrint(edgeDetection.sobelRightDiagonal, debuging, 1);
			debuging.write("\nContent of Sobel Left Diagonal:\n");
			edgeDetection.prettyPrint(edgeDetection.sobelLeftDiagonal, debuging, 1);
			sobel.write("Sum of Two Sobel's Edge Diagonals:\n");
			edgeDetection.prettyPrint(edgeDetection.edgeSum, sobel, 1);
			selection = new ThresholdAutoselection(edgeDetection, 1);
			sobel.write("\nHistogram of Sobel Edge Detection:\n");
			selection.printHistogram(selection.histogramArray, sobel);
			
			selection.plotHistogramGraph(selection.histogramArray);
			sobel.write("\nBi-Gaussian Curve Fitting of the Graph:\n");
			selection.prettyPrint(selection.histogramGraph, edgeDetection.getImageRows(), edgeDetection.getImageCols(), sobel, 3);
			bestThreshold = selection.biMeanGauss();
			printing.write("\n\nBest Threshold Value of Sobel's Edge Detector: " + bestThreshold + ". The Result:\n");
			selection.thresholdPrint(edgeDetection.edgeSum, edgeDetection.getImageRows(), edgeDetection.getImageCols(), printing, bestThreshold);
			
			
			imageMax = edgeDetection.addTwoArrays(edgeDetection.gradientEdge, edgeDetection.gradientEdge,edgeDetection.edgeSum, 2);
			debuging.write("Content of Gradient Detector:\n");
			edgeDetection.prettyPrint(edgeDetection.gradientEdge, debuging, 1);
			gradient.write("Content of the Gradient Detector:\n");
			edgeDetection.prettyPrint(edgeDetection.gradientEdge, gradient, 1);
			selection = new ThresholdAutoselection(edgeDetection, 2);
			gradient.write("\nHistogram of Gradient Detection:\n");
			selection.printHistogram(selection.histogramArray, gradient);
			
			selection.plotHistogramGraph(selection.histogramArray);
			gradient.write("\nBi-Gaussian Curve Fitting of the Graph:\n");
			selection.prettyPrint(selection.histogramGraph, edgeDetection.getImageRows(), edgeDetection.getImageCols(), gradient, 3);
			bestThreshold = selection.biMeanGauss();
			printing.write("\n\nBest Threshold Value of Gradient Detector: " + bestThreshold + ". The Result:\n");
			selection.thresholdPrint(edgeDetection.gradientEdge, edgeDetection.getImageRows(), edgeDetection.getImageCols(), printing, bestThreshold);
			

			image.close();
			robert.close();
			sobel.close();
			gradient.close();
			printing.close();
			debuging.close();

		} catch (IOException e) {
			e.printStackTrace();
		}

	}

}
