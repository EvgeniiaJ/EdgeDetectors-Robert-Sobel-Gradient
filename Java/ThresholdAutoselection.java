import java.io.FileWriter;
import java.io.IOException;

public class ThresholdAutoselection {

	private int offset;
	private int thresholdValue;
	private int maxHeight;
	private int max;
	private int min;
	
	public int[][] histogramGraph;
	public int[][] gaussianGraph;
	public int[] histogramArray;
	public int[] gaussianArray;
	public int[] neighbors2x2Array;
	public int[] neighbors3x3Array;
	
	
	public ThresholdAutoselection(ImageEdge detectors, int passId) {
//		detectors.edgeSum
		max = 0;
		min = 9999;
		if (passId == 1) {
			for (int i = 0; i < detectors.getImageRows() + 2; i++) {
				for (int j = 0; j < detectors.getImageCols() + 2; j++) {
					if (max < detectors.edgeSum[i][j]) {
						max = detectors.edgeSum[i][j];
					}

					if (min > detectors.edgeSum[i][j]) {
						min = detectors.edgeSum[i][j];
					}
				}
			}
			initializeHistogram(1);
			computeHistogram(detectors.edgeSum, detectors.getImageRows(), detectors.getImageCols(), histogramArray);
		}
		else if(passId == 2) {
			for (int i = 0; i < detectors.getImageRows() + 2; i++) {
				for (int j = 0; j < detectors.getImageCols() + 2; j++) {
					if (max < detectors.gradientEdge[i][j]) {
						max = detectors.gradientEdge[i][j];
					}

					if (min > detectors.gradientEdge[i][j]) {
						min = detectors.gradientEdge[i][j];
					}
				}
			}
			initializeHistogram(1);
			computeHistogram(detectors.gradientEdge, detectors.getImageRows(), detectors.getImageCols(), histogramArray);
		}
		
		
	}
	
	public void initializeHistogram(int passId) {
		
		if(passId == 1) {
			histogramArray = new int[max + 1];
			gaussianArray = new int[max + 1];
			
			for(int i = 0; i <= max; i++) {
				histogramArray[i] = 0;
				gaussianArray[i] = 0;
			}
		}
		
		else if(passId == 2) {
			
			for (int i = 0; i <= max; i++) {
				if (maxHeight < histogramArray[i]) {
					maxHeight = histogramArray[i];
				}
			}
			
			histogramGraph = new int[max + 1][maxHeight + 1];
			gaussianGraph = new int[max + 1][maxHeight + 1];
			
			for(int i = 0; i < max; i++) {
				for(int j = 0; j < maxHeight; j++) {
					histogramGraph[i][j] = 0;
					gaussianGraph[i][j] = 0;
				}
			}			
		}	
		
	}
	
	public void computeHistogram(int[][] array, int rows, int cols, int[] histogram) {
		offset = (int) ((max - min) / 10);
		thresholdValue = offset;
		maxHeight = -1;
		
		for(int i = 1; i <= rows; i++) {
			for(int j = 1; j <= cols; j++) {
				if(array[i][j] != 0) {
					histogram[array[i][j]]++;
				}
			}
		}
		
		initializeHistogram(2);
	}
	
	public void plotHistogramGraph(int[] histogram) {
		for (int i = 1; i <= max; i++) {
			histogramGraph[i][histogram[i]] = 1;
		}
	}
	
	public int biMeanGauss() {
		double sum1, sum2, total, minSumDiff;
		int bestThr = thresholdValue;
		minSumDiff = 999999.0;

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

	void set1DZero(int[] array1D) {
		for (int i = 0; i <= max; i++) {
			array1D[i] = 0;
		}
	}

	void set2DZero(int[][] array2D) {
		for (int i = 0; i <= max; i++) {
			for (int j = 0; j <= maxHeight; j++) {
				array2D[i][j] = 0;
			}
		}
	}

	double fitGauss(int leftIndex, int rightIndex, int[] gaussAry, int[][] gaussGraph) {
		double mean, var, sum, gVal;
		sum = 0.0;
		mean = computeMean(leftIndex, rightIndex, maxHeight);
		var = computeVar(leftIndex, rightIndex, mean);
		int index = leftIndex;
		while (index <= rightIndex) {
			gVal = modifiedGauss(index, mean, var, maxHeight);
			sum += Math.abs(gVal - (double) histogramArray[index]);
			gaussAry[index] = (int) gVal;
			gaussGraph[index][(int) gVal] = 1;
			index++;
		}
		return sum;
	}

	double computeMean(int leftIndex, int rightIndex, int maxHeight2) {
		maxHeight2 = 0;
		int sum = 0;
		int numPixels = 0;
		int index = leftIndex;
		while (index < rightIndex) {
			sum += (histogramArray[index] * index);
			numPixels += histogramArray[index];

			if (histogramArray[index] > maxHeight2) {
				maxHeight2 = histogramArray[index];
			}
			index++;
		}

		return ((double) sum / (double) numPixels);
	}

	double computeVar(int leftIndex, int rightIndex, double mean) {
		double sum = 0.0;
		int numPixels = 0;
		int index = leftIndex;
		while (index < rightIndex) {
			sum += (double) histogramArray[index] * ((double) index - mean) * ((double) index - mean);
			numPixels += histogramArray[index];
			index++;
		}
		return ((double) sum / (double) numPixels);
	}

	double modifiedGauss(int x, double mean, double var, int maxHeight) {
		return (double) (maxHeight * Math.exp(-(((x - mean) * (x - mean)) / (2 * var))));
	}

	void prettyPrint(int[][] array, int numRows, int numCols, FileWriter output, int id) {
		if (id == 1) {
			try {
				for (int i = 0; i < numRows + 2; i++) {
					for (int j = 0; j < numCols + 2; j++) {
						output.write(array[i][j] + " ");
					}
					output.write("\n");
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if (id == 3) {
			try {
				for (int i = 0; i < max + 1; i++) {
					for (int j = 0; j <= this.maxHeight; j++) {
						if (histogramGraph[i][j] <= 0) {
							output.write(" ");
						} else {
							output.write("* ");
						}
					}
					output.write("\n");
				}
				output.write("\n");
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public void thresholdPrint(int[][] array, int numRows, int numCols, FileWriter output, int thrVal) {
		try {
			for (int i = 1; i < numRows + 1; i++) {
				for (int j = 1; j < numCols + 1; j++) {
					if (array[i][j] >= thrVal) {
						output.write("1 ");
					} else {
						output.write(" ");
					}
				}
				output.write("\n");
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void printHistogram(int[] histAry, FileWriter output) {
		try {
			for (int i = 0; i <= max; i++) {
				output.write(i + "  (" + histAry[i] + "):");
				if (histAry[i] != 0) {
					int max = histAry[i];
					if (max > 30) {
						max = 30;
					}
					for (int j = 0; j < max; j++) {
						output.write("+");
					}
				}
				output.write("\n");
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
}
