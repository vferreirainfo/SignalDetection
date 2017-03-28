#define _CRT_SECURE_NO_DEPRECATE
#define MAX(x,y) (x>y?x:y)
#define MIN(x,y) (x<y?x:y)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2016/2017
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//             [  SANDRO QUEIRÓS - squeiros@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUNÇÕES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




int vc_rgb_to_gray(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int channels_dst = dst->channels;
	int bytesperline_dst = dst->width * dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height)) return 0;
	if ((src->channels != 3) || (dst->channels != 1)) return 0;

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pos_src = y*bytesperline_src + x * channels_src;
			pos_dst = y*bytesperline_dst + x * channels_dst;

			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((rf * 0.299) + (gf * 0.587) + (bf * 0.114));

		}
	}
	return 1;
}

	
	



int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int xx, yy;
	int xxyymax = (int)floor(((double)kernel) / 2.0);//começa em 1, se kernel =3
	int xxyymin = -xxyymax;//-1
	int max, min;
	long int pos, posk;
	unsigned char threshold;

	// Verificacao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
	if (channels != 1) return 0;

	for (y = 0; y<height; y++)//y=0 ...
	{
		for (x = 0; x<width; x++)//x=0 ...
		{
			pos = y * bytesperline + x * channels;

			max = datasrc[pos];
			min = datasrc[pos];

			// NxM Vizinhos
			for (yy = xxyymin; yy <= xxyymax; yy++)//yy=-1 ... yy<=yymax (yymax=1) YY++ .... 
			{
				for (xx = xxyymin; xx <= xxyymax; xx++)//xx=-1 ... zz<=xxyymax; xx++ ...
				{
					if ((y + yy >= 0) && (y + yy < height) && (x + xx >= 0) && (x + xx < width))
					{

						posk = (y + yy) * bytesperline + (x + xx) * channels;

						if (datasrc[posk] > max) max = datasrc[posk];
						if (datasrc[posk] < min) min = datasrc[posk];
					}
				}
			}

			threshold = (unsigned char)((float)(max + min) / (float)2);

			if (datasrc[pos] > threshold) datadst[pos] = 255;
			else datadst[pos] = 0;
		}
	}

	return 1;
}



// Etiquetagem de blobs
// src		: Imagem bin�ria
// dst		: Imagem grayscale (ir� conter as etiquetas)
// nlabels	: Endere�o de mem�ria de uma vari�vel inteira. Recebe o n�mero de etiquetas encontradas.
// OVC*		: Retorna lista de estruturas de blobs (objectos), com respectivas etiquetas. � necess�rio libertar posteriormente esta mem�ria.
OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num;
	OVC *blobs; // Apontador para lista de blobs (objectos) que ser� retornada desta fun��o.

				// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem bin�ria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pix�is de plano de fundo devem obrigat�riamente ter valor 0
	// Todos os pix�is de primeiro plano devem obrigat�riamente ter valor 255
	// Ser�o atribu�das etiquetas no intervalo [1,254]
	// Este algoritmo est� assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i<size; i++)
	{
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem bin�ria
	for (y = 0; y<height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x<width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y<height - 1; y++)
	{
		for (x = 1; x<width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

													// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a etiqueta "num"
					if ((datadst[posA] != 0) && (datadst[posA] != 255) && (datadst[posA] < num))
					{
						num = datadst[posA];
					}
					// Se B est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (datadst[posB] != 255) && (datadst[posB] < num))
					{
						num = datadst[posB];
					}
					// Se C est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (datadst[posC] != 255) && (datadst[posC] < num))
					{
						num = datadst[posC];
					}
					// Se D est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (datadst[posD] != 255) && (datadst[posD] < num))
					{
						num = datadst[posD];
					}

					// Actualiza a tabela de etiquetas
					if ((datadst[posA] != 0) && (datadst[posA] != 255))
					{
						if (labeltable[datadst[posA]] != labeltable[num])
						{
							for (a = 1; a<label; a++)
							{
								if (labeltable[a] == labeltable[datadst[posA]])
								{
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					if ((datadst[posB] != 0) && (datadst[posB] != 255))
					{
						if (labeltable[datadst[posB]] != labeltable[num])
						{
							for (a = 1; a<label; a++)
							{
								if (labeltable[a] == labeltable[datadst[posB]])
								{
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					if ((datadst[posC] != 0) && (datadst[posC] != 255))
					{
						if (labeltable[datadst[posC]] != labeltable[num])
						{
							for (a = 1; a<label; a++)
							{
								if (labeltable[a] == labeltable[datadst[posC]])
								{
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					if ((datadst[posD] != 0) && (datadst[posD] != 255))
					{
						if (labeltable[datadst[posD]] != labeltable[num])
						{
							for (a = 1; a<label; a++)
							{
								if (labeltable[a] == labeltable[datadst[posD]])
								{
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					labeltable[datadst[posX]] = num;

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y<height - 1; y++)
	{
		for (x = 1; x<width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	// Contagem do n�mero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a<label - 1; a++)
	{
		for (b = a + 1; b<label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que n�o hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a<label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se n�o h� blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC *)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a<(*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}






// Etiquetagem de blobs
// src		: Imagem bin�ria
// dst		: Imagem grayscale (ir� conter as etiquetas)
// nlabels	: Endere�o de mem�ria de uma vari�vel inteira. Recebe o n�mero de etiquetas encontradas.
// OVC*		: Retorna lista de estruturas de blobs (objectos), com respectivas etiquetas. � necess�rio libertar posteriormente esta mem�ria.















































int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs)
{
	unsigned char *data = (unsigned char *)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta �rea de cada blob
	for (i = 0; i<nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y<height - 1; y++)
		{
			for (x = 1; x<width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// �rea
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Per�metro
					// Se pelo menos um dos quatro vizinhos n�o pertence ao mesmo label, ent�o � um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		blobs[i].xc = sumx / (blobs[i].area > 1 ? blobs[i].area : 1);
		blobs[i].yc = sumy / (blobs[i].area > 1 ? blobs[i].area : 1);
	}

	return 1;
}

























/*

int vc_identify_roadsign(IVC *srcdst)
{
	int pos, posK; // posK we may not need
	int height, width;
	int channels;
	int bytesperline;
	unsigned char *data = (unsigned char *)srcdst->data; // imagem src
	unsigned char *seconddata = (unsigned char *)srcdst->data; 
	height = srcdst->height;
	width = srcdst->width;
	channels = srcdst->channels;
	bytesperline = srcdst->width * srcdst->channels;
	int xxline, xxendline, yyline, yyendline;


	if ((height <= 0) || (width <= 0) || (data == NULL) || (seconddata == NULL)) return 0;
	if ((channels != 3))return 0;

	//Percorrer a imagem destino a procura de cores
	
	for (int y = 0; y < height; y++) // percorrer y a partir da primeira linha para baixo
		{
		for (int x = 0; x < width; x++) //percorrer a primeira linha
		{


			pos = (y*bytesperline) + (x*channels);
			posK = (y*bytesperline) + (x*channels);

			if ((data[pos] >= 31 && data[pos] <= 65) && (data[pos + 1] >= 42 && data[pos + 1] <= 75) && (data[pos + 2] > 55 && data[pos + 2] <= 80))
			{
			
				int xx, yy; // where x and y starts
				yy = (height * 0.25) + 1;// +1 por precisao ... yy<-160

				int lastx, lasty;
				lasty = (int)height*0.80; // ~ pixel 511 em y
				xx = (int)width * 0.095; // ~ pixel 45 em x
				lastx = xx + 1;
				//Algoritmo tem que ser revisto !!

				//comecar a desenhar em y
				// quero desenhar apenas uma linha na vertical ... x apenas será incrementado uma vez
				for (xx; xx <= lastx; xx++)
				{
					for (yy; yy <= lasty; yy++)
					{


						if ((seconddata[posK]>=175 && seconddata[posK]<=195)&&(seconddata[posK+1]>=175&& seconddata[posK+1]<=190)
							&& (seconddata[posK + 2] >= 175 && seconddata[posK+2] <= 190))
							//testar padrao yy
						{
							seconddata[posK] = 0;
							seconddata[posK + 1] = 255;
							seconddata[posK + 2] = 0;

						}
					}


				}

			}
			//procurar padroes vermelhos nos sinais se existirem

			else if ((data[pos] >= 153 && data[pos] <= 255) && (data[pos + 1] >= 42 && data[pos + 1] <= 51) && (data[pos + 2] >= 0 && data[pos + 2] <= 51))
			{
				//genericamente encontramos-nos em padroes de vermelho

				//verificar area
			}
			}
			}

	return 1;
}

*/

int vc_rgb_identifyroadsign(IVC *srcdst, int xmin, int ymin, int xmax, int ymax)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	//verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
	{
		return 0;
	}

	if (channels != 3)
	{
		return 0;
	}

	if ((width < xmin < 0) || (height < ymin < 0) || (0 > xmax > width) || (0 > ymax > height))
	{
		return 0;
	}

	//criar retangulo com as dimensões especificadas como argumentos
	for (x = xmin; x <= xmax; x++)
	{
		for (y = ymin; y <= ymax; y++)
		{
			pos = y*bytesperline + x*channels;
			if (x == xmin || x == xmax || y == ymin || y == ymax)
			{
				// cor vermelha formato rgb
				data[pos] = 0;
				data[pos + 1] = 255;
				data[pos + 2] = 0;
			}

		}
	}
	return 1;
}


int vc_gray_to_binary(IVC *srcdst, int treshold)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int i;
	int pos;
	int x, y;

	// Verificaï¿½ï¿½o de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
	if (channels != 1) return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			if (data[pos] > treshold)
			{
				data[pos] = 255;
			}
			else
			{
				data[pos] = 0;
			}
		}
	}
	return 1;

}


//++++++++ Gerar negativo da imagem Gray ++++++++++++++++++++++++
int vc_gray_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char*)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	//+++++++++++++++++++++++++ CONDIÇÃO DA FUNÇÃO:  ++++++++++++++++++++++++++

	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
	if (channels != 1) return 0;


	//++++++++++++++++++++++++++++ NEGATIVE GRAY  +++++++++++++++++++++++++++++

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y*bytesperline + x*channels;
			data[pos] = 255 - data[pos];
		}
	}
	return 1;
}

	int vc_rgb_negative(IVC *srcdst)
	{
		unsigned char *data = (unsigned char *)srcdst->data;
		int width = srcdst->width;
		int height = srcdst->height;
		int bytesperline = srcdst->bytesperline;
		int channels = srcdst->channels;
		int x, y;
		long int pos;


		//+++++++++++++++++++++++++ CONDIÇÃO DA FUNÇÃO:  ++++++++++++++++++++++++++

		if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
		if (channels != 3) return 0;


		//++++++++++++++++++++++++++++ NEGATIVE RGB  ++++++++++++++++++++++++++++++

		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				pos = (y*bytesperline) + (x*channels);

				//SEGMENTA R:
				if ((255 - data[pos]) < 0)
					data[pos] = 0;
				else
					data[pos] = 255 - data[pos];

				//SEGMENTA G:
				if ((255 - data[pos + 1]) < 0)
					data[pos + 1] = 0;
				else
					data[pos + 1] = 255 - data[pos + 1];

				//SEGMENTA B:
				if ((255 - data[pos + 2]) < 0)
					data[pos + 2] = 0;
				else
					data[pos + 2] = 255 - data[pos + 2];
			}
		}
		return 1;
	}

	int vc_rgb_get_red_gray(IVC *srcdst)
	{
		unsigned char *data = (unsigned char*)srcdst->data;
		int width = srcdst->width;
		int height = srcdst->height;
		int bytesperline = srcdst->width * srcdst->channels;
		int channels = srcdst->channels;
		int x, y;
		long int pos;


		//+++++++++++++++++++++++++ CONDIÇÃO DA FUNÇÃO:  ++++++++++++++++++++++++++

		if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
		if (channels != 3) return 0;


		//+++++++++++++++++++ IMAGEM GRAY COM COMPONENTE RED:  +++++++++++++++++++++

		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				pos = y*bytesperline + x*channels;

				//COLOCA A IMAGEM COM APENAS RED:
				data[pos + 1] = data[pos];
				data[pos + 2] = data[pos];
			}
		}
		return 1;
	}




	//++++++++ Mostra componente Blue da imagem RGB  +++++++
	int vc_rgb_get_blue(IVC *srcdst)
	{
		unsigned char *data = (unsigned char*)srcdst->data;
		int width = srcdst->width;
		int height = srcdst->height;
		int bytesperline = srcdst->width * srcdst->channels;
		int channels = srcdst->channels;
		int x, y;
		long int pos;


		//+++++++++++++++++++++++++ CONDIÇÃO DA FUNÇÃO:  ++++++++++++++++++++++++++

		if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
		if (channels != 3) return 0;


		//++++++++++++++++ IMAGEM RGB COM APENAS COMPONENTE BLUE:  +++++++++++++++++

		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				pos = y*bytesperline + x*channels;

				//COLOCA A IMAGEM COM APENAS BLUE:
				data[pos] = 0;
				data[pos + 1] = 0;
			}
		}
		return 1;
	}

	//++++++++ Converter imagem


	//++++++++ Converter imagem RGB para Gray +++++++++++++++++++++++
	

	//++++++++ Converter imagem RGB para HSV ++++++++++++++++++++++++
	// RGB para HSV
int vc_rgb_to_hsv(IVC *srcdst)
{
	unsigned char *data = (unsigned char *) srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	float r, g, b, hue, saturation, value;
	float rgb_max, rgb_min;
	int i, size;

	// Verifica��o de erros
	if((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
	if(channels != 3) return 0;
	
	size = width * height * channels;
	
	for(i=0; i<size; i=i+channels)
	{
		r = (float) data[i];
		g = (float) data[i + 1];
		b = (float) data[i + 2];
		
		// Calcula valores m�ximo e m�nimo dos canais de cor R, G e B
		rgb_max = (r > g ? (r > b ? r : b) : (g > b ? g : b));
		rgb_min = (r < g ? (r < b ? r : b) : (g < b ? g : b));
		
		// Value toma valores entre [0,255]
		value = rgb_max;
		if(value == 0.0)
		{
			hue = 0.0;
			saturation = 0.0;
		}
		else
		{
			// Saturation toma valores entre [0,255]
			saturation = ((rgb_max - rgb_min) / rgb_max) * 255.0f;

			if(saturation == 0.0)
			{
				hue = 0.0f;
			}
			else
			{
				// Hue toma valores entre [0,360]
				if((rgb_max == r) && (g >= b))
				{
					hue = 60.0f * (g - b) / (rgb_max - rgb_min);
				}
				else if((rgb_max == r) && (b > g))
				{
					hue = 360.0f + 60.0f * (g - b) / (rgb_max - rgb_min);
				}
				else if(rgb_max == g)
				{
					hue = 120.0f + 60.0f * (b - r) / (rgb_max - rgb_min);
				}
				else /* rgb_max == b*/
				{
					hue = 240.0f + 60.0f * (r - g) / (rgb_max - rgb_min);
				}
			}
		}

		// Atribui valores entre [0,255]
		data[i] = (unsigned char) (hue / 360.0 * 255.0);
		data[i + 1] = (unsigned char) (saturation);
		data[i + 2] = (unsigned char) (value);
	}

	return 1;
}

	//binario para nao binário


	
	/*
	int BinaryImage_NonBinary(IVC *src, IVC *dst)
	{
		// para a imagem original

		unsigned char *data = (unsigned char*)src->data;
		int width = src->width;
		int height = src->height;
		int bytesperline = src->width * src->channels;
		int channels = src->channels;
		

		//para a imagem de destino
		unsigned char *data = (unsigned char*)dst->data;
		int width = dst->width;
		int height = dst->height;
		int bytesperline = src->width * src->channels;
		int channels = src->channels;
		int bytesperline_src = src->width*src->channels;






		//+++++++++++++++++++++++++ CONDIÇÃO DA FUNÇÃO:  ++++++++++++++++++++++++++

		if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
		if (channels != 1) return 0;

		//+++++++++++++++++++++++++ CONDIÇÃO DA FUNÇÃO:  ++++++++++++++++++++++++++

		if ((dst->width <= 0) || (dst->height <= 0)) return 0;
		if (channels == 1) return 0;


	}
	*/


	int vc_binary_dilate(IVC *src, IVC *dst, int size)
	{
		unsigned char *data = (unsigned char *)src->data;
		unsigned char *datadst = (unsigned char *)dst->data;
		int width_src, width_dst, height_src, height_dst;
		int bytesperline = src->bytesperline;
		width_src = src->width;
		width_dst = dst->width;
		height_src = src->height;
		height_dst = dst->height;
		int channels = src->channels;
		int x, y, xx, yy;
		int xxyymax = (int)floor(((double)size) / 2.0);
		int xxyymin = -xxyymax;
		long int pos, posk;


		// Verifica??o de erros
		if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
		if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
		if (channels != 1) return 0;


		for (y = 0; y < height_src; y++)
		{

			for (x = 0; x < width_src; x++){

				pos = y*bytesperline + x*channels;
				int count = 0; 


				//vizinhos
				for(yy=xxyymin; yy<=xxyymax; yy++)
				{
					for (xx = xxyymin; xx <= xxyymax; xx++)
					{
						if ((y + yy >= 0) && (y + yy < height_src) && (x + xx >= 0) && (x + xx < width_src))
						{
							posk = (y + yy) * bytesperline + (x + xx)*channels;

							if (data[posk] == 255)
							{
								count++; // incrementar 1 em 'count'	
							}
						}
					}
				}
				if (count != 0)
				{
					datadst[pos] = 255; //preenche a branco
				}

				else
				{
					datadst[pos] = 0; // preenche a preto
				}

	
			}
		 
		}
		return 1;

	}


	int vc_binary_erode(IVC *src, IVC *dst, int kernel)
	{
		unsigned char *datasrc = (unsigned char *)src->data;
		unsigned char *datadst = (unsigned char *)dst->data;
		int widthsrc, widthdst, heightsrc, heightdst, channelssrc;
		int bytesperline = src->bytesperline;
		widthsrc = src->width;
		heightsrc = src->height;
		widthdst = dst->width;
		heightdst = dst->height;
		int x, y, xx, yy;
		int xxyymax = (int)floor(((double)kernel) / 2.0); //floor de double kernel ... asim se kernel 1.7 entao será 1.0 com a funcao floor
		int xxyymin = -xxyymax;
		int max, min;
		long int pos=0, posk=0;
		unsigned char threeshold;


		if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
		if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
		if (src->channels != 1) return 0;




		for (y = 0; y<heightsrc; y++)
		{
			for (x = 0; x<widthsrc; x++)
			{
				//pos = y * bytesperline + x * channels;

				int cont = 0;


				// NxM Vizinhos
				for (yy = xxyymin; yy <= xxyymax; yy++)
				{
					for (xx = xxyymin; xx <= xxyymax; xx++)
					{
						if ((y + yy >= 0) && (y + yy < heightsrc) && (x + xx >= 0) && (x + xx < widthsrc))
						{
							//posk = (y + yy) * bytesperline + (x + xx) * channels;

							if (datasrc[posk] == 0)
							{
								cont++;
							}

						}
					}
				}

				if (cont != 255)
				{
					datadst[pos] = 0;
				}

				else
				{
					datadst[pos] = 255;
				}

			}
		}
		return 1;




	}



	int vc_scale_gray_to_rgb(IVC *src, IVC *dst)
	{
		unsigned char *datasrc = (unsigned char *)src->data;
		int bytesperline_src = src->width * src->channels;
		int channels_src = src->channels;

		unsigned char *datadst = (unsigned char *)dst->data;
		int bytesperline_dst = dst->width* dst->channels;
		int channels_dst = dst->channels;

		int width = src->width;
		int heigth = src->height;

		int x, y;
		int pos_src;
		int pos_dst;
		float red, green, blue;
		int redd, greenn, bluee;
		float value;
		int d, i;
		//verificaÃ§Ã£o de erros
		if ((src->width <= 0) || (heigth <= 0) || (src->data == NULL)) return 0; //verificar "se Ã© imagem"
		if ((src->width != dst->width) || (src->height != dst->height)) return 0; //verificar tamanhos (largura e altura)
		if ((src->channels != 1) || (dst->channels != 3)) return 0; //verificar nÂº canais


		for (y = 0; y < heigth; y++)
		{
			for (x = 0; x < width; x++)
			{
				pos_src = y * bytesperline_src + x * channels_src;
				pos_dst = y * bytesperline_dst + x * channels_dst;
				value = datasrc[pos_src];

				if (value < 64)
				{
					red = 0;
					green = value * 4;
					blue = 255;
				}
				else if (value < 128)
				{
					red = 0;
					green = 255;
					blue = (255 - (value - 64) * 4);
				}
				else if (value < 192)
				{
					red = value * 4;
					green = 255;
					blue = 0;
				}
				else if (value <= 255)
				{
					red = 255;
					green = (255 - (value - 192) * 4);
					blue = 0;
				}
				/*
				for (pos_src = 0, pos_dst = 0; pos_src < bytesperline_src*heigth; pos_src++, pos_dst += 3)
				{
				for (d=0, i<0; i<64; i++, d+=4)
				{
				redd[i] = 0;
				greenn[i] = d;
				bluee[i] = 255;
				}
				}
				*/

				datadst[pos_dst] = (unsigned char)(red);
				datadst[pos_dst + 1] = (unsigned char)(green);
				datadst[pos_dst + 2] = (unsigned char)(blue);

			}
		}
		return 1;
		
	}
// Alocar memória para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *) malloc(sizeof(IVC));

	if(image == NULL) return NULL;
	if((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *) malloc(image->width * image->height * image->channels * sizeof(char));

	if(image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}


// Libertar memória de uma imagem
IVC *vc_image_free(IVC *image)
{
	if(image != NULL)
	{
		if(image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;
	
	for(;;)
	{
		while(isspace(c = getc(file)));
		if(c != '#') break;
		do c = getc(file);
		while((c != '\n') && (c != EOF));
		if(c == EOF) break;
	}
	
	t = tok;
	
	if(c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));
		
		if(c == '#') ungetc(c, file);
	}
	
	*t = 0;
	
	return tok;
}


long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);
				
				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}


void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;
				
				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}


IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;
	
	// Abre o ficheiro
	if((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if(strcmp(tok, "P4") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if(strcmp(tok, "P5") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if(strcmp(tok, "P6") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
			#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
			#endif

			fclose(file);
			return NULL;
		}
		
		if(levels == 1) // PBM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			if((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			size = image->width * image->height * image->channels;

			if((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}
		
		fclose(file);
	}
	else
	{
		#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
		#endif
	}
	
	return image;
}


int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;
	
	if(image == NULL) return 0;

	if((file = fopen(filename, "wb")) != NULL)
	{
		if(image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;
			
			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);
			
			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if(fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);
		
			if(fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				return 0;
			}
		}
		
		fclose(file);

		return 1;
	}
	
	return 0;
}
