#define _CRT_SECURE_NO_DEPRECATE
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
	IVC *image[2];
	int i, nblobs;
	OVC *blobs;

	image[0] = vc_read_image("Images/Highway.ppm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}

	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, 255);
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}
	vc_rgb_to_hsv(image[0]);
	vc_write_image("hsv.pgm",image[0]);
	vc_rgb_to_gray(image[0], image[1]);
	vc_gray_to_binary(image[1], 105);
	vc_write_image("binary.pgm", image[1]);
	vc_image_free(image[1]);
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, 255);
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}
	image[0] = vc_read_image("binary.pgm");
	blobs = vc_binary_blob_labelling(image[0], image[1], &nblobs);

	if (blobs != NULL)
	{
		printf("\nNumber of labels: %d\n", nblobs);
		for (i = 0; i<nblobs; i++)
		{
			printf("-> Label %d\n", blobs[i].label);
		}

		free(blobs);
	}

	vc_write_image("vc0022.pgm", image[1]);

	vc_image_free(image[0]);
	vc_image_free(image[1]);

	system("FilterGear vc0022.pgm");

	printf("Press any key to exit...\n");
	getchar();

	return 0;
}
