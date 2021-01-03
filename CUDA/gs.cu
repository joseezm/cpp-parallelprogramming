#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <cuda_runtime.h>
#include <cuda.h>

#define BLUR_SIZE 1

using namespace std;
using namespace cv;


__global__ void blanco_negro(unsigned char* Image, int Channels){
	int x = blockIdx.x;
	int y = blockIdx.y;
	int idx = (x + y * gridDim.x) * Channels;

	for (int i = 0; i < Channels; i++){
        if(i==0){
            Image[idx + i] = Image[idx + i] * 0.07f/350;
        }
        else if(i==1){
            Image[idx + i] = Image[idx + i] * 0.71f/350;
        }
        else if(i==2){
            Image[idx + i] = Image[idx + i] * 0.21f/350;
        }
		
	}
}

void Image_blanco_negro(unsigned char* Input_Image, int Height, int Width, int Channels){
	unsigned char* Dev_Input_Image = NULL;

	cudaMalloc((void**)&Dev_Input_Image, Height * Width * Channels);

	cudaMemcpy(Dev_Input_Image, Input_Image, Height * Width * Channels, cudaMemcpyHostToDevice);

	dim3 Grid_Image(Width, Height);
	blanco_negro << <Grid_Image, 1024 >> >(Dev_Input_Image, Channels);

	cudaMemcpy(Input_Image, Dev_Input_Image, Height * Width * Channels, cudaMemcpyDeviceToHost);

	//free gpu mempry
	cudaFree(Dev_Input_Image);
}



int main(){
	Mat Input_Image = imread("goku.jpg");

	cout << "Height: " << Input_Image.rows << ", Width: " << Input_Image.rows << ", Channels: " << Input_Image.channels() << endl;

	Image_blanco_negro(Input_Image.data, Input_Image.rows, Input_Image.rows, Input_Image.channels());

	imwrite("blancoynegro.jpg", Input_Image);
	system("pause");
	return 0;
}