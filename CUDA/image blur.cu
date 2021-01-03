#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <cuda_runtime.h>
#include <cuda.h>

#define BLUR_SIZE 1

using namespace std;
using namespace cv;


__global__
void blurKernel (unsigned char * in, unsigned char * out, int w, int h){
    int Col = blockIdx.x * blockDim.x + threadIdx.x;
    int Row = blockIdx.y * blockDim.y + threadIdx.y;
    

    if(Col < w && Row < h){
        int pixVal = 0;
        int pixels = 0;

        for(int blurRow = -BLUR_SIZE; blurRow < BLUR_SIZE+1; ++blurRow){
            for(int blurCol = -BLUR_SIZE; blurCol < BLUR_SIZE+1; ++blurCol){
                int curRow = Row + blurRow;
                int curCol = Col + blurCol;
                if(curRow > -1 && curRow < h && curCol > -1 && curCol < w){
                    pixVal += in[curRow * w + curCol];
                    pixels++;
                }
            }
        }
        out[Row * w + Col] = (float)(pixVal/pixels);
    }

}



void blur_image(unsigned char * Input_image, int Height, int Width){
    unsigned char * Dev_Input_Image = NULL;
    unsigned char * Dev_Output_Image = NULL;
    int n = Height * Width;

    cudaMalloc((void**)&Dev_Input_Image, Height * Width);
    cudaMemcpy(Dev_Input_Image, Input_image, Height * Width, cudaMemcpyHostToDevice);

    cudaMalloc((void**)&Dev_Output_Image, Height * Width);

    dim3 Grid_Image(Width,Height);
    blurKernel << <Grid_Image,1024>> >  (Dev_Input_Image, Dev_Output_Image, Width, Height);

    cudaMemcpy(Input_image, Dev_Output_Image, Width * Height, cudaMemcpyDeviceToHost);
    cudaFree(Dev_Input_Image);
    cudaFree(Dev_Input_Image);

}

int main(){
    string image_path = "goku.jpg";
    Mat img = imread(image_path, IMREAD_COLOR);

    blur_image(img.data, img.rows, img.cols);

    imwrite("blur_image_res.jpg", img);


    return 0;
}