#include <iostream>
#include <stdlib.h>
#include <ctime>

#include <cuda_runtime.h>
#include <cuda.h>

using namespace std;


__global__
void vecAddKernel(float * A, float *B, float *C, int n){
    int i = blockDim.x*blockIdx.x + threadIdx.x;
    if(i<n) C[i] = A[i] + B[i];

}


void vecAdd(float * A, float *B, float *C, int n){
	int size = n * sizeof(float);
    float *d_A, *d_B, *d_C;

    cudaMalloc((void**) &d_A, size);
    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMalloc((void**) &d_B, size);
    cudaMemcpy(d_B,B,size, cudaMemcpyHostToDevice);

    cudaMalloc((void**) &d_C, size);

    dim3 dimGrid(ceil(n/256.0),1,1);
    dim3 dimBlock(256,1,1);
    vecAddKernel<<<dimGrid,dimBlock >>> (d_A, d_B, d_C, n);

    cudaMemcpy(C,d_C,size, cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

}

int main(){
	
    float * A = new float[1000];
    float * B = new float[1000];
    float * C = new float[1000];

    for(int i = 0; i<1000; i++){
        A[i] = i;
        B[i] = i;
        C[i] = 1;
    }

    vecAdd(A,B,C,1000);

    for(int i = 0; i<20; i++)
        cout << C[i] << endl;

	return 0;
}