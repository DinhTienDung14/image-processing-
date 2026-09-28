#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_IMAGES 10
#define FEATURE_SIZE 64

int features[NUM_IMAGES][FEATURE_SIZE];
int image_id[NUM_IMAGES];

int ManhattanDistance(int X[FEATURE_SIZE],int T[FEATURE_SIZE]){
    int distance = 0;
    for (int i = 0; i < FEATURE_SIZE; i++){
        distance += abs(X[i] - T[i]);
    }
    return distance;
}

int ReadFeatures(const char *filename)
{
    FILE *input = fopen(filename, "r");
    if (input == NULL){
        printf("Cannot open %s\n", filename);
        return 0;
    }
    char line[2048];
    if (fgets(line, sizeof(line), input) == NULL)
    {
        fclose(input);
        return 0;
    }
    int row = 0;
    while (row < NUM_IMAGES &&fgets(line, sizeof(line), input) != NULL){
        char *token;
        token = strtok(line, ",\r\n");
        if (token == NULL){
            fclose(input);
            return 0;
        }
        image_id[row] = atoi(token);
        for (int i = 0; i < FEATURE_SIZE; i++){
            token = strtok(NULL, ",\r\n");
            if (token == NULL){
                printf("Invalid feature data at row %d\n",row + 1);
                fclose(input);
                return 0;
            }
            features[row][i] = atoi(token);
        }
        row++;
    }
    fclose(input);
    if (row != NUM_IMAGES){
        printf("Expected %d images, but read %d\n",NUM_IMAGES, row);
        return 0;
    }
    return 1;
}

int main()
{
    int distance[NUM_IMAGES][NUM_IMAGES];

    if (!ReadFeatures("APED_features.csv"))
    {
        printf("Failed to read feature vectors.\n");
        return 1;
    }
    for (int i = 0; i < NUM_IMAGES; i++){
        for (int j = 0; j < NUM_IMAGES; j++){
            if (i == j){
                distance[i][j] = 0;
            }
            else{
                distance[i][j] = ManhattanDistance(features[i], features[j]);
            }
        }
    }
    printf("\nManhattan Distance Matrix\n\n");
    printf("%8s", "");
    for (int j = 0; j < NUM_IMAGES; j++){
        printf("%8d", image_id[j]);
    }
    printf("\n");
    for (int i = 0; i < NUM_IMAGES; i++){
        printf("%8d", image_id[i]);
        for (int j = 0; j < NUM_IMAGES; j++){
            printf("%8d", distance[i][j]);
        }
        printf("\n");
    }
    printf("\nNearest face image for each image\n\n");
    for (int i = 0; i < NUM_IMAGES; i++){
        int min_distance = -1;
        int nearest_index = -1;

        for (int j = 0; j < NUM_IMAGES; j++){
            if (i == j) continue;
            if (min_distance == -1 ||distance[i][j] < min_distance){
                min_distance = distance[i][j];
                nearest_index = j;
            }
        }
        printf("Image %d -> Image %d, distance = %d\n",image_id[i],image_id[nearest_index],min_distance);
    }
    return 0;
}