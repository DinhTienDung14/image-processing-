#include <stdio.h>
#include <stdint.h>

#define WIDTH 64
#define HEIGHT 64
#define KERNEL_SIZE 5
#define NUM_KERNELS 4

int KH[5][5] = {
    { 0,  0,  0,  0,  0},
    { 1,  1,  1,  1,  1},
    { 0,  0,  0,  0,  0},
    {-1, -1, -1, -1, -1},
    { 0,  0,  0,  0,  0}
};

int KP[5][5] = {
    { 0,  0,  0,  1,  0},
    { 0,  1,  1,  0, -1},
    { 0,  1,  0, -1,  0},
    { 1,  0, -1, -1,  0},
    { 0, -1,  0,  0,  0}
};

int KV[5][5] = {
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0}
};

int KM[5][5] = {
    { 0, -1,  0,  0,  0},
    { 1,  0, -1, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  1,  0, -1},
    { 0,  0,  0,  1,  0}
};

int ApplyKernel(unsigned char image[HEIGHT][WIDTH],int kernel[5][5],int x,int y){
    int sum = 0;
    for (int i = 0; i < KERNEL_SIZE; i++){
        for (int j = 0; j < KERNEL_SIZE; j++){
            int pixel_x = x + j - 2;
            int pixel_y = y + i - 2;
            sum += kernel[i][j] * image[pixel_y][pixel_x];
        }
    }
    if (sum < 0) sum = -sum;
    return sum;
}

int main(){
    unsigned char image[HEIGHT][WIDTH];

    uint16_t response[NUM_KERNELS][HEIGHT][WIDTH];

    int (*kernels[NUM_KERNELS])[5] = { KH, KP, KV, KM };

    char input_name[32];
    char output_name[64];

    char *direction[NUM_KERNELS] = {"H", "P", "V", "M"};

    for (int n = 1; n <= 10; n++)
    {
        snprintf(input_name, sizeof(input_name), "Gray_raw/gray%d.raw", n);
        FILE *input = fopen(input_name, "rb");

        if (input == NULL) return 1;

        size_t count = fread( image, sizeof(unsigned char), WIDTH * HEIGHT, input);

        fclose(input);

        if (count != WIDTH * HEIGHT)return 1;

        for (int d = 0; d < NUM_KERNELS; d++){
            for (int y = 0; y < HEIGHT; y++){
                for (int x = 0; x < WIDTH; x++){
                    if (x < 2 || x >= WIDTH - 2 ||y < 2 || y >= HEIGHT - 2) response[d][y][x] = 0;
                    else{
                        response[d][y][x] = (uint16_t)ApplyKernel(image, kernels[d], x, y);
                    }
                }
            }
            snprintf(output_name, sizeof(output_name), "Response_raw/response%d_%s.raw", n, direction[d]);

            FILE *output = fopen(output_name, "wb");
            if (output == NULL) return 1;
            count = fwrite(response[d], sizeof(uint16_t),WIDTH * HEIGHT, output);

            fclose(output);

            if (count != WIDTH * HEIGHT) return 1;
        }
    }
    return 0;
}