#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <cuda_runtime.h>
#include <cuda.h>

using namespace std;
using namespace cv;


__global__
void cudaGrayScale(float *R, float *G, float *B, float* gray, int n){
    int i = blockDim.x*blockIdx.x + threadIdx.x;
    if(i < n) {
        gray[i] = static_cast<float>((R[i] * 0.21 + G[i] * 0.71 + B[i] * 0.07) / 350.0);
    }
}

void grayscale(float* R, float* G, float* B, float* grayscale, int n){
    int size = n * sizeof(float);
    float *d_R, *d_G, *d_B, *d_gray;
    cudaMalloc((void **) &d_R, size);
    cudaMemcpy(d_R, R, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_G, size);
    cudaMemcpy(d_G, G, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_B, size);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_gray, size);

    cudaGrayScale<<<ceil(n/1024.0), 1024>>>(d_R, d_G, d_B, d_gray, n);
    cudaMemcpy(grayscale, d_gray, size, cudaMemcpyDeviceToHost);

    cudaFree(d_R);
    cudaFree(d_G);
    cudaFree(d_B);
    cudaFree(d_gray);
}


int main(){
    string image_path = "ejemplo.jpg";
    Mat img = imread(image_path, IMREAD_COLOR);
    
    int filas = img.rows;
    int columnas = img.cols;

    float * R = new float[filas*columnas];
    float * G = new float[filas*columnas];
    float * B = new float[filas*columnas];

    float* GrayScaleMatrix = new float[filas * columnas];

    for(int i = 0; i < filas; ++i){
        for(int j = 0; j < columnas; ++j){
            int index = columnas * i + j;
            R[index] = (float)img.at<cv::Vec3b>(i, j)[2];
            G[index] = (float)img.at<cv::Vec3b>(i, j)[1];
            B[index] = (float)img.at<cv::Vec3b>(i, j)[0];
        }
    }

    grayscale(R, G, B, GrayScaleMatrix, columnas * filas);
    Mat gray = Mat(filas, columnas, CV_32FC1, GrayScaleMatrix);
    gray.convertTo(gray, CV_8UC3, 255.0);
    imwrite("./ejemplobyn.jpg", gray);


    return 0;
}