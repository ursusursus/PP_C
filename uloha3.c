#include <stdio.h>

void printMatrix(int mRows, int mColumns, int m[mRows][mColumns]) {
	int row, column;
	for(row = 0; row < mRows; row++) {
		for(column = 0; column < mColumns; column++) {
			printf("%d ", m[row][column]);
		}
		printf("\n");
	}
	printf("\n");
}

void populateMatrix(int mRows, int mColumns, int m[mRows][mColumns]) {
	int row, column;
	for(row = 0; row < mRows; row++) {
		for(column = 0; column < mColumns; column++) {
			m[row][column] = rand() % 10;
		}
	}
}

void multiplyMatrices(int aRows, int aColumns, int aM[aRows][aColumns], int bRows, int bColumns, int bM[bRows][bColumns], int rRows, int rColumns, int rM[rRows][rColumns]) {
	int i, j, k;
	for(i = 0; i < aRows; i++){
	    for(j = 0; j < aRows; j++){
	    	// Pre-init with zero
	    	rM[i][j] = 0;
	        for(k = 0; k < aRows - 1; k++){
	            rM[i][j] += aM[i][k] * bM[k][j];
	        }
	    }
	}
}

/**
* Nasobenie matic
*/
int main(int argc, char *argv[]) {
	// Init
	const int aRows = 3;
	const int aColumns = 2;
	// Kktina ta inicializacia
	// int matrixA[3][2] = {{8,1}, {8,6}, {9,1}};
	int matrixA[aRows][aColumns];

	// Init
	const int bRows = 2;
	const int bColumns = 3;
	// Kktina ta inicializacia
	// int matrixB[2][3] = {{1,5,2}, {3,0,5}};
	int matrixB[bRows][bColumns];

	if(aColumns != bRows) {
		printf("Pocet stlpcov prvej matice sa musi rovnat poctu riadkov druhej matice!");
		return 0;
	}

	// Populate matrices
	srand(time(NULL));
	populateMatrix(aRows, aColumns, matrixA);
	populateMatrix(bRows, bColumns, matrixB);

	printf("Matrix A\n");
	printMatrix(aRows, aColumns, matrixA);

	printf("Matrix B\n");
	printMatrix(bRows, bColumns, matrixB);


	// Init
	const int rRows = aRows;
	const int rColumns = bColumns;
	// Kktina ta inicializacia
	// int matrixResult[3][2] = {0};
	int matrixResult[rRows][rColumns];

	multiplyMatrices(
		aRows, aColumns, matrixA,
		bRows, bColumns, matrixB,
		rRows, rColumns, matrixResult
		);

	printf("Multiplied Matrix\n");
	printMatrix(rRows, rColumns, matrixResult);

	return 0;
}
