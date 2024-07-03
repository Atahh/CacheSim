/* 
 * Atah Omar
 * 101865123 
 *
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans_64(int M,int N, int A[N][M], int B[M][N]);
void trans_61(int M,int N, int A[N][M], int B[M][N]);
void trans_32(int M,int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  //block size for 32x32 = 8
  //block size for 64x64 = 8 for outside rows and cols and 4 for inner
  //block size for 61x67 = 16

    if (N == 32 && M == 32) {
      trans_32(M,N,A,B);
    }
    else if (N == 64 && M == 64)
      {
	trans_64(M,N,A,B);
      }
    else // 61x67 matrix
      {
	trans_61(M,N,A,B);
      }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

void trans_61(int M,int N,int A[N][M],int B[M][N])
{
  //temp var
  int temp1,temp2;
  int bSize = 16;//block size
  
  for(int col = 0;col<M;col+=bSize)
    {
      for(int row = 0;row<N;row+=bSize)
	{
	  for(int i = row;(i<N) && (i<row+bSize);i++)
	    {
	      for(int j = col;(j<M) && (j<col+bSize);j++)
		{
		  if(i != j)
		    {
		      B[j][i] = A[i][j];
		    }
		  else
		    {
		      temp1 = A[i][j];
		      temp2 = i;
		    }
		}
	      if(row == col)
		{
		  B[temp2][temp2] = temp1;
		}
	    }
	}
    }
}

void trans_32(int M,int N,int A[N][M],int B[M][N])
{
  //temp vars
  int temp1,temp2;
  int bSize = 8;//block size

  for(int col = 0;col<M;col+=bSize)
    {
      for(int row = 0;row<N;row+=bSize)
	{
	  for(int i = row;i<row+bSize;i++)
	    {
	      for(int j = col;j<col+bSize;j++)
		{
		  if(i != j)
		    {
		      B[j][i] = A[i][j];//set B col and row to A row and col
		    }
		  else
		    {
		      temp1 = A[i][j];
		      temp2 = i;
		    }
		}
	      if( row == col)
		{
		  B[temp2][temp2] = temp1;
		}
	    }
	}
    }
}
void trans_64(int M,int N,int A[N][M],int B[M][N])
{
  //temp vars
  int temp1, temp2, temp3,temp4,temp5,temp6, temp7, temp8;
  int bSize = 8;
  int bSizeSmall = 4;

  for (int i = 0; i < N; i += bSize)
    {
      for (int j = 0; j < M; j += bSize)
	{
	  for (int k = i; k < i + bSizeSmall; k++)
	    {
	      //loop unrolling
	      //assign the temp vars to each of the values
	      //of each col in a row
	      temp1 = A[k][j + 0];
	      temp2 = A[k][j + 1];
	      temp3 = A[k][j + 2];
	      temp4 = A[k][j + 3];
	      temp5 = A[k][j + 4];
	      temp6 = A[k][j + 5];
	      temp7 = A[k][j + 6];
	      temp8 = A[k][j + 7];
	      //set the first 4 cols and rows of B from the temps
	      B[j + 0][k] = temp1;
	      B[j + 1][k] = temp2;
	      B[j + 2][k] = temp3;
	      B[j + 3][k] = temp4;
	      //set the first 4 cols and 4th row to the last 4 temps
	      B[j + 0][k + 4] = temp5;
	      B[j + 1][k + 4] = temp6;
	      B[j + 2][k + 4] = temp7;
	      B[j + 3][k + 4] = temp8;
	    }
	  //smaller block with a block size of 4
	  for (int k = j; k < j + bSizeSmall; k++)
	    {
	      temp1 = B[k][i+4];
	      temp2 = B[k][i+5];
	      temp3 = B[k][i+6];
	      temp4 = B[k][i+7];
	      
	      temp5 = A[i + 4][k];
	      temp6 = A[i + 5][k];
	      temp7 = A[i + 6][k];
	      temp8 = A[i + 7][k];

	      B[k][i + 4] = temp5;
	      B[k][i + 5] = temp6;
	      B[k][i + 6] = temp7;
	      B[k][i + 7] = temp8;
	      
	      B[k + 4][i + 0] = temp1;
	      B[k + 4][i + 1] = temp2;
	      B[k + 4][i + 2] = temp3;
	      B[k + 4][i + 3] = temp4;

	      for (int l = 0; l < bSizeSmall; l++)
	        {
		  B[k + bSizeSmall][i + l + bSizeSmall] = A[i + l + bSizeSmall][k + bSizeSmall];
		}
	      }
	  }
     }
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

