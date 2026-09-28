#include <stdio.h>

#define WIDTH 64
#define HEIGHT 64
#define CELL_SIZE 16
#define NUM_CELLS 16
#define NUM_DIRECTIONS 4
#define FEATURE_SIZE 64

#define H 0
#define P 1
#define V 2
#define M 3

int main()
{
    unsigned char edge[NUM_DIRECTIONS][HEIGHT][WIDTH];
    int feature[FEATURE_SIZE];
    char input_name[64];
    char *direction[NUM_DIRECTIONS] = {"H", "P", "V", "M"};

    FILE *input;
    FILE *output;
    size_t count;

    output = fopen("APED_features.csv", "w");

    if (output == NULL) return 1;

    fprintf(output, "Image");

    for (int d = 0; d < NUM_DIRECTIONS; d++){
        for (int a = 0; a < 4; a++){
            for (int b = 0; b < 4; b++){
                fprintf(output, ",%s_%d_%d", direction[d], a, b);
            }
        }
    }
    fprintf(output, "\n");

    for (int image_number = 1; image_number <= 10; image_number++)
    {
        for (int d = 0; d < NUM_DIRECTIONS; d++){
            snprintf(input_name, sizeof(input_name), "Edge_raw/edge%d_%s.raw", image_number, direction[d]);
            input = fopen(input_name, "rb");
            if (input == NULL){
                printf("Khong the mo %s\n", input_name);
                fclose(output);
                return 1;
            }
            count = fread(edge[d], sizeof(unsigned char), WIDTH * HEIGHT, input);
            fclose(input);
            if (count != WIDTH * HEIGHT){
                fclose(output);
                return 1;
            }
        }
        int feature_index = 0;
        for (int d = 0; d < NUM_DIRECTIONS; d++){
            for (int a = 0; a < 4; a++){
                for (int b = 0; b < 4; b++){
                    int sum = 0;
                    for (int y = a * CELL_SIZE; y < (a + 1) * CELL_SIZE; y++){
                        for (int x = b * CELL_SIZE; x < (b + 1) * CELL_SIZE; x++){
                            sum += edge[d][y][x];
                        }
                    }
                    feature[feature_index] = sum;
                    feature_index++;
                }
            }
        }
        fprintf(output, "%d", image_number);
        for (int i = 0; i < FEATURE_SIZE; i++){
            fprintf(output, ",%d", feature[i]);
        }
        fprintf(output, "\n");
    }
    fclose(output);
    return 0;
}