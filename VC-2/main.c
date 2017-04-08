#define _CRT_SECURE_NO_DEPRECATE
#define M_PI 3.1459265358979323846
#include <math.h>
#include <stdio.h>
#include "vc.h"



/*
//Abrir imagem, alterar e gravar um novo ficheiro
int main(void)
{
	IVC *image[2]; // image[0] -> rgb image[1] -> gray
	int i, nblobs;
	OVC *blobs;
	image[0] = vc_read_image("Images/Highway.ppm");
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);

	if (image[0]==NULL || image[1] == NULL)
	{
		printf("Error -> vc_read_image():\n\t File not found!\n");
		getchar();
		return 0;
	}

	vc_rgb_to_gray(image[0], image[1]);
	vc_write_image("gray.pgm", image[1]);
	vc_image_free(image[0]);
	vc_image_free(image[1]);
	image[0] = vc_read_image("gray.pgm");
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	
	if (image[0] == NULL || image[1]==NULL)
	{
		printf("Error -> vc_read_image():\n\t File not found!\n");
		getchar();
		return 0;
	}


	
	vc_gray_to_binary(image[0], 110); // threeshold between 95 and 110
	blobs = vc_binary_blob_labelling(image[0], image[1], &nblobs);
	if (blobs != NULL)
	{
		vc_binary_blob_info(image[1],blobs,nblobs);
		printf("\nNumero de objetos segmentados: %d\n", nblobs);
		for (i = 0; i<nblobs; i++)
		{
			if (blobs[i].area > 0) {
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
			}
		}

		free(blobs);
	}

	vc_write_image("test.pgm", image[1]);
	//vc_image_free(image[0]);
	vc_image_free(image[1]);



	
	// 2º Teste ... Tentativa de marcar borda a volta do sinal

	image[0] = vc_read_image("Images/Highway.ppm");
	if (image[0] == NULL)
	{
		printf("Error -> vc_read_image():\n\t File not found!\n");
		getchar();
		return 0;
	}
	else
	{
		vc_rgb_identifyroadsign(image[0], 45,160,408,510);
		vc_write_image("Teste.pgm", image[0]);
		vc_image_free(image[0]);
	}



	

	//vc_write_image("newSignal.pgm", image[0]);
	//vc_image_free(image[0]);
	//vc_image_free(image[1]);


	/*

	for (i = 0; i<image->bytesperline*image->height; i += image->channels)
		image->data[i] = 255 * image->data[i];

	vc_write_image("Results/vc-0001.pgm", image);

	system("FilterGear Images/FLIR/flir-01.pgm");
	system("FilterGear Results/vc-0001.pgm");

	vc_image_free(image);

	printf("Press any key to exit...\n");
	getchar();
	*/
	//return 0;
//}


int main(void)
{
	IVC *image[3];
	int i, nblobs;
	OVC *blobs;

	image[0] = vc_read_image("Images/ArrowUp.ppm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}

	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, 255); //nova imagem de 1 canal
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}
	//vc_rgb_to_hsv(image[0]);//transformar image[0] de rgb para hsv
	//vc_write_image("hsv.pgm",image[0]);
	//vc_image_free(image[0]);
	//image[0] = vc_read_image("hsv.pgm");
	/*if (image[0] == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}
	*/
	//color threesholding (out binary image)
	
	//vc_gray_to_binary(image[0], 103); // threeshold >95 && <110
	//vc_write_image("gray_to_bin.pgm", image[0]);
	colorThreeSholding(image[0], image[1]);
	vc_write_image("colorthr.pgm", image[1]);

	
	image[0] = vc_read_image("colorthr.pgm");
	//vc_image_free(image[1]);
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

	////eliminar ruido da imagem
	
    
//	vc_binary_open(image[0], image[1], 30, 30);
	//vc_write_image("testerosion.pgm", image[1]);

	if (image[0] == NULL || image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

	blobs = vc_binary_blob_labelling(image[0], image[1], &nblobs);
	int area, equation, sideOne, sideTwo;
	
	if (blobs != NULL)
	{
		vc_binary_blob_info(image[1], blobs, nblobs);
		printf("\nNumero de objetos segmentados: %d\n", nblobs);
		for (i = 0; i<nblobs; i++)
		{

		
			if (blobs[i].area > 70000) {
				int xmin, ymin, xmax, ymax;
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
				area = blobs[i].width * blobs[i].height;
				xmin = blobs[i].x;
				ymin = blobs[i].y;
				xmax = (blobs[i].width - xmin);
				ymax = (blobs[i].height - ymin);
			
			}
			//circulo
			/*
			else if (blobs[i].area > 45000 && blobs[i].area < 65000) {
					printf("\n-> Label %d:\n", blobs[i].label);
					printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
					int r = blobs[i].width / 2;
					double areaCirculo = M_PI * pow(r, 2);
					printf("\n  Area %.2f\n\n", areaCirculo);
					xmin = blobs[i].x;
					ymin = blobs[i].y;
					xmax = blobs[i].width;
					ymax = blobs[i].height;

				}
			*/
			

			else if (blobs[i].area > 6000 && blobs[i].area < 8000) {
				int xmin, ymin, xmax, ymax;
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
				int r = blobs[i].width / 2;
				double areaCirculo = M_PI * pow(r, 2);
				printf("\n  Area %.2f\n\n", areaCirculo);
				xmin = blobs[i].x - (blobs[i].width * 0,20);
				ymin = blobs[i].y - (blobs[i].height * 0,20);
				xmax = blobs[i].width + 120;
				ymax = blobs[i].height + 160;
				image[2] = vc_read_image("Images/ArrowLeft.ppm");
				vc_rgb_identifyroadsign(image[2], xmin, ymin, xmax, ymax);
				vc_write_image("signdetected.pgm", image[2]);
				system("FilterGear signdetected.pgm");
			}
			else if (blobs[i].area > 8000 && blobs[i].area < 12000) {
				int xmin, ymin, xmax, ymax;
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
				int r = blobs[i].width / 2;
				double areaCirculo = M_PI * pow(r, 2);
				printf("\n  Area %.2f\n\n", areaCirculo);
				xmin = blobs[i].x + (blobs[i].width * 0, 10);
				ymin = blobs[i].y + (blobs[i].height * 0, 10);
				xmax = blobs[i].width + 60;
				ymax = blobs[i].height + 160;
				image[2] = vc_read_image("Images/ArrowUp.ppm");
				vc_rgb_identifyroadsign(image[2], xmin, ymin, xmax, ymax);
				vc_write_image("signdetected.pgm", image[2]);
				system("FilterGear signdetected.pgm");

			}
			
			/*
				else if (blobs[i].area > 5000 && blobs[i].area < 6000) {
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
				int r = blobs[i].width / 2;
				double areaCirculo = M_PI * pow(r, 2);
				printf("\n  Area %.2f\n\n", areaCirculo);
				xmin = blobs[i].x - (blobs[i].width * 0,20);
				ymin = blobs[i].y - (blobs[i].height * 0,20);
				xmax = blobs[i].width + 120;
				ymax = blobs[i].height + 160;

			}
				else if (blobs[i].area > 5000 && blobs[i].area < 6000) {
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
				int r = blobs[i].width / 2;
				double areaCirculo = M_PI * pow(r, 2);
				printf("\n  Area %.2f\n\n", areaCirculo);
				xmin = blobs[i].x - (blobs[i].width * 0,20);
				ymin = blobs[i].y - (blobs[i].height * 0,20);
				xmax = blobs[i].width + 120;
				ymax = blobs[i].height + 160;

			}
				*/
			
			if (blobs[i].area > 0) {
				printf("\n-> Label %d:\n", blobs[i].label);
				printf("   Area=%-5d Perimetro=%-5d x=%-5d y=%-5d w=%-5d h=%-5d xc=%-5d yc=%-5d\n", blobs[i].area, blobs[i].perimeter, blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height, blobs[i].xc, blobs[i].yc);
				
			}
			
		}
		free(blobs);
	}
	

//	vc_write_image("test.pgm", image[1]);

	vc_image_free(image[0]);
	vc_image_free(image[1]);
	
//	system("FilterGear vc0022.pgm");
	printf("Press any key to exit...\n");


	getchar();

	return 0;
}

