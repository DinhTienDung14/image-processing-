#include <stdio.h>
#include <stdint.h>

#define WIDTH 64
#define HEIGHT 64

int main()
{
    FILE *input;
    FILE *output;

    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char gray;

    float gray_value;

    char input_name[32];
    char output_name[32];

    for (int i = 1; i <= 10; i++){
        snprintf(input_name, sizeof(input_name), "RGB_raw/%d.raw", i);
        snprintf(output_name, sizeof(output_name), "Gray_raw/gray%d.raw", i);
        input = fopen(input_name, "rb");
        if (input == NULL) return 1;

        output = fopen(output_name, "wb");
        if (output == NULL)
        {
            fclose(input);
            return 1;
        }
        for (int j = 0; j < HEIGHT; j++){
            for (int k = 0; k < WIDTH; k++){
                fread(&R, sizeof(unsigned char), 1, input);
                fread(&G, sizeof(unsigned char), 1, input);
                fread(&B, sizeof(unsigned char), 1, input);

                gray_value = 0.299* R + 0.587 * G + 0.114 * B;
                gray = (unsigned char)(gray_value + 0.5f);
                fwrite(&gray, sizeof(unsigned char), 1, output);
            }
        }
        fclose(input);
        fclose(output);
    }
    return 0;
}