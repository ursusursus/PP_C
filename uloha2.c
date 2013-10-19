#include <stdio.h>
#include <mpi.h>

void printMatrix(int mRows, int mColumns, int m[mRows][mColumns]) {
  printf("printMatrix\n");
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
  // Init random
  srand(time(NULL));

  int row, column;
  for(row = 0; row < mRows; row++) {
    for(column = 0; column < mColumns; column++) {
      m[row][column] = rand() % 10;
    }
  }
}

void transposeMatrix(int rows, int columns, int m[rows][columns], int tM[columns][rows]) {
  int row, column;
  for(row = 0; row < rows; row++) {
    for(column = 0; column < columns; column++) {
      tM[column][row] = m[row][column];
    }
  }
}

void divideMatrix(int rows, int columns, int m[rows][columns], int fM[rows / 2][columns], int sM[rows / 2][columns]) {
  int i, j;
  int rowsHalf = rows / 2;
  for(i = 0; i < rows; i++) {
    for(j = 0; j < columns; j++) { 
      if (i < rowsHalf) {
        fM[i][j] = m[i][j];
      } else {
        sM[i % rowsHalf][j] = m[i][j];
      }
    }
  }
}

void joinMatrices(int rows, int columns, int joinedMatrix[rows][columns * 2], int fM[rows][columns], int sM[rows][columns]) {
  int columnsHalf = columns;

  int i, j;
  for(i = 0; i < rows; i++) {
    for(j = 0; j < columns * 2; j++) { 
      if (j < columnsHalf) {
        joinedMatrix[i][j] = fM[i][j];
      } else {
        joinedMatrix[i][j] = sM[i][j % columnsHalf];
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int size, rank;
  int tag = 1234;
  MPI_Status status;

  const int rows = 4;
  const int columns = 2;
  int matrix[rows][columns];

  const int tRows = columns;
  const int tColumns = rows;

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(size != 3) {
    printf("Su potrebne prave 3 procesy\n");
    return 0;
  }

  if(rows % 2 != 0) {
    printf("Matica musi mat parny pocet riadkov\n");
    return 0;
  }

  if(rank == 0) {
    // Generate matrix
    populateMatrix(rows, columns, matrix);

    // Print matrix
    printMatrix(rows, columns, matrix);

    int firstHalf[rows / 2][columns];
    int secondHalf[rows / 2][columns];
    divideMatrix(rows, columns, matrix, firstHalf, secondHalf);

    MPI_Send(firstHalf, (rows / 2) * columns, MPI_INT, 1, tag, MPI_COMM_WORLD);
    MPI_Send(secondHalf, (rows / 2) * columns, MPI_INT, 2, tag, MPI_COMM_WORLD);

    int tFirstHalf[tRows][tColumns / 2];
    int tSecondHalf[tRows][tColumns / 2];
    MPI_Recv(tFirstHalf, tRows * tColumns / 2, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
    MPI_Recv(tSecondHalf, tRows * tColumns / 2, MPI_INT, 2, tag, MPI_COMM_WORLD, &status);

    int tMatrix[tRows][tColumns];
    joinMatrices(tRows, tColumns / 2, tMatrix, tFirstHalf, tSecondHalf);
    printMatrix(tRows, tColumns, tMatrix);

  } else {
    int sRows = rows / 2;
    int sColumns = 2;
    int subMatrix[sRows][sColumns];
    MPI_Recv(subMatrix, sRows * sColumns, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

    // Transpose matrix
    int subTMatrix[sColumns][sRows];
    transposeMatrix(sRows, sColumns, subMatrix, subTMatrix);

    MPI_Send(subTMatrix, sRows * sColumns, MPI_INT, 0, tag, MPI_COMM_WORLD);
  }

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}

