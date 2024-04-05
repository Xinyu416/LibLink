// LibLink.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include "LibLink.h"


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
	SDL_Delay(1000);
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

	//printf("myImage.width = %d ,myImage.height = %d ,myImage.chanel = %d ,myImage.sizeofRow = %d\n", myImage.width, myImage.height, myImage.chanel, myImage.sizeofRow);
	return myImage;
}

Color getUVSample(Point2 uv, ImageData myImage)
{
	Color color = { 0 };
	int posx = (int)(myImage.width * (uv.x /** 0.5*/));
	int posy = (int)(myImage.height * (1 - uv.y /** 0.5*/));

	int index = (posx + 0) * myImage.chanel + (posy + 0) * myImage.sizeofRow;

	color.r = myImage.pixel[index + 0];
	color.g = myImage.pixel[index + 1];
	color.b = myImage.pixel[index + 2];
	color.a = myImage.pixel[index + 3];

	return color;
}

Color getUVRotateSample(Point2 uv, ImageData myImage)
{
	Color color = { 0 };

	if ((uv.x < 0) || (uv.x > 1) || (uv.y < 0) || (uv.y > 1))
	{
		return color = { 255,255,255,255 };
	}

	int posx = (int)(myImage.width * (uv.x /** 0.5*/));
	int posy = (int)(myImage.height * (1 - uv.y /** 0.5*/));

	int index = (posx + 0) * myImage.chanel + (posy + 0) * myImage.sizeofRow;

	color.r = myImage.pixel[index + 0];
	color.g = myImage.pixel[index + 1];
	color.b = myImage.pixel[index + 2];
	color.a = myImage.pixel[index + 3];

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


Point2 setMoveOrigin(Point2 pA, Point2 pB, Point2 pC)
{
	Point2 O;
	O.x = (pA.x + pB.x) * 0.5f;
	O.y = (pA.y + pC.y) * 0.5f;
	//O.x = pC.x+1;
	//O.y = pC.y+1;
	//printf("O = (%f,%f)\n", O.x, O.y);
	return O;
}


Triangle rotateTri(float angle, Point2 pA, Point2 pB, Point2 pC)
{
	//三个点当前角度
	const double PI = acos(-1);
	//printf("sin(30) = %f \n", sin(3.14 * 30 / 180.0));
	printf("PI = %f \n", M_PI);
	//R 2 D 
	printf("atan(1) = %f\n", atan(1) * R2D);

	float radianA, radianB, radianC;
	radianA = atan(pA.y / pA.x);
	radianB = atan(pB.y / pB.x);
	radianC = atan(pC.y / pC.x);

	//三个点的半径 勾股定理
	float radiusA, radiusB, radiusC;
	radiusA = sqrt((pA.x * pA.x) + (pA.y * pA.y));
	radiusB = sqrt((pB.x * pB.x) + (pB.y * pB.y));
	radiusC = sqrt((pC.x * pC.x) + (pC.y * pC.y));
	printf("A 半径 = %f \n", radiusA);
	printf("B 半径 = %f \n", radiusB);
	printf("C 半径 = %f \n", radiusC);

	//旋转一定角度后 新的三个点位置
	float radian = angle * D2R;
	float new_pA_x, new_pA_y;
	float new_pB_x, new_pB_y;
	float new_pC_x, new_pC_y;

	//y = sin() x = cos()
	new_pA_y = sin(radianA + radian) * radiusA;
	new_pA_x = cos(radianA + radian) * radiusA;

	new_pB_y = sin(radianB + radian) * radiusB;
	new_pB_x = cos(radianB + radian) * radiusB;

	new_pC_y = sin(radianC + radian) * radiusC;
	new_pC_x = cos(radianC + radian) * radiusC;

	pA.x = new_pA_x;
	pA.y = new_pA_y;

	pB.x = new_pB_x;
	pB.y = new_pB_y;

	pC.x = new_pC_x;
	pC.y = new_pC_y;

	Triangle tri = { pA,pB,pC };
	return tri;
}

float ConverAtan(float x, float y)
{
	float radian;
	if (x < 0)
	{
		radian = atan(y / x) + M_PI;
	}
	else if (y < 0)
	{
		radian = atan(y / x) + 2 * M_PI;
	}
	else
	{
		radian = atan(y / x);
	}

	return radian;
}


float ConverAtanByMath(float x, float y)
{
	//不用逻辑运算 只用数学运算
	float radian;
	if (x < 0)
	{
		radian = atan(y / x) + M_PI;
	}
	else if (y < 0)
	{
		radian = atan(y / x) + 2 * M_PI;
	}
	else
	{
		radian = atan(y / x);
	}

	return radian;
}

Triangle rotateTriBySpecialPoint(float angle, Point2 pA, Point2 pB, Point2 pC)
{
	Point2 O = setMoveOrigin(pA, pB, pC);

	//计算相对于 O 点的相对坐标
	Point2 new_pA, new_pB, new_pC;
	new_pA = Point2SubtractCovert(O, pA);
	new_pB = Point2SubtractCovert(O, pB);
	new_pC = Point2SubtractCovert(O, pC);
	printf("--------------计算相对于 O 点的相对坐标--------------\n");
	printf("\n new_pA = ( %f , %f ) \n new_pB = ( %f , %f )  \n new_pC =  ( %f , %f )  \n", new_pA.x, new_pA.y, new_pB.x, new_pB.y, new_pC.x, new_pC.y);

	//各个角度的弧度表示
	float radianA, radianB, radianC;
	//radianA = atan(new_pA.y / new_pA.x);
	//radianB = atan(new_pB.y / new_pB.x);
	//radianC = atan(new_pC.y / new_pC.x);

	radianA = ConverAtan(new_pA.x, new_pA.y);
	radianB = ConverAtan(new_pB.x, new_pB.y);
	radianC = ConverAtan(new_pC.x, new_pC.y);

	float degreeA, degreeB, degreeC;
	degreeA = radianA * R2D;
	degreeB = radianB * R2D;
	degreeC = radianC * R2D;
	printf("--------------计算点相对于旋转基准点的角度--------------\n");
	printf("degreeA = %f \n ", degreeA);
	printf("degreeB = %f \n ", degreeB);
	printf("degreeC = %f \n ", degreeC);

	//三个点的半径 勾股定理
	float radiusA, radiusB, radiusC;
	radiusA = sqrt((new_pA.x * new_pA.x) + (new_pA.y * new_pA.y));
	radiusB = sqrt((new_pB.x * new_pB.x) + (new_pB.y * new_pB.y));
	radiusC = sqrt((new_pC.x * new_pC.x) + (new_pC.y * new_pC.y));
	printf("--------------计算新的相对于旋转基准点的半径--------------\n");
	printf("A 半径 = %f \n", radiusA);
	printf("B 半径 = %f \n", radiusB);
	printf("C 半径 = %f \n", radiusC);

	//旋转一定角度后 新的三个点位置
	float radian = angle * D2R;
	float new_pA_x, new_pA_y;
	float new_pB_x, new_pB_y;
	float new_pC_x, new_pC_y;

	//计算旋转对应角度后的坐标位置 后需要加上偏移量！
	//y = sin() x = cos()
	new_pA_y = sin(radianA + radian) * radiusA + O.y;
	new_pA_x = cos(radianA + radian) * radiusA + O.x;

	new_pB_y = sin(radianB + radian) * radiusB + O.y;
	new_pB_x = cos(radianB + radian) * radiusB + O.x;

	new_pC_y = sin(radianC + radian) * radiusC + O.y;
	new_pC_x = cos(radianC + radian) * radiusC + O.x;

	pA.x = new_pA_x;
	pA.y = new_pA_y;

	pB.x = new_pB_x;
	pB.y = new_pB_y;

	pC.x = new_pC_x;
	pC.y = new_pC_y;

	Triangle tri = { pA,pB,pC };
	printf("新三角形位置 :  \nA (%f,%f) ,  B (%f,%f) ,  C (%f,%f)\n ", pA.x, pA.y, pB.x, pB.y, pC.x, pC.y);
	return tri;
}



void drawRotateTri()
{
	ImageData image = { 0 };
	image.width = 680;
	image.height = 460;
	image.chanel = 4;
	image.sizeofRow = image.width * image.chanel;

	Point2 pA = { 10 ,10 };
	Point2 pB = { 400 ,40 };
	Point2 pC = { 10 ,400 };

	Point2 O = setMoveOrigin(pA, pB, pC);

	//Triangle tri = rotateTri(0.f, pA, pB, pC);
	Triangle tri = rotateTriBySpecialPoint(30.f, pA, pB, pC);

	uint8_t* pixel = (uint8_t*)malloc(image.sizeofRow * image.height);


	//UV图片
	ImageData myImage = readUVImage("3.png");

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
				uv.x = factor.alpha * uva.x + factor.beta * uvb.x + factor.gama * uvc.x;
				uv.y = factor.alpha * uva.y + factor.beta * uvb.y + factor.gama * uvc.y;


				Color color = getUVSample(uv, myImage);
				pixel[image.sizeofRow * y + x * image.chanel + 0] = color.r;
				pixel[image.sizeofRow * y + x * image.chanel + 1] = color.g;
				pixel[image.sizeofRow * y + x * image.chanel + 2] = color.b;
				pixel[image.sizeofRow * y + x * image.chanel + 3] = color.a;
			}
			if (y == O.y || x == O.x)
			{
				//画出中心点
				pixel[image.sizeofRow * y + x * image.chanel + 0] = 255;
				pixel[image.sizeofRow * y + x * image.chanel + 1] = 255;
				pixel[image.sizeofRow * y + x * image.chanel + 2] = 255;
				pixel[image.sizeofRow * y + x * image.chanel + 3] = 255;
			}
		}
	}

	SDLDraw(pixel, image.width, image.height, image.sizeofRow, SDL_PIXELFORMAT_RGBA32);
}

float getPointRadius(Point2 O, Point2 p)
{
	//计算相对于 O 点的相对坐标
	Point2 new_p;
	new_p = Point2SubtractCovert(O, p);
	//点的弧度
	float  radian = ConverAtan(new_p.x, new_p.y);
	//点的角度
	float degree = radian * R2D;
	//点的半径
	float radius = sqrt((new_p.x * new_p.x) + (new_p.y * new_p.y));
	//printf("radius = %f \n", radius);
	return radius;
}

float getPointAngle(Point2 O, Point2 p)
{
	//计算相对于 O 点的相对坐标
	Point2 new_p;
	new_p = Point2SubtractCovert(O, p);
	//点的弧度
	float  radian = ConverAtan(new_p.x, new_p.y);
	//点的角度
	float degree = radian * R2D;
	//点的半径
	float radius = sqrt((new_p.x * new_p.x) + (new_p.y * new_p.y));
	//printf("radius = %f \n", radius);
	return degree;
}


//极坐标显示半径和角度
void drawRA()
{

	ImageData image = { 0 };
	image.width = 680;
	image.height = 460;
	image.chanel = 4;
	image.sizeofRow = image.width * image.chanel;

	Point2 pA = { 10 ,10 };
	Point2 pB = { 400 ,40 };
	Point2 pC = { 10 ,400 };
	Triangle tri = { pA, pB, pC };

	uint8_t* pixel = (uint8_t*)malloc(image.sizeofRow * image.height);
	Point2 O = setMoveOrigin(pA, pB, pC);

	Point2 uv = { 1,1 };
	Point2 uva = { 0,1 };
	Point2 uvb = { 1,1 };
	Point2 uvc = { 0,0 };

	//UV图片
	ImageData myImage = readUVImage("3.png");

	for (size_t y = 0; y < image.height; y++)
	{
		for (size_t x = 0; x < image.width; ++x)
		{
			TriangleFactor factor = { 0 };
			factor = inTriangle(x + 0.5, y + 0.5, tri);

			if ((factor.gama >= 0) && (factor.beta >= 0) && (factor.alpha >= 0))
			{

				Point2 p;
				p.x = x;
				p.y = y;

				float radius = getPointRadius(O, p) / (float)image.height;
				float angle = getPointAngle(O, p) / 360;
				//printf("\nangle = %f\n",angle);

				//R/G/B/A = alpha*A + beta*B + gama*C

				//uv.x = factor.alpha * uva.x + factor.beta * uvb.x + factor.gama * uvc.x;
				//uv.y = factor.alpha * uva.y + factor.beta * uvb.y + factor.gama * uvc.y;
				uv.x = radius;
				uv.y = angle;

				Color color = getUVSample(uv, myImage);
				pixel[image.sizeofRow * y + x * image.chanel + 0] = color.r;
				pixel[image.sizeofRow * y + x * image.chanel + 1] = color.g;
				pixel[image.sizeofRow * y + x * image.chanel + 2] = color.b;
				pixel[image.sizeofRow * y + x * image.chanel + 3] = color.a;


			}

		}
	}

	SDLDraw(pixel, image.width, image.height, image.sizeofRow, SDL_PIXELFORMAT_RGBA32);
}

Triangle rotateUVPoint(float angle, Point2 pA, Point2 pB, Point2 pC)
{
	////获取当前的uv坐标

	////旋转当前的uv

	////获取旋转对应角度后的uv坐标
	Point2 O;
	O.x = (pA.x + pB.x) * 0.5f;
	O.y = (pA.y + pC.y) * 0.5f;
	//Point2 O = setMoveOrigin(pA, pB, pC);
	//计算相对于 O 点的相对坐标
	Point2 new_pA, new_pB, new_pC;
	new_pA = Point2SubtractCovert(O, pA);
	new_pB = Point2SubtractCovert(O, pB);
	new_pC = Point2SubtractCovert(O, pC);
	//各个角度的弧度表示
	float radianA, radianB, radianC;
	//radianA = atan(new_pA.y / new_pA.x);
	//radianB = atan(new_pB.y / new_pB.x);
	//radianC = atan(new_pC.y / new_pC.x);

	radianA = ConverAtan(new_pA.x, new_pA.y);
	radianB = ConverAtan(new_pB.x, new_pB.y);
	radianC = ConverAtan(new_pC.x, new_pC.y);

	float degreeA, degreeB, degreeC;
	degreeA = radianA * R2D;
	degreeB = radianB * R2D;
	degreeC = radianC * R2D;

	//三个点的半径 勾股定理
	float radiusA, radiusB, radiusC;
	radiusA = sqrt((new_pA.x * new_pA.x) + (new_pA.y * new_pA.y));
	radiusB = sqrt((new_pB.x * new_pB.x) + (new_pB.y * new_pB.y));
	radiusC = sqrt((new_pC.x * new_pC.x) + (new_pC.y * new_pC.y));

	//旋转一定角度后 新的三个点位置
	float radian = angle * D2R;
	float new_pA_x, new_pA_y;
	float new_pB_x, new_pB_y;
	float new_pC_x, new_pC_y;

	//计算旋转对应角度后的坐标位置 后需要加上偏移量！
	//y = sin() x = cos()
	new_pA_y = sin(radianA + radian) * radiusA + O.y;
	new_pA_x = cos(radianA + radian) * radiusA + O.x;

	new_pB_y = sin(radianB + radian) * radiusB + O.y;
	new_pB_x = cos(radianB + radian) * radiusB + O.x;

	new_pC_y = sin(radianC + radian) * radiusC + O.y;
	new_pC_x = cos(radianC + radian) * radiusC + O.x;

	pA.x = new_pA_x;
	pA.y = new_pA_y;

	pB.x = new_pB_x;
	pB.y = new_pB_y;

	pC.x = new_pC_x;
	pC.y = new_pC_y;

	Triangle tri = { pA,pB,pC };
	return tri;
}


//旋转UV
void rotateUVbyAngle(float angle)
{
	ImageData image = { 0 };
	image.width = 680;
	image.height = 460;
	image.chanel = 4;
	image.sizeofRow = image.width * image.chanel;

	Point2 pA = { 10 ,10 };
	Point2 pB = { 400 ,40 };
	Point2 pC = { 10 ,400 };
	Triangle tri = { pA, pB, pC };

	uint8_t* pixel = (uint8_t*)malloc(image.sizeofRow * image.height);

	//设置旋转原点
	Point2 O = setMoveOrigin(pA, pB, pC);

	Point2 uv = { 1,1 };
	Point2 uva = { 0,1 };
	Point2 uvb = { 1,1 };
	Point2 uvc = { 0,0 };

	//旋转UV
	Triangle newuvVec = rotateUVPoint(angle, uva, uvb, uvc);

	uva = newuvVec.A;
	uvb = newuvVec.B;
	uvc = newuvVec.C;

	//UV图片
	ImageData myImage = readUVImage("3.png");

	for (size_t y = 0; y < image.height; y++)
	{
		for (size_t x = 0; x < image.width; ++x)
		{
			TriangleFactor factor = { 0 };
			factor = inTriangle(x + 0.5, y + 0.5, tri);

			if ((factor.gama >= 0) && (factor.beta >= 0) && (factor.alpha >= 0))
			{

				//R/G/B/A = alpha*A + beta*B + gama*C

				uv.x = factor.alpha * uva.x + factor.beta * uvb.x + factor.gama * uvc.x;
				uv.y = factor.alpha * uva.y + factor.beta * uvb.y + factor.gama * uvc.y;

				Color color = getUVRotateSample(uv, myImage);
				pixel[image.sizeofRow * y + x * image.chanel + 0] = color.r;
				pixel[image.sizeofRow * y + x * image.chanel + 1] = color.g;
				pixel[image.sizeofRow * y + x * image.chanel + 2] = color.b;
				pixel[image.sizeofRow * y + x * image.chanel + 3] = color.a;


			}

		}
	}

	SDLDraw(pixel, image.width, image.height, image.sizeofRow, SDL_PIXELFORMAT_RGBA32);

	free(pixel);
	free(image.pixel);
	free(myImage.pixel);
	pixel = NULL;
	image.pixel = NULL;
	myImage.pixel = NULL;
}

int main()
{
	//readPNG("9.png", SDL_PIXELFORMAT_RGBA32);
	//drawTriangle();
	//drawUVTri();
	//drawUVTriangle();
	//drawRotateTri();
	//drawRA();
	float angle = 0;
	rotateUVbyAngle(angle);
	while (true)
	{
		angle = angle - 5.0f;
		printf("angle = %f \n", angle);
		rotateUVbyAngle(angle);
		SDL_Delay(200);
	}

	std::cout << "Hello World!\n";
	return 0;
}

