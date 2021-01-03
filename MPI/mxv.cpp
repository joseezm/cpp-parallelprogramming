// mpic++ -g -Wall -o mxv mxv.cpp
// mpiexec -n 1 --oversubscribe ./mpi^C

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <iostream>
using namespace std;

#define SIZE 5000

void printVector(int * a, int n){
    cout << "\n[";
    for(int i=0; i<n; i++){
        printf("%d ",a[i]);
    }
    cout << ("]\n");
}

void Read_vector(int local_a[], int local_n, int n, char vec_name[], int my_rank, MPI_Comm comm){
  int* a = NULL;
  int i;

  srand(time(NULL));
  if(my_rank == 0){
    a = new int[n];
    // printf("Enter the vector %s\n", vec_name);
    for(i = 0; i < n; i++)
      a[i] = rand() % 100;
     MPI_Scatter(a, local_n, MPI_INT, local_a, local_n, MPI_INT,0,comm);
     free(a);
    
    }
    else{
      MPI_Scatter(a,local_n,MPI_INT,local_a,local_n,MPI_INT,0,comm);
    }
  

}

void Print_vector(int local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm){
  int* b = NULL;
  int i;
  if(my_rank == 0){
    b = new int[n];
    MPI_Gather(local_b, local_n, MPI_INT,b,local_n, MPI_INT,0,comm);
    // printf("%s\n", title);
    // for(i = 0; i < n; i++){
    //   printf("%d ", b[i]);
    // }
    //  printf("OK\n");
    free(b);
  }
  else{
    MPI_Gather(local_b, local_n, MPI_INT, b, local_n, MPI_INT, 0, comm);
  }
}

void Mat_vect_mult(int local_A[], int local_x[], int local_y[], int local_m, int n, int local_n, MPI_Comm comm){
  int* x;
  int local_i,j;
  x = new int[n];
  MPI_Allgather(local_x, local_n, MPI_INT, x, local_n, MPI_INT, comm);
  for(local_i = 0; local_i < local_m; local_i++){
    local_y[local_i] = 0.0;
    for(j = 0; j <n; j++){
      
      local_y[local_i] += local_A[local_i*n+j] * x[j];
    }

  }
  free(x);
}

int main(){
  int comm_sz;
  int my_rank;
  double start,finish,t;

  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  int local_n = SIZE/comm_sz;
  //printf("-%d\n", local_n);

  int * local_A = new int[SIZE*SIZE/comm_sz];
  int * local_x = new int[SIZE/comm_sz];
  int * local_y = new int[SIZE/comm_sz];

  Read_vector(local_A, SIZE*SIZE/comm_sz, SIZE*SIZE, "A", 0, MPI_COMM_WORLD );
  Read_vector(local_x, SIZE/comm_sz, SIZE, "A", 0, MPI_COMM_WORLD );
//   printVector(local_A,SIZE*SIZE/comm_sz);
//   cout << "-";
//   printVector(local_x,SIZE/comm_sz);

  start= MPI_Wtime();
  Mat_vect_mult(local_A, local_x, local_y, local_n, SIZE, local_n, MPI_COMM_WORLD);
  Print_vector(local_y,local_n, SIZE, "Y", my_rank, MPI_COMM_WORLD);
  finish = MPI_Wtime();
  t = finish-start;

  double tMax;
  MPI_Reduce(&t, &tMax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if(my_rank == 0){
    printf("\nTIEMPO: %f\n",tMax);
  }

  MPI_Finalize();
  return 0;
}
