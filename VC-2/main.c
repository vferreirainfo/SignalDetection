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


int main_001(void)
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
	vc_write_image("graysignal.ppm", image[1]);
	vc_image_free(image[1]);
	vc_image_free(image[0]);
	image[0] = vc_read_image("graysignal.ppm");
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	vc_gray_to_binary(image[0], 105); // threeshold >95 && <110
	vc_write_image("binary.pgm", image[0]);
	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels,image[0]->levels);
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}
	////eliminar ruido da imagem
	
	vc_binary_open(image[0], image[1], 2, 2);
	vc_write_image("testerosion.pgm", image[1]);
	vc_image_free(image[1]);
	vc_image_free(image[0]);


	image[0] = vc_read_image("open.pgm");
	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels , image[0]->levels);
	if (image[0] == NULL || image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

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

	vc_image_free(image[0]);
	vc_image_free(image[1]);
	printf("Press any key to exit...\n");

	system("FilterGear vc0022.pgm");


	getchar();

	return 0;
}


int main(void) 
	{
	//	return 0;
	//imagem de entrada e saida! Array c/ 2 imagens tipo IVC

	IVC *image[2];


	//a imagem de entrada
	//image[0]= vc_read_image("Images/4850FU(Passat).ppm");

	image[0] = vc_read_image("Images/Highway.ppm");
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	if (image[0] == NULL)
	{
		printf("Error -> vc_read_image():\n\t Ficheiro Nao Encontrado!\n");
		printf("Ficheiro não encontrado!");
		getchar();
		return 0;
	}
	else
	{
		printf("ficheiro Encontado");

		//vc_rgb_to_hsv(image[0]);

		//transformar em escala de cinzentos
		vc_rgb_to_gray(image[0], image[1]);
		//vc_write_image("togray.pgm", image[1]);
		vc_write_image("togray.pgm", image[1]);

		//vc_gray_edge_prewitt(image[1], image[0], 140);
		//vc_write_image("toprewitt.ppm", image[0]);
		// COR A CINZENTO (gray) a CONTORNOS (binario) e fazemos 2 histogramas 1 na horizontal e outro na vertical e quando der match sabes onde ta a matricula

		//image[0] = image[1];
		//vc_gray_histogram(image[1], image[0]);

		//vc_write_image("tohistogramtest.ppm", image[0]);

		vc_image_free(image[1]);
		vc_image_free(image[0]);

		image[0] = vc_read_image("togray.pgm");

		if (image[0] == NULL)
		{
			printf("Error -> vc_read_image():\n\t Ficheiro Nao Encontrado!\n");
			printf("Ficheiro não encontrado!");
			getchar();
			return 0;
		}
		//		//vc_gray_histogram(image[0],image[1]);
		//		//vc_write_image("imagehist.pgm",image[1]);
		//		//vc_image_free(image[0]);
		//		//vc_image_free(image[1]);
		vc_gray_to_binary(image[0], 140);
		vc_write_image("graytobinary.pgm", image[0]);
		//		//printf("Height by 3: %d",image[0]->height/3);
		//vc_image_free(image[0]);
		//image[0] = vc_read_image("togray.pgm");
		image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
		//int *contaLinhas;
		//contaLinhas = (int *)malloc(image[1]->height, sizeof(int));
		//contaLinhas++;
		//
		vc_binary_open(image[0], image[1], 2, 2);
		vc_write_image("testerosion.pgm", image[1]);
		vc_image_free(image[1]);
		vc_image_free(image[0]);

		//image[0] = vc_read_image("testerosion.pgm");
		//image[1] = vc_image_new(image[0]->width, image[0]->height, 3, image[0]->levels);
		//detectamatricula(image[0],image[1],210,135);
		//vc_write_image("teste-1.pgm",image[1]);
		//vc_image_free(image[1]);
		//vc_image_free(image[0]);

		//vc_detect_europeansheepform(image[0], image[1], 25);
		//vc_write_image("license.pgm",image[1]);
		//vc_image_free(image[0]);
		//vc_image_free(image[1]);		
		/*
		image[0] = vc_read_image("graytobinary.pgm");
		image[1] = vc_image_new(image[0]->width,image[0]->height, 3,image[0]->levels);
		if(image[0]==NULL)
		{
		printf("Error -> vc_read_image():\n\t Ficheiro Nao Encontrado!\n");
		printf("Ficheiro não encontrado!");
		ge	tchar();
		return 0;
		}
		vc_detect_europeansheepform(image[0],image[1],50);
		vc_write_image("test.ppm",image[1]);
		printf("BTL: %d",image[0]->bytesperline);
		vc_image_free(image[1]);
		vc_image_free(image[0]);

		*/
		//		if(image[0]==NULL)
		//		{
		//			printf("Error -> vc_read_image():\n\t Ficheiro Nao Encontrado!\n");
		//			printf("Ficheiro não encontrado!");
		//			getchar();
		//			return 0;
		//		}
		//		
		//		vc_gray_to_binary(image[0],150);
		//		vc_write_image("graytobinary.pgm",image[0]);
		//		vc_image_free(image[0]);

		//		image[0]= vc_read_image("graytobinary.pgm");
		//		
		//		if(image[0]==NULL)
		//		{
		//			printf("Error -> vc_read_image():\n\t Ficheiro Nao Encontrado!\n");
		//			printf("Ficheiro não encontrado!");
		//			getchar();
		//			return 0;
		//		}
		//image[1] = vc_image_new(image[0]->width,image[0]->height, image[0]->channels,image[0]->levels);
		//vc_binary_open(image[0],image[1], 5, 5);
		//vc_write_image("testerosion.pgm",image[1]);
		//vc_image_free(image[1]);
		//vc_image_free(image[0]);

		//		image[0]=vc_read_image("graytobinary.pgm");
		//		image[1] = vc_image_new(image[0]->width,image[0]->height,image[0]->channels,image[0]->levels);
		//		vc_detect_europeansheepform(image[0],image[1],6);		
		//		vc_write_image("test.pgm",image[1]);
		//		vc_image_free(image[1]);
		//		vc_image_free(image[0]);
		//		
		//	vc_write_image("vc1-0001.ppm",image[0]);
		//	vc_image_free(image[0]);
		//image[1]=vc_image_new(image[0]->width,image[0]->height, 1,8); //Nao tenho bem certeza disto. Temos que ter uma imagem em escala de cinzentos
		//vc_rgb_gray(image[0],image[1]);
		//vc_write_image("4850FU-Gray.pgm",image[1]);

		//vc_image_free(image[0]);

		printf("Prima qualquer tecla para continuar");
		//get the new pgm image and convert to binary

		//IVC *newImage[2];

		//newImage[0]=image[1];//passagem da imagem gpm para a variavel newImage[0]
		//newImage[1] = vc_image_new(newImage[0]->width,newImage[0]->height, 1,newImage[0]->levels); //com o mesmo tamanho de newImage[0]



		//vc_gray_to_binary_midpoint(newImage[0],newImage[1],3);
		//	vc_gray_to_binary_midpoint(image[1], image[0], 3);

		//vc_image_free(newImage[0]);

		getchar();



	}
	return 1;

}

