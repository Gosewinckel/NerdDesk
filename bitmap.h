#include <stdint.h>

//Info about the type, size and layout of a bitmap file
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} __attribute__((__packed__)) BITMAPFILEHEADER;

//Info about the dimensions and colour format of a bitmap
typedef struct {
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} __attribute__((__packed__)) BITMAPINFOHEADER;

//Holds values of intensity of red green and blue in a pixel
typedef struct {
	uint8_t rgbtBlue;
	uint8_t rgbtRed;
	uint8_t rgbtGreen;
} __attribute__((__packed__)) RGBTRIPLE;
