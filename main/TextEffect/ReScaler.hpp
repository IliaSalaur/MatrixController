#ifndef MY_RESCALER_HPP
#define MY_RESCALER_HPP

// #include <stdio.h>
// #include <cstdint>
// // #include <cmath>

// int lround(float num, float ratio)
// {
//     if(num <= 0.0)
//         return 0;
//     else return (int)(num + ratio / 2); // num + float(w) / float(sW) / 2
// }

// uint8_t B[5] = {0b1111111, 0b1001001, 0b1001001, 0b0110110, 0b0};
// uint8_t M[5] = {0x7F, 0x02, 0x1C, 0x02, 0x7F};
// uint8_t g[5] = {0x7F, 0x10, 0x28, 0x44, 0x00};

// template<uint8_t w, uint8_t h>
// void printScaled(uint8_t let[w], uint8_t sW, uint8_t sH){
//     // uint8_t xInt = 0;
//     // uint8_t yInt = 0;
//     // printf("magic number: %f\n", (w / sW) - (float(w) / float(sW)));
//     // for(float y = (h / sH) - float(h) / float(sH); yInt < sH; y += float(h) / float(sH), yInt++)
//     // {
//     //     xInt = 0;
//     //     for(float x = (w / sW) - (float(w) / float(sW)) * 2; xInt < sW; x += float(w) / float(sW), xInt++)
//     //     {
//     //         printf("%s", let[lround(x)] & (1 << lround(y)) ? "#" : "|");
//     //     }
//     //     printf("\n");
//     // }
//     float ratio = float(w) / float(sW);
//     uint8_t xInt = 0;
//     uint8_t yInt = 0;
//     // float y = 0, x = -pow(float(w) / float(sW), 2);
//     float y = 0, x = 0;
    
    
//     printf("Ratio x: %f, Ratio y: %f\n", float(w) / float(sW), float(h) / float(sH));
//     // printf("magic number: %f\n", (w / sW) - (float(w) / float(sW)));
//     for(; yInt < sH; y += float(h) / float(sH), yInt++)
//     {
//         xInt = 0;
//         // x = -pow(float(w) / float(sW), 2);
//         x = 0;
//         for(; xInt < sW; x += float(w) / float(sW), xInt++)
//         {
//             printf("%s", let[lround(x, ratio)] & (1 << lround(y, ratio)) ? "#" : "|");
//             // printf("%f, %f\n", x, y);
//         }
//         printf("\n");
//     }
    
//     printf("%f, %f\n", x, y);
    
// }

// int main()
// {
//     // for(uint8_t y = 0; y < 7; y++)
//     // {
//     //     for(uint8_t x = 0; x < 5; x++)
//     //     {
//     //         printf("%s", B[x] & (1 << y) ? "#" : "|");
//     //     }
//     //     printf("\n");
//     // }
    
//     uint8_t sW = 11;
//     uint8_t sH = 15;
    
//     printScaled<5, 7>(B, 5, 7);
//     printf("\n");
//     printScaled<5, 7>(B, sW, sH);

//     printf("\n");
//     printScaled<5, 7>(M, 5, 7);
//     printf("\n");
//     printScaled<5, 7>(M, sW, sH);
    
//     printf("\n");
//     printScaled<5, 7>(g, 5, 7);
//     printf("\n");
//     printScaled<5, 7>(g, sW, sH);
//     return 0;
// }


#endif