// LibLink.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "SDL.h"
#include "png.h"


struct Color
{
	uint8_t r, g, b, a;
};

struct ImageData
{
	size_t width;
	size_t height;
	size_t chanel;
	size_t sizeofRow;
	uint8_t* pixel;
};

struct Point2
{
	float x;
	float y;
};

struct Triangle
{
	Point2 A, B, C;
};

struct TriangleFactor
{
	float alpha, beta, gama;
};


void png_error_callback(png_structp pngPtr, png_const_charp error)
{
	printf("png_error_callback : %s", error);
}

void png_warning_callback(png_structp pngPtr, png_const_charp warning)
{
	printf("png_error_callback : %s", warning);
}

void SDLDraw(const void* pixel, int w, int h, int _sizeOfRow, Uint32 depth)
{
	int res = SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* win = SDL_CreateWindow("window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, 0);
	SDL_Texture* tex = SDL_CreateTexture(renderer, depth, SDL_TEXTUREACCESS_STREAMING, w, h);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_UpdateTexture(tex, NULL, pixel, _sizeOfRow);
	SDL_RenderCopy(renderer, tex, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(20000);
	// 销毁纹理、渲染器和窗口
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	// 退出SDL
	SDL_Quit();
}

void SDLDrawRect()
{
	SDL_Window* window = SDL_CreateWindow(
		"SDL Draw Rectangle",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN
	);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Rect rect = { 100, 100, 320, 240 };
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderDrawRect(renderer, &rect);
	SDL_RenderPresent(renderer);
	SDL_Delay(2000); // 等待2秒
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void checkIsPNG()
{
	const size_t bytesToCheck = 4;
	unsigned char buf[bytesToCheck] = { 0 };

	FILE* fp = fopen("1.png", "rb");
	if (!fp)
	{
		printf("read failed!");
		return;
	}
	size_t sizeOfRead = fread(buf, 1, bytesToCheck, fp);
	fclose(fp);
	if (sizeOfRead != bytesToCheck)
	{
		printf("read size not match!");
		return;
	}
}

void readPNG(const char* filename, Uint32 depth)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	FILE* fp;
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("open png failed!");
	}
	/*const size_t bytesToCheck = 4;
	unsigned char buf[bytesToCheck] = { 0 };*/

	//size_t sizeOfRead = fread(buf, 1, bytesToCheck, fp);
	//fclose(fp);
	//if (sizeOfRead != bytesToCheck)return;
	//if (!png_sig_cmp(buf, 0, bytesToCheck) == 0)
	//return;

	png_voidp user_error_ptr = NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		user_error_ptr, png_error_callback, png_warning_callback);

	if (png_ptr == NULL)
	{
		fclose(fp);
		printf("pngptr == NULL!");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		printf("info_ptr == NULL!");
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);

	//png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,&interlace_type, NULL, NULL);

	const uint32_t _width = png_get_image_width(png_ptr, info_ptr);
	const uint32_t _height = png_get_image_height(png_ptr, info_ptr);
	const uint32_t _bitDepth = png_get_bit_depth(png_ptr, info_ptr);
	const uint32_t _channels = png_get_channels(png_ptr, info_ptr);
	const uint32_t _sizeOfRow = png_get_rowbytes(png_ptr, info_ptr);
	const uint8_t interlaceType = png_get_interlace_type(png_ptr, info_ptr);
	width = _width;
	height = _height;

	png_read_update_info(png_ptr, info_ptr);
	uint8_t* pixel = (uint8_t*)malloc(sizeof(uint8_t) * _sizeOfRow * height);
	png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (size_t i = 0; i < height; i++)
	{
		row_pointers[i] = NULL;
	}
	for (int row = 0; row < height; row++)
	{
		row_pointers[row] = pixel + (row * _sizeOfRow);
	}

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);

	printf("myImage.width = %d ,myImage.height = %d ,myImage.chanel = %d ,myImage.sizeofRow = %d\n", width, height, _channels, _sizeOfRow);


	SDLDraw(pixel, width, height, _sizeOfRow, depth);
}


TriangleFactor inTriangle(float x, float y, Triangle P)
{
	float a = ((P.A.y - P.B.y) * x + (P.B.x - P.A.x) * y + P.A.x * P.B.y - P.B.x * P.A.y) / ((P.A.y - P.B.y) * P.C.x + (P.B.x - P.A.x) * P.C.y + P.A.x * P.B.y - P.B.x * P.A.y);
	float b = ((P.A.y - P.C.y) * x + (P.C.x - P.A.x) * y + P.A.x * P.C.y - P.C.x * P.A.y) / ((P.A.y - P.C.y) * P.B.x + (P.C.x - P.A.x) * P.B.y + P.A.x * P.C.y - P.C.x * P.A.y);
	float c = 1 - a - b;
	TriangleFactor factor;
	factor.gama = a;
	factor.beta = b;
	factor.alpha = c;
	//return (a >= 0) && (b >= 0) && c >= 0;
	return factor;
}

void drawTriangle()
{
	ImageData image = { 200,150,3,200 * 3 };
	Point2 pA = { 10 ,20 }; //(255, 0, 0)
	Point2 pB = { 107 ,38 }; //(0, 255, 0)
	Point2 pC = { 42 ,115 };//(0, 0, 255)
	Triangle tri = { pA,pB,pC };
	uint8_t* pixel = (uint8_t*)malloc(image.sizeofRow * image.height);
	int index = 0;
	for (size_t y = 0; y < image.height; y++)
	{
		for (size_t x = 0; x < image.width; ++x)
		{
			TriangleFactor factor = inTriangle(x + 0.5, y + 0.5, tri);
			if (factor.alpha >= 0 && factor.beta >= 0 && factor.gama >= 0)
			{
				Color A = { 255,0,0,0 };
				Color B = { 255,255,255,255 };
				Color C = { 0,0,0,0 };
				//R/G/B/A = alpha*A + beta*B + gama*C
				pixel[index++] = A.r * factor.alpha + B.r * factor.beta + C.r * factor.gama;
				pixel[index++] = A.g * factor.alpha + B.g * factor.beta + C.g * factor.gama;
				pixel[index++] = A.b * factor.alpha + B.b * factor.beta + C.b * factor.gama;
			}
			else
			{
				pixel[index++] = 100;
				pixel[index++] = 100;
				pixel[index++] = 100;
			}
			/*	if ((tri.A.x == x && tri.A.y == y) || (tri.B.x == x && tri.B.y == y) || (tri.C.x == x && tri.C.y == y))
				{
					*(&pixel[index++]) = 255;
					*(&pixel[index++]) = 255;
					*(&pixel[index++]) = 255;
			}*/
		}
	}
	SDLDraw(pixel, 200, 150, image.sizeofRow, SDL_PIXELFORMAT_RGB24);

}

ImageData readUVImage(const char* filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	FILE* fp;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("open png failed!");
	}

	png_voidp user_error_ptr = NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		user_error_ptr, png_error_callback, png_warning_callback);

	if (png_ptr == NULL)
	{
		fclose(fp);
		printf("pngptr == NULL!");
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		printf("info_ptr == NULL!");
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	const uint32_t _channels = png_get_channels(png_ptr, info_ptr);
	size_t sizeOfRow = png_get_rowbytes(png_ptr, info_ptr);
	png_read_update_info(png_ptr, info_ptr);
	Uint8* pixel = (Uint8*)malloc(sizeOfRow * height);

	png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (size_t row = 0; row < height; row++)
		row_pointers[row] = NULL; /* Clear the pointer array */
	for (size_t row = 0; row < height; row++)
		row_pointers[row] = pixel + (row * sizeOfRow);

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);

	ImageData myImage;
	myImage.width = width;
	myImage.height = height;
	myImage.chanel = _channels;
	myImage.sizeofRow = sizeOfRow;
	myImage.pixel = pixel;

	printf("myImage.width = %d ,myImage.height = %d ,myImage.chanel = %d ,myImage.sizeofRow = %d\n", myImage.width, myImage.height, myImage.chanel, myImage.sizeofRow);

	return myImage;
}

Color getUVSample(Point2 uv, ImageData myImage)
{
	Color color = { 0 };
	int posx = (int)(myImage.width * (uv.x /** 0.5*/));
	int posy = (int)(myImage.height * (1 - uv.y /** 0.5*/));

	int index = (posx + 0) * myImage.chanel + (posy + 0) * myImage.sizeofRow;

	color.r = myImage.pixel[index + 1200 + 0];
	color.g = myImage.pixel[index + 1200 + 1];
	color.b = myImage.pixel[index + 1200 + 2];
	color.a = myImage.pixel[index + 1200 + 3];

	return color;
}

void drawUVTriangle()
{
	ImageData image = { 0 };
	image.width = 680;
	image.height = 460;
	image.chanel = 4;
	image.sizeofRow = image.width * image.chanel;

	Point2 pA = { 10 ,10 };
	Point2 pB = { 400 ,40 };
	Point2 pC = { 10 ,400 };

	Triangle tri = { pA,pB,pC };
	uint8_t* pixel = (uint8_t*)malloc(image.sizeofRow * image.height);


	//UV图片
	ImageData myImage = readUVImage("3.png");
	ImageData backImage = readUVImage("10.png");
	ImageData bitImage = readUVImage("4.png");
	printf("backImage.sizeofRow = %d \n", backImage.sizeofRow);

	Point2 uv = { 1,1 };
	Point2 uva = { 0,1 };
	Point2 uvb = { 1,1 };
	Point2 uvc = { 0,0 };


	for (size_t y = 0; y < image.height; y++)
	{
		for (size_t x = 0; x < image.width; x++)
		{
			TriangleFactor factor = { 0 };
			factor = inTriangle(x + 0.5, y + 0.5, tri);
			float trans = 1.0f;
			//float trans = ((float)(bitImage.pixel[image.sizeofRow * y + x * image.chanel + 3])) / (float)255;
			pixel[image.sizeofRow * y + x * image.chanel + 0] = 100 * (1 - trans) + bitImage.pixel[image.sizeofRow * y + x * image.chanel + 0] * trans;
			pixel[image.sizeofRow * y + x * image.chanel + 1] = 100 * (1 - trans) + bitImage.pixel[image.sizeofRow * y + x * image.chanel + 1] * trans;
			pixel[image.sizeofRow * y + x * image.chanel + 2] = 100 * (1 - trans) + bitImage.pixel[image.sizeofRow * y + x * image.chanel + 2] * trans;
			pixel[image.sizeofRow * y + x * image.chanel + 3] = 100 * (1 - trans) + bitImage.pixel[image.sizeofRow * y + x * image.chanel + 3] * trans;


			if ((factor.gama >= 0) && (factor.beta >= 0) && (factor.alpha >= 0))
			{
				uv.x = factor.alpha * uva.x + factor.beta * uvb.x + factor.gama * uvc.x;
				uv.y = factor.alpha * uva.y + factor.beta * uvb.y + factor.gama * uvc.y;
				Color color = getUVSample(uv, myImage);
				float transparency = ((float)(backImage.pixel[image.sizeofRow * y + x * image.chanel + 3])) / (float)255;
				//printf("transparency = %f \t", transparency);
				//左上角  画UV三角形
				pixel[image.sizeofRow * y + x * image.chanel + 0] = color.r /** (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 0] * transparency*/;
				pixel[image.sizeofRow * y + x * image.chanel + 1] = color.g /** (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 1] * transparency*/;
				pixel[image.sizeofRow * y + x * image.chanel + 2] = color.b /** (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 2] * transparency*/;
				pixel[image.sizeofRow * y + x * image.chanel + 3] = color.a /** (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 3] * transparency*/;

				//右下角
				/**(&pixel[image.sizeofRow * (image.height - y) + (image.width - x) * image.chanel + 0]) = color.r;
				*(&pixel[image.sizeofRow * (image.height - y) + (image.width - x) * image.chanel + 1]) = color.g;
				*(&pixel[image.sizeofRow * (image.height - y) + (image.width - x) * image.chanel + 2]) = color.b;
				*(&pixel[image.sizeofRow * (image.height - y) + (image.width - x) * image.chanel + 3]) = color.a;*/


				//右上角
				/** (&pixel[image.sizeofRow * y + (image.width - x) * image.chanel + 0]) = color.r;
				*(&pixel[image.sizeofRow * y + (image.width - x) * image.chanel + 1]) = color.g;
				*(&pixel[image.sizeofRow * y + (image.width - x) * image.chanel + 2]) = color.b;
				*(&pixel[image.sizeofRow * y + (image.width - x) * image.chanel + 3]) = color.a;*/

				//左下角
				/**(&pixel[image.sizeofRow * (image.height - y) + x * image.chanel + 0]) = color.r;
				*(&pixel[image.sizeofRow * (image.height - y) + x * image.chanel + 1]) = color.g;
				*(&pixel[image.sizeofRow * (image.height - y) + x * image.chanel + 2]) = color.b;
				*(&pixel[image.sizeofRow * (image.height - y) + x * image.chanel + 3]) = color.a;*/

			}

			//叠第三张图
			float transparency = ((float)(backImage.pixel[image.sizeofRow * y + x * image.chanel + 3])) / (float)255;
			pixel[image.sizeofRow * y + x * image.chanel + 0] = pixel[image.sizeofRow * y + x * image.chanel + 0] * (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 0] * transparency;
			pixel[image.sizeofRow * y + x * image.chanel + 1] = pixel[image.sizeofRow * y + x * image.chanel + 1] * (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 1] * transparency;
			pixel[image.sizeofRow * y + x * image.chanel + 2] = pixel[image.sizeofRow * y + x * image.chanel + 2] * (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 2] * transparency;
			pixel[image.sizeofRow * y + x * image.chanel + 3] = pixel[image.sizeofRow * y + x * image.chanel + 3] * (1 - transparency) + backImage.pixel[image.sizeofRow * y + x * image.chanel + 3] * transparency;

		}
	}

	SDLDraw(pixel, image.width, image.height, image.sizeofRow, SDL_PIXELFORMAT_RGBA32);
}

void drawUVTri()
{
	ImageData image = { 0 };
	image.width = 680;
	image.height = 460;
	image.chanel = 4;
	image.sizeofRow = image.width * image.chanel;

	Point2 pA = { 10 ,10 };
	Point2 pB = { 400 ,40 };
	Point2 pC = { 10 ,400 };

	Triangle tri = { pA,pB,pC };
	uint8_t* pixel = (uint8_t*)malloc(image.sizeofRow * image.height);


	//UV图片
	ImageData myImage = readUVImage("3.png");
	ImageData backImage = readUVImage("8.png");
	ImageData bitImage = readUVImage("4.png");

	Point2 uv = { 1,1 };
	Point2 uva = { 0,1 };
	Point2 uvb = { 1,1 };
	Point2 uvc = { 0,0 };

	for (size_t y = 0; y < image.height; y++)
	{
		for (size_t x = 0; x < image.width; ++x)
		{
			TriangleFactor factor = { 0 };
			factor = inTriangle(x + 0.5, y + 0.5, tri);

			if ((factor.gama >= 0) && (factor.beta >= 0) && (factor.alpha >= 0))
			{
				/*uint8_t* rp = pixel + (image.sizeofRow * y + x * image.chanel + 0);
				uint8_t* gp = pixel + (image.sizeofRow * y + x * image.chanel + 1);
				uint8_t* bp = pixel + (image.sizeofRow * y + x * image.chanel + 2);*/

				//*(&pixel[index++]) = 255 * factor.alpha;
				//*(&pixel[index++]) = 255 * factor.beta;
				//*(&pixel[index++]) = 255 * factor.gama;

				uv.x = factor.alpha * uva.x + factor.beta * uvb.x + factor.gama * uvc.x;
				uv.y = factor.alpha * uva.y + factor.beta * uvb.y + factor.gama * uvc.y;

				//Color color = getUVSample(uv, myImage);
				//*(&pixel[image.sizeofRow * y + x * image.chanel + 0]) = factor.alpha * 255 + factor.beta * 128 + factor.gama * 0;//0,0,0
				//*(&pixel[image.sizeofRow * y + x * image.chanel + 1]) = factor.alpha * 255 + factor.beta * 128 + factor.gama * 0;//255,255,255
				//*(&pixel[image.sizeofRow * y + x * image.chanel + 2]) = factor.alpha * 255 + factor.beta * 128 + factor.gama * 0;

				//*(&pixel[image.sizeofRow * y + x * image.chanel + 0]) = (uint8_t)(uv.x * 255);//255,0,0
				//*(&pixel[image.sizeofRow * y + x * image.chanel + 1]) = (uint8_t)(uv.y * 255);//0,255,0
				//*(&pixel[image.sizeofRow * y + x * image.chanel + 2]) = 0;

				Color color = getUVSample(uv, myImage);
				pixel[image.sizeofRow * y + x * image.chanel + 0] = color.r;
				pixel[image.sizeofRow * y + x * image.chanel + 1] = color.g;
				pixel[image.sizeofRow * y + x * image.chanel + 2] = color.b;
				pixel[image.sizeofRow * y + x * image.chanel + 3] = color.a;
			}
			else
			{
				/**(&pixel[index++]) = 100;
				*(&pixel[index++]) = 100;
				*(&pixel[index++]) = 100;*/
				/**(&pixel[image.sizeofRow * y + x * image.chanel + 0]) = 100;
				*(&pixel[image.sizeofRow * y + x * image.chanel + 1]) = 100;
				*(&pixel[image.sizeofRow * y + x * image.chanel + 2]) = 100;*/
			}
			/*if ((tri.A.x == x && tri.A.y == y) || (tri.B.x == x && tri.B.y == y) || (tri.C.x == x && tri.C.y == y))
			{
				*(&pixel[index++]) = 255;
				*(&pixel[index++]) = 0;
				*(&pixel[index++]) = 0;
			}*/
		}
	}

	SDLDraw(pixel, image.width, image.height, image.sizeofRow, SDL_PIXELFORMAT_RGBA32);
}


int main()
{
	//readPNG("9.png", SDL_PIXELFORMAT_RGBA32);
	//drawTriangle();
	drawUVTri();
	//drawUVTriangle();
	std::cout << "Hello World!\n";
	return 0;
}

