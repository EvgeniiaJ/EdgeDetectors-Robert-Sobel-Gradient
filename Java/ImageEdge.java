import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class ImageEdge {
	
	private int imageRows;
	private int imageCols;
	private int imageMin;
	private int imageMax;
	
//	private int offset;
//	private int thresholdValue;
//	private int maxHeight;
//	private int max;
//	private int min;
	
	public int[][] mirrorFramedArray;
	public int[][] maskRobertRightDiagonal;
	public int[][] maskRobertLeftDiagonal;
	public int[][] maskSobelRightDiagonal;
	public int[][] maskSobelLeftDiagonal;
	
	public int[][] robertRightDiagonal;
	public int[][] robertLeftDiagonal;
	public int[][] sobelRightDiagonal;
	public int[][] sobelLeftDiagonal;
	public int[][] gradientEdge;
	public int[][] edgeSum;
	
	public int[][] histogramGraph;
	public int[][] gaussianGraph;
	public int[] histogramArray;
	public int[] gaussianArray;
	public int[] neighbors2x2Array;
	public int[] neighbors3x3Array;
	
	
	public ImageEdge(Scanner input){
		if(input.hasNext()) {
			this.imageRows = input.nextInt();
		}
		
		if(input.hasNext()) {
			this.imageCols = input.nextInt();
		}
		
		if(input.hasNext()) {
			this.imageMin = input.nextInt();
		}
		
		if(input.hasNext()) {
			this.imageMax = input.nextInt();
		}
		
		initializeArrays();
	}
	
	public void initializeArrays() {
				
		maskRobertRightDiagonal = new int[2][];
		maskRobertRightDiagonal[0] = new int[] { 1, -1};
		maskRobertRightDiagonal[1] = new int[] {-1,  1};
		
		maskRobertLeftDiagonal = new int[2][];
		maskRobertLeftDiagonal[0] = new int[]{-1,  1};
		maskRobertLeftDiagonal[1] = new int[]{ 1, -1};
		
		maskSobelRightDiagonal = new int[3][];
		maskSobelRightDiagonal[0] = new int[] { 2,  1,  0 };
		maskSobelRightDiagonal[1] = new int[] { 1,  0, -1 };
		maskSobelRightDiagonal[2] = new int[] { 0, -1, -2 };
		
		maskSobelLeftDiagonal = new int[3][3];
		maskSobelLeftDiagonal[0] = new int[] {  0,  1, 2 };
		maskSobelLeftDiagonal[1] = new int[] { -1,  0, 1 };
		maskSobelLeftDiagonal[2] = new int[] { -2, -1, 0 };
		
		mirrorFramedArray = new int[imageRows + 2][imageCols + 2];
		robertRightDiagonal = new int[imageRows + 2][imageCols + 2];
		robertLeftDiagonal = new int[imageRows + 2][imageCols + 2];
		sobelRightDiagonal = new int[imageRows + 2][imageCols + 2];
		sobelLeftDiagonal = new int[imageRows + 2][imageCols + 2];
		gradientEdge = new int[imageRows + 2][imageCols + 2];
		edgeSum = new int[imageRows + 2][imageCols + 2];
		neighbors2x2Array = new int[4];
		neighbors3x3Array = new int[9];
		
		
		for(int i = 0; i < (imageRows + 2); i++) {
			for(int j = 0; j < (imageCols + 2); j++) {
				mirrorFramedArray[i][j] = 0;
				robertRightDiagonal[i][j] = 0;
				robertLeftDiagonal[i][j] = 0;
				sobelRightDiagonal[i][j] = 0;
				sobelLeftDiagonal[i][j] = 0;
				gradientEdge[i][j] = 0;
				edgeSum[i][j] = 0;
			}
		}
		
		for(int i = 0; i < neighbors2x2Array.length; i++) {
			neighbors2x2Array[i] = 0;
		}
		
		for(int i = 0; i < neighbors3x3Array.length; i++) {
			neighbors3x3Array[i] = 0;
		}
		
	}
	
	void loadImage(Scanner image, int[][] mirrorFramedAry) {
		while (image.hasNext()) {
			for (int i = 1; i < imageRows + 1; i++) {
				for (int j = 1; j < imageCols + 1; j++) {
					mirrorFramedAry[i][j] = image.nextInt();
				}
			}
		}
	}
	
	public void mirrorFraming(int[][] mirrorFramedArray) {
		for(int i = 0; i < (imageRows + 2); i++) {
			mirrorFramedArray[i][0] = mirrorFramedArray[i][1];
			mirrorFramedArray[i][imageCols + 1] = mirrorFramedArray[i][imageCols];
		}
		
		for(int i = 0; i < (imageCols + 2); i++) {
			mirrorFramedArray[0][i] = mirrorFramedArray[1][i];
			mirrorFramedArray[imageRows + 1][i] = mirrorFramedArray[imageRows][i];
		}
		
	}
	
	public void identifyEdges(int[][] robertRightDiagonal, int[][] robertLeftDiagonal, int[][] sobelRightDiagonal,
			int[][] sobelLeftDiagonal, int[][] gradientEdge) {
		
		for (int i = 1; i < (imageRows + 1); i++) {
			for (int j = 1; j < (imageCols + 1); j++) {
				robertRightDiagonal[i][j] = Math.abs(robertConvolution(i, j, maskRobertRightDiagonal));
				robertLeftDiagonal[i][j] = Math.abs(robertConvolution(i, j, maskRobertLeftDiagonal));
				sobelRightDiagonal[i][j] = Math.abs(sobelConvolution(i, j, maskSobelRightDiagonal));
				sobelLeftDiagonal[i][j] = Math.abs(sobelConvolution(i, j, maskSobelLeftDiagonal));
				gradientEdge[i][j] = Math.abs(computeGradient(i, j));
			}
		}
		
	}
	
	public int robertConvolution(int i, int j, int[][] mask) {
		
		int sum = 0;
		
		for(int r = -1; r < 1; r++) {
			for(int c = -1; c < 1; c++) {
				sum += (mirrorFramedArray[r + i][c + j] * mask[r + 1][c + 1]);
			}
		}
		
		return sum;
	}
	
	public int sobelConvolution(int i, int j, int[][] mask) {

		int sum = 0;

		for (int r = -1; r < 2; r++) {
			for (int c = -1; c < 2; c++) {
				sum += (mirrorFramedArray[r + i][c + j] * mask[r + 1][c + 1]);
			}
		}

		return sum;
	}
	
	public int computeGradient(int i, int j) {
		double answer = 0.0;
		double square1 = Math.pow((mirrorFramedArray[i][j] - mirrorFramedArray[i + 1][j]), 2);
		double square2 = Math.pow((mirrorFramedArray[i][j] - mirrorFramedArray[i][j + 1]), 2);
		answer = Math.sqrt(square1 + square2);
		return (int)answer;
	}
	
	public int addTwoArrays(int[][] array1, int[][] array2, int[][] array3, int passId) {
		int valueMax = -1;
		
		if(passId == 1) {
			
			for(int i = 0; i < (imageRows + 2); i++) {
				for(int j = 0; j < (imageCols + 2); j++) {
					array3[i][j] = array1[i][j] + array2[i][j];
					
					if(array3[i][j] > valueMax) {
						valueMax = array3[i][j];
					}
				}
			}
		}
		
		else if(passId == 2) {
			
			for(int i = 0; i < (imageRows + 2); i++) {
				for(int j = 0; j < (imageCols + 2); j++) {					
					if(array3[i][j] > valueMax) {
						valueMax = array3[i][j];
					}
				}
			}
		}
		return valueMax;
		
	}
	
	void prettyPrint(int[][] array, FileWriter output, int id) {
		
		if (id == 1) {
			
			try {
				for (int i = 0; i < (imageRows + 2); i++) {
					for (int j = 0; j < (imageCols + 2); j++) {
						output.write(array[i][j] + " ");
					}
					output.write("\n");
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
//		if (id == 3) {
//			try {
//				for (int i = 0; i < imageMax + 1; i++) {
//					for (int j = 0; j <= maxHeight; j++) {
//						if (this.histogramGraph[i][j] <= 0) {
//							output.write(" ");
//						} else {
//							output.write("* ");
//						}
//					}
//					output.write("\n");
//				}
//				output.write("\n");
//			} catch (IOException e) {
//				e.printStackTrace();
//			}
//		}
	}
	
	public int getImageRows() {
		return this.imageRows;
		
	}
	
	public int getImageCols() {
		return this.imageCols;
		
	}

}
