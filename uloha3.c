#include <stdio.h>
#include <mpi.h>

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
	    for(j = 0; j < bColumns; j++){
	    	// Pre-init with zero
	    	rM[i][j] = 0;
	        for(k = 0; k < aColumns; k++){
	            rM[i][j] += aM[i][k] * bM[k][j];
	        }
	    }
	}
}

/**
* Nasobenie matic
*/
int main(int argc, char *argv[]) {
	int size, rank;

	const int aRows = 3;
	const int aColumns = 3;
	int matrixA[aRows][aColumns];

	const int bRows = 3;
	const int bColumns = 3;
	int matrixB[bRows][bColumns];

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(aColumns != bRows) {
		printf("Pocet stlpcov prvej matice sa musi rovnat poctu riadkov druhej matice!\n");
		return 0;
	}

	if(size != aRows) {
		printf("Pocet procesov musi byt rovny poctu riadkov prvej matice!\n");
	}

	if(rank == 0) {
		srand(time(NULL));

		// Populate matrices
		populateMatrix(aRows, aColumns, matrixA);
		populateMatrix(bRows, bColumns, matrixB);

		printf("Matrix A\n");
		printMatrix(aRows, aColumns, matrixA);

		printf("Matrix B\n");
		printMatrix(bRows, bColumns, matrixB);

	}

	const int smRows = 1;
	const int smColumns = aColumns;
	int sMatrix[smRows][smColumns];

	// Scatter matrix A by rows
	// so everyone gets one row
	MPI_Scatter(
		matrixA, smColumns, MPI_INT,
		sMatrix, smColumns, MPI_INT,
		0, MPI_COMM_WORLD
		);

	// Broadcast matrix B to everyone
	MPI_Bcast(matrixB, bRows * bColumns, MPI_INT, 0, MPI_COMM_WORLD);

	const int mRows = smRows;
	const int mColumns = bColumns;
	int mMatrix[mRows][mColumns];

	// Multiply submatrix with matrix B
	multiplyMatrices(
		smRows, smColumns, sMatrix,
		bRows, bColumns, matrixB,
		mRows, mColumns, mMatrix
		);

	const int rRows = aRows;
	const int rColumns = bColumns;
	int resultMatrix[rRows][rColumns];

	// Pozor, tu count je pocet prijatych
	// elementov na proces, nie vsetkych spolu
	// Gather multiplied rows from everyone
	// aj join them into final multiplied matrix
	MPI_Gather(
		mMatrix, mRows * mColumns, MPI_INT,
		resultMatrix, mRows * mColumns, MPI_INT,
		0, MPI_COMM_WORLD
		);

	// Root prints the result
	if(rank == 0) {
		printf("Multiplied Matrix\n");
		printMatrix(rRows, rColumns, resultMatrix);
	}

	MPI_Finalize();

	return 0;
}
