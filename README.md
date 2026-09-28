# image-processing-
chuyển ảnh RGB sang grayscale 64x64-> edge detect -> vector 64 chiều -> so sánh khoảng cách Manhattan

ĐẠI HỌC QUỐC GIA THÀNH PHỐ HỒ CHÍ MINH
ĐẠI HỌC KHOA HỌC TỰ NHIÊN
----------*----------
KHOA ĐIỆN TỬ VIỄN THÔNG
CHUYÊN NGÀNH MÁY TÍNH – HỆ THỐNG NHÚNG


BÀI TẬP 2

Môn: Nhập môn Xử lý ảnh và Video


Họ và Tên	MSSV
Đinh Tiến Dũng	23200008













BÀI TẬP: Lấy 10 khuôn mặt 64x64 -> chuyển sang grayscale -> edge detect-> Tạo 10 vector 64 chiều. So sánh khoảng cách Manhattan.
 
Ảnh gốc
1. Chuyển ảnh thông thường thành ảnh grayscale 64x64
- Sử dụng python (file convertRGBraw.py)
from PIL import Image

import os

os.makedirs("raw", exist_ok=True)
for i in range(1,11):
    img = Image.open(f"anhgoc/{i}.jpg")
    img = img.convert("RGB")
    img = img.resize((64,64))
    with open(f"RGB_raw/{i}.raw", "wb") as f:
        f.write(img.tobytes())
- Chuyển ảnh RGB thành grayscale sử dụng công thức: (file grayscale.c)
Gray=0.299R+0.587G+0.114B
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
- Ảnh trước và sau chuyển:
 
	

Ảnh gốc	Ảnh grayscale 64x64

2. Phát hiện 4 cạnh theo hướng 
2.1 Các kernel phát triển cạnh (file reponse_raw.c)
- K_H: Horizontol – Hướng ngang
int KH[5][5] = {
    { 0,  0,  0,  0,  0},
    { 1,  1,  1,  1,  1},
    { 0,  0,  0,  0,  0},
    {-1, -1, -1, -1, -1},
    { 0,  0,  0,  0,  0}
};
- K_P: + 45°
int KP[5][5] = {
    { 0,  0,  0,  1,  0},
    { 0,  1,  1,  0, -1},
    { 0,  1,  0, -1,  0},
    { 1,  0, -1, -1,  0},
    { 0, -1,  0,  0,  0}
};

- K_V: Vertical - Hướng dọc
int KV[5][5] = {
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  0, -1,  0}
};
- K_M: - 45°
int KM[5][5] = {
    { 0, -1,  0,  0,  0},
    { 1,  0, -1, -1,  0},
    { 0,  1,  0, -1,  0},
    { 0,  1,  1,  0, -1},
    { 0,  0,  0,  1,  0}
};
2.2 Tính phản hồi của kernel theo 4 hướng (file reponse_raw.c)
- Tại mỗi pixel, thực hiện phép nhân từng phần tử của kernel với pixel tương ứng trong vùng lân cận 5×5, sau đó cộng các kết quả để thu được phản hồi theo hướng tương ứng.
 
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int d = 0; d < NUM_KERNELS; d++){
// Tính phản hồi của kernel trên toàn bộ ảnh
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
// ghi dữ liệu phản hồi vào file RAW	
            count = fwrite(response[d], sizeof(uint16_t),WIDTH * HEIGHT, output);
            fclose(output);
            if (count != WIDTH * HEIGHT) return 1;
        }
2.3 Tạo 4 bản đồ cạnh (file edge.c)
2.3.a Tính trung vị median và ngưỡng cục bộ 
 
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
    return CalculateMedian(differences) * 5.0f; 
}
2.3.b Chọn hướng có phản hồi lớn nhất
- Tại mỗi pixel có 4 phản hồi
 
- Tìm giá trị lớn nhất, hướng có giá trị lớn nhất được đánh dấu bằng 1, các hướng còn lại bằng 0.
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
- Ảnh edge map theo 4 hướng:
 	 	 	 
Edge_H	Edge_P	Edge_V	Edge_M

3. Trích xuất vector đặc trưng APED (file createVector64.c)
- Đọc 4 file edge map của từng ảnh, chia mỗi edge map thành 16 vùng, đếm số pixel cạnh của từng vùng rồi ghép thành vector 64 chiều. 
for (int image_number = 1; image_number <= 10; image_number++){
// đọc 4 map edge
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
// Tính 16 đặc trưng cho mỗi hướng
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
- Vector được ghi vào file APED_features.csv
 
- Mỗi hàng là 1 vector
4. Tính khoảng cách nhỏ nhất và các vị trí tương ứng (file compare.c)
4.1 Khoảng cách Manhattan
- Hai vector đặc trưng X, T có khoảng cách Manhattan được tính theo công thức:
 
int ManhattanDistance(int X[FEATURE_SIZE],int T[FEATURE_SIZE]){
    int distance = 0;
    for (int i = 0; i < FEATURE_SIZE; i++){
        distance += abs(X[i] - T[i]);
    }
    return distance;
}
- Để tìm khoảng cách Manhattan nhỏ nhất, ta đi so sánh từng cặp ảnh với nhau theo công thức: 
 
- Khoảng cách của cặp ảnh
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
- Khoảng cách của các ảnh với nhau
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


- Tìm ảnh gần nhất với mỗi ảnh
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
    }
- Kết quả:
 





   
