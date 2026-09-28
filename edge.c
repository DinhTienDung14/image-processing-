#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define WIDTH 64
#define HEIGHT 64
#define NUM_DIRECTIONS 4

#define H 0
#define P 1
#define V 2
#define M 3
// Xắp xếp 40 giá trị tăng dần và tính median
float CalculateMedian(int values[40]){
    int temp;
    for (int i = 1; i < 40; i++){
        temp = values[i];
        int j = i - 1;

        while (j >= 0 && values[j] > temp){
            values[j + 1] = values[j];
            j--;
        }
        values[j + 1] = temp;
    }
    return (values[19] + values[20]) / 2.0f;
}
//ngưỡng cục bộ tại pixel
float CalculateThreshold(unsigned char image[HEIGHT][WIDTH], int x, int y){
    int differences[40];
    int count = 0;
    int difference;

    for (int n = -2; n <= 1; n++){
        for (int m = -2; m <= 2; m++){
            difference = (int)image[y + m][x + n + 1] - (int)image[y + m][x + n];
            differences[count] = abs(difference);
            count++;
        }
    }

    for (int n = -2; n <= 1; n++){
        for (int m = -2; m <= 2; m++){
            difference = (int)image[y + n + 1][x + m] - (int)image[y + n][x + m];
            differences[count] = abs(difference);
            count++;
        }
    }
    return CalculateMedian(differences) * 5.0f; //ngưỡng
}

int main()
{
    unsigned char image[HEIGHT][WIDTH];
    uint16_t response[NUM_DIRECTIONS][HEIGHT][WIDTH];
    unsigned char edge[NUM_DIRECTIONS][HEIGHT][WIDTH];
    char input_name[64];
    char output_name[64];
    char *direction[NUM_DIRECTIONS] = {"H", "P", "V", "M"};

    FILE *input;
    FILE *output;
    size_t count;

    for (int image_number = 1; image_number <= 10; image_number++){
// Đọc ảnh grayscale
        snprintf(input_name, sizeof(input_name), "Gray_raw/gray%d.raw", image_number);
        input = fopen(input_name, "rb");

        if (input == NULL) return 1;
        count = fread(image, sizeof(unsigned char), WIDTH * HEIGHT, input);
        fclose(input);

        if (count != WIDTH * HEIGHT) return 1;
// Đọc 4 file reponse
        for (int d = 0; d < NUM_DIRECTIONS; d++){
            snprintf(input_name, sizeof(input_name), "Response_raw/response%d_%s.raw", image_number, direction[d]);
            input = fopen(input_name, "rb");
            if (input == NULL) return 1;

            count = fread(response[d], sizeof(uint16_t), WIDTH * HEIGHT, input);
            fclose(input);
            if (count != WIDTH * HEIGHT) return 1;
        }
//tạo 4 edge map
        for (int y = 0; y < HEIGHT; y++){
            for (int x = 0; x < WIDTH; x++){
                if (x < 2 || x >= WIDTH - 2 ||y < 2 || y >= HEIGHT - 2){
                    for (int d = 0; d < NUM_DIRECTIONS; d++){
                        edge[d][y][x] = 0;
                    }
                    continue;
                }
//Tìm phần hồi lớn nhất
                uint16_t max_response = response[H][y][x];

                for (int d = 1; d < NUM_DIRECTIONS; d++){
                    if (response[d][y][x] > max_response){
                        max_response = response[d][y][x];
                    }
                }
                float threshold = CalculateThreshold(image, x, y); // ngưỡng cục bộ
//ngưỡng áp dụng, hướng
                for (int d = 0; d < NUM_DIRECTIONS; d++){
                    if (response[d][y][x] == max_response &&response[d][y][x] > threshold){
                        edge[d][y][x] = 1;
                    }
                    else edge[d][y][x] = 0;
                }
            }
        }
// Lưu 4 edge maps
        for (int d = 0; d < NUM_DIRECTIONS; d++){
            snprintf(output_name, sizeof(output_name), "Edge_raw/edge%d_%s.raw", image_number, direction[d]);
            output = fopen(output_name, "wb");
            if (output == NULL) return 1;

            count = fwrite(edge[d], sizeof(unsigned char), WIDTH * HEIGHT, output);
            fclose(output);
            if (count != WIDTH * HEIGHT) return 1;
        }
    }
    return 0;
}