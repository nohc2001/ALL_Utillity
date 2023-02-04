#pragma once
#include <complex>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <queue>
#include <valarray>
#include "Utill_FreeMemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> 

using namespace freemem;

namespace reform {
#define boyut 128

	const double PI = 3.14159265358979323846;

	typedef std::complex<double> Complex;
	typedef std::valarray<Complex> CArray;

	constexpr int fmsize = 1024 * 1024; // 1MB
	static freemem::FM_Model0 reformFM;

	inline void reformInit() {
		reformFM.SetHeapData(new byte8[fmsize], fmsize);
	}

	// Function to reverse bits of num
	inline unsigned int reverseBits(unsigned int num, int bits)
	{
		unsigned int reverse_num = 0;
		for (int i = 0; i < bits; i++)
		{
			if ((num & (1 << i)))
				reverse_num |= 1 << ((bits - 1) - i);
		}
		return reverse_num;
	}

	// Iterative in-place bit-reversed Cooley-Tukey FFT
	inline void fft(CArray& x)
	{
		const size_t N = x.size();
		const size_t M = log2(N);

		// bit-reversed permutation
		for (size_t i = 0; i < N; i++)
		{
			size_t j = reverseBits(i, M);
			if (i < j) std::swap(x[i], x[j]);
		}

		// butterfly updates
		for (size_t s = 1; s <= M; s++)
		{
			size_t m = 1 << s;
			Complex wm = std::polar(1.0, -2 * PI / m);

			for (size_t k = 0; k < N; k += m)
			{
				Complex w = 1;
				for (size_t j = 0; j < m / 2; j++)
				{
					Complex t = w * x[k + j + m / 2];
					Complex u = x[k + j];

					x[k + j] = u + t;
					x[k + j + m / 2] = u - t;
					w *= wm;
				}
			}
		}
	}

	// Iterative in-place bit-reversed Cooley-Tukey IFFT
	inline void ifft(CArray& x)
	{
		// conjugate the complex numbers
		x = x.apply(std::conj);

		// forward fft
		fft(x);

		// conjugate the complex numbers again
		x = x.apply(std::conj);

		// scale the numbers
		x /= x.size();
	}

	// Fast Convolution
	inline void fastConvolusion(CArray& a, CArray& b, Complex* resultdata_out, int rsiz) {
		reformFM.ClearAll();
		int maxsiz = (a.size() > b.size()) ? a.size() : b.size();

		Complex* adata = (Complex*)reformFM._New(maxsiz * sizeof(Complex));
		Complex* bdata = (Complex*)reformFM._New(maxsiz * sizeof(Complex));
		for (int i = 0; i < maxsiz; ++i) {
			adata[i] = complex<double>();
			bdata[i] = complex<double>();
			adata[i] = a[i];
			bdata[i] = b[i];
		}
		CArray aarr(adata, maxsiz);
		CArray barr(adata, maxsiz);
		fft(aarr);
		fft(barr);

		for (int i = 0; i < maxsiz; ++i) {
			aarr[i] = aarr[i] * barr[i];
		}
		ifft(aarr);

		int m = 0;
		if (maxsiz >= rsiz) m = maxsiz;
		else m = rsiz;
		for (int i = 0; i < m; ++i) {
			resultdata_out[i] = aarr[i];
		}
	}

	//huffman Coding
	typedef struct agacDugum {
		float frekans;
		char c;
		char code[boyut];
		struct agacDugum* sol;
		struct agacDugum* sag;

	} agacDugums;

	char toupper(char c) {
		char add = 'A' - 'a';
		return c + add;
	}

	void agacTara(agacDugums* node, char* key) {
		if (node != NULL) {
			if (node->c >= 'A' && node->c <= 'Z') {
				// printf("\t%c - \tkodu: %s\n", node->c, node->code);
				strncat_s(key, 1000, &node->c, 1);
				strcat_s(key, 1000, node->code);
			}
			agacTara(node->sol, key);
			agacTara(node->sag, key);
		}
	}

	void agacTara_ASCII(agacDugums* node, char* key) {
		if (node != NULL) {
			strncat_s(key, 1000, &node->c, 1);
			strcat_s(key, 1000, node->code);
			if (node->sol != NULL) {
				agacTara_ASCII(node->sol, key);
			}

			if (node->sag != NULL) {
				agacTara_ASCII(node->sag, key);
			}
		}
	}

	void minBulma(agacDugums* d[], float* min, int* minIndex, float* ikinciMin, int* ikinciMinIndex) {
		int i, k;
		k = 0;
		*minIndex = -1;

		while (k < 26 && d[k] == NULL) k++;

		*minIndex = k;
		*min = d[k]->frekans;

		for (i = k; i < 26; i++) {
			if (d[i] != NULL && d[i]->frekans < *min) {
				*min = d[i]->frekans;
				*minIndex = i;
			}
		}

		k = 0;
		*ikinciMinIndex = -1;

		while ((k < 26 && d[k] == NULL) || (k == *minIndex && d[k] != NULL))
			k++;

		*ikinciMin = d[k]->frekans;
		*ikinciMinIndex = k;

		if (k == *minIndex)
			k++;

		for (i = k; i < 26; i++) {
			if (d[i] != NULL && d[i]->frekans < *ikinciMin && i != *minIndex) {
				*ikinciMin = d[i]->frekans;
				*ikinciMinIndex = i;
			}
		}
	}

	void minBulma_ASCII(agacDugums* d[], float* min, int* minIndex, float* ikinciMin, int* ikinciMinIndex) {
		int i, k;
		k = 0;
		*minIndex = -1;

		while (k < 256 && d[k] == NULL) k++;

		*minIndex = k;
		*min = d[k]->frekans;

		for (i = k; i < 256; i++) {
			if (d[i] != NULL && d[i]->frekans < *min) {
				*min = d[i]->frekans;
				*minIndex = i;
			}
		}

		k = 0;
		*ikinciMinIndex = -1;

		while ((k < 256 && d[k] == NULL) || (k == *minIndex && d[k] != NULL))
			k++;

		*ikinciMin = d[k]->frekans;
		*ikinciMinIndex = k;

		if (k == *minIndex)
			k++;

		for (i = k; i < 256; i++) {
			if (d[i] != NULL && d[i]->frekans < *ikinciMin && i != *minIndex) {
				*ikinciMin = d[i]->frekans;
				*ikinciMinIndex = i;
			}
		}
	}

	void encode(agacDugums* node, agacDugums** harf, char yon, short seviye, char* code) {
		int n;
		if (node != NULL) {
			if ((n = strlen(code)) < seviye) {
				if (yon == 'R') {
					strcat_s(code, boyut, "1");
				}
				else {
					if (yon == 'L') {
						strcat_s(code, boyut, "0");
					}
				}
			}
			else {
				if (n >= seviye) {
					int index = n - (n - seviye) - 1;
					if (index >= 0 && index < 128) {
						code[index] = 0;
					}

					if (yon == 'R') {
						strcat_s(code, boyut, "1");
					}
					else {
						if (yon == 'L') {
							strcat_s(code, boyut, "0");
						}
					}
				}
			}
			if (node->c >= 'A' && node->c <= 'Z') {
				strcpy_s(node->code, 128, code);
				strcpy_s(harf[node->c - 'A']->code, 128, code);
			}
			encode(node->sol, harf, 'L', seviye + 1, code);
			encode(node->sag, harf, 'R', seviye + 1, code);
		}
	}

	void encode_ASCII(agacDugums* node, agacDugums** harf, char yon, short seviye, char* code) {
		int n;
		if (node != NULL) {
			if ((n = strlen(code)) < seviye) {
				if (yon == 'R') {
					strcat_s(code, boyut, "1");
				}
				else {
					if (yon == 'L') {
						strcat_s(code, boyut, "0");
					}
				}
			}
			else {
				if (n >= seviye) {
					int index = n - (n - seviye) - 1;
					if (index >= 0 && index < 128) {
						code[index] = 0;
					}

					if (yon == 'R') {
						strcat_s(code, boyut, "1");
					}
					else {
						if (yon == 'L') {
							strcat_s(code, boyut, "0");
						}
					}
				}
			}
			strcpy_s(node->code, 128, code);

			if (harf[node->c] != NULL) {
				strcpy_s(harf[node->c]->code, 128, code);
			}

			if (node->sol != NULL) {
				encode_ASCII(node->sol, harf, 'L', seviye + 1, code);
			}

			if (node->sag != NULL) {
				encode_ASCII(node->sag, harf, 'R', seviye + 1, code);
			}
		}
	}

	void huffman_alphabet(char* inTxt, char* outDosya) {
		char* key = (char*)malloc(1000);
		key[0] = '>'; key[1] = '\0';
		agacDugums* d[26], * harf[26];
		char str[boyut];
		int kontrol;
		int  k, ind;
		float min, ikinciMin;
		int minIndex, ikinciMinIndex;
		int karakterSayi = 0;
		agacDugums* agac;

		FILE* in;
		fopen_s(&in, inTxt, "r");
		FILE* out;

		int i;
		for (i = 'A'; i <= 'Z'; i++) {
			ind = i - 'A';
			d[ind] = NULL;
		}


		int strUzunluk;
		karakterSayi = 0;
		kontrol = fgets(str, boyut, in) != NULL;

		while (!feof(in) || kontrol) {
			strUzunluk = strlen(str);
			for (i = 0; i < strUzunluk; i++) {
				str[i] = toupper(str[i]);
				if (str[i] >= 'A' && str[i] <= 'Z') {
					karakterSayi++;
					ind = str[i] - 'A';
					if (d[ind] == NULL) {
						d[ind] = (agacDugums*)malloc(sizeof(agacDugums));
						d[ind]->c = str[i];
						d[ind]->frekans = 1;
						d[ind]->sol = d[ind]->sag = NULL;
					}
					else {
						d[ind]->frekans += 1;
					}
				}
			}
			if (kontrol) {
				kontrol = fgets(str, boyut, in) != NULL;
			}
		}

		fclose(in);

		for (i = 0, strUzunluk = 0; i < 26; i++) {
			harf[i] = d[i];
			if (d[i] != NULL) {
				d[i]->frekans /= karakterSayi;

				strUzunluk++;
			}
		}

		int j = 1;
		do {
			minBulma(d, &min, &minIndex, &ikinciMin, &ikinciMinIndex);

			if (minIndex != -1 && ikinciMinIndex != -1 && minIndex != ikinciMinIndex) {
				agacDugums* gecici;
				agac = (agacDugums*)malloc(sizeof(agacDugums));
				agac->frekans = d[minIndex]->frekans + d[ikinciMinIndex]->frekans;
				agac->c = j;
				agac->sol = d[minIndex];
				gecici = (agacDugums*)malloc(sizeof(agacDugums));
				gecici->c = d[ikinciMinIndex]->c;
				gecici->frekans = d[ikinciMinIndex]->frekans;
				gecici->sol = d[ikinciMinIndex]->sol;
				gecici->sag = d[ikinciMinIndex]->sag;
				agac->sag = gecici;

				d[minIndex] = agac;
				d[ikinciMinIndex] = NULL;
			}
			j++;
		} while (j < strUzunluk);

		for (i = 0; i < 26; i++) {
			if (d[i] != NULL) {
				char code[boyut] = "";
				//strcpy_s(code, 128, "");
				encode(agac = d[i], harf, 0, 0, code);
				agacTara(d[i], key);
				break;
			}
		}
		fopen_s(&in, inTxt, "r");
		fopen_s(&out, outDosya, "w");
		kontrol = fgets(str, boyut, in) != NULL;
		while (!feof(in) || kontrol) {
			strUzunluk = strlen(str);
			for (i = 0; i < strUzunluk; i++) {
				str[i] = toupper(str[i]);
				if (str[i] >= 'A' && str[i] <= 'Z') {
					ind = str[i] - 'A';
					fputs(harf[ind]->code, out);
				}
			}
			if (kontrol) {
				kontrol = fgets(str, boyut, in) != NULL;
			}
		}

		fclose(in);
		fclose(out);

		//printf("\nbefore --> %d bit\n", karakterSayi * 8);

		fopen_s(&out, outDosya, "r");
		kontrol = fgets(str, boyut, out) != NULL;
		karakterSayi = 0;
		while (!feof(out) || kontrol) {
			karakterSayi += strlen(str);
			if (kontrol) {
				kontrol = fgets(str, boyut, out) != NULL;
			}
		}
		fclose(out);

		// printf("after  --> %d bit\n", karakterSayi);

		fopen_s(&out, outDosya, "r");
		kontrol = fgets(str, boyut, out) != NULL;
		while (!feof(out) || kontrol) {
			strUzunluk = strlen(str);
			if (kontrol) {
				kontrol = fgets(str, boyut, out) != NULL;
			}
		}
		fclose(out);

		FILE* savav;
		fopen_s(&savav, outDosya, "a");

		fputs(key, savav);
		fclose(savav);
	}

	void huffman_ASCII(char* inTxt, char* outDosya) {
		constexpr int byteCases = 256;
		char* key = (char*)malloc(1000);
		key[0] = '>'; key[1] = '\0';
		agacDugums* d[byteCases], * harf[byteCases];
		char str[boyut];
		int kontrol;
		int  k, ind;
		float min, ikinciMin;
		int minIndex, ikinciMinIndex;
		int karakterSayi = 0;
		agacDugums* agac;

		FILE* in;
		fopen_s(&in, inTxt, "r");
		FILE* out;

		int i;
		for (i = 0; i < byteCases; i++) {
			d[i] = NULL;
		}


		int strUzunluk;
		karakterSayi = 0;
		kontrol = fgets(str, boyut, in) != NULL;

		while (!feof(in) || kontrol) {
			strUzunluk = strlen(str);
			for (i = 0; i < strUzunluk; i++) {
				//str[i] = toupper(str[i]);
				karakterSayi++;
				ind = str[i];
				if (d[ind] == NULL) {
					d[ind] = (agacDugums*)malloc(sizeof(agacDugums));
					d[ind]->c = str[i];
					d[ind]->frekans = 1;
					d[ind]->sol = d[ind]->sag = NULL;
					d[ind]->code[0] = 0;
				}
				else {
					d[ind]->frekans += 1;
				}
			}
			if (kontrol) {
				kontrol = fgets(str, boyut, in) != NULL;
			}
		}

		fclose(in);

		for (i = 0, strUzunluk = 0; i < byteCases; i++) {
			harf[i] = d[i];
			if (d[i] != NULL) {
				d[i]->frekans /= karakterSayi;

				strUzunluk++;
			}
		}

		int j = 1;
		do {
			minBulma_ASCII(d, &min, &minIndex, &ikinciMin, &ikinciMinIndex);

			if (minIndex != -1 && ikinciMinIndex != -1 && minIndex != ikinciMinIndex) {
				agacDugums* gecici;
				agac = (agacDugums*)malloc(sizeof(agacDugums));
				agac->frekans = d[minIndex]->frekans + d[ikinciMinIndex]->frekans;
				agac->c = j;
				agac->sol = d[minIndex];
				gecici = (agacDugums*)malloc(sizeof(agacDugums));
				gecici->c = d[ikinciMinIndex]->c;
				gecici->frekans = d[ikinciMinIndex]->frekans;
				gecici->sol = d[ikinciMinIndex]->sol;
				gecici->sag = d[ikinciMinIndex]->sag;
				agac->sag = gecici;

				d[minIndex] = agac;
				d[ikinciMinIndex] = NULL;
			}
			j++;
		} while (j < strUzunluk);

		for (i = 0; i < byteCases; i++) {
			if (d[i] != NULL) {
				char code[boyut] = "";
				//strcpy_s(code, 128, "");
				d[i]->code[0] = 0;
				encode_ASCII(agac = d[i], harf, 0, 0, code);
				agacTara_ASCII(d[i], key);
				break;
			}
		}
		fopen_s(&in, inTxt, "r");
		fopen_s(&out, outDosya, "w");
		kontrol = fgets(str, boyut, in) != NULL;
		while (!feof(in) || kontrol) {
			strUzunluk = strlen(str);
			for (i = 0; i < strUzunluk; i++) {
				ind = str[i];
				fputs(harf[ind]->code, out);
			}
			if (kontrol) {
				kontrol = fgets(str, boyut, in) != NULL;
			}
		}

		fclose(in);
		fclose(out);

		//printf("\nbefore --> %d bit\n", karakterSayi * 8);

		fopen_s(&out, outDosya, "r");
		kontrol = fgets(str, boyut, out) != NULL;
		karakterSayi = 0;
		while (!feof(out) || kontrol) {
			karakterSayi += strlen(str);
			if (kontrol) {
				kontrol = fgets(str, boyut, out) != NULL;
			}
		}
		fclose(out);

		// printf("after  --> %d bit\n", karakterSayi);

		fopen_s(&out, outDosya, "r");
		kontrol = fgets(str, boyut, out) != NULL;
		while (!feof(out) || kontrol) {
			strUzunluk = strlen(str);
			if (kontrol) {
				kontrol = fgets(str, boyut, out) != NULL;
			}
		}
		fclose(out);

		FILE* savav;
		fopen_s(&savav, outDosya, "a");

		fputs(key, savav);
		fclose(savav);
	}


	//LZ77 Compress
	typedef struct Data {
		int offset;
		int length;
		char ch;

	}Data;

	void yaz(Data sonuc, FILE* out) {
		short int kayan = sonuc.offset << 6;
		short int off_len = kayan + sonuc.length;
		fwrite(&off_len, sizeof(off_len), 1, out);
		fwrite(&sonuc.ch, 1, sizeof(sonuc.ch), out);
	}

	float DosyaBoyut(char* metinTxt) {

		float DosyaBoyut;

		FILE* dosya;
		fopen_s(&dosya, metinTxt, "rb");
		fseek(dosya, 0, SEEK_END);
		DosyaBoyut = ftell(dosya);
		fclose(dosya);
		return DosyaBoyut * 0.000977;
	}

	Data encoder(char arama[], int aramaUzunluk, char ileri[], int ileriUzunluk) {
		Data datas;
		if (aramaUzunluk == 0) {
			datas.offset = 0;
			datas.length = 0;
			datas.ch = ileri[0];
			return datas;
		}

		if (ileriUzunluk == 0) {
			datas.offset = -1;
			datas.length = -1;
			datas.ch = ' ';
			return datas;
		}


		int en_benzerlik = 0;
		int en_uzaklik = 0;

		int birlesimDiziUzunluk = aramaUzunluk + ileriUzunluk; // birlesim dizisi uzunluk
		char* birlesimDizi = (char*)malloc(birlesimDiziUzunluk); // iki dizinin birlestirilecek dizi

		//***********************************************************

		//iki diziyi birlestirme islemi

		memcpy(birlesimDizi, arama, aramaUzunluk);
		memcpy(birlesimDizi + aramaUzunluk, ileri, ileriUzunluk);



		//***********************************************************

		//Arama dizisi ve ileri dizisi

		//printf("search : ' ");
		//int i;
		//for(i=0; i<aramaUzunluk; i++)
		//    printf("%c",arama[i]);
		//printf(" ' ->");
		//printf("look a head : ");
		//for(i=0; i<ileriUzunluk; i++)
		//    printf("%c",ileri[i]);

		//***********************************************************

		int arama_indis = aramaUzunluk;
		int p;
		for (p = 0; p < aramaUzunluk; p++) {

			int benzerlik = 0;
			// Benzerlik kontrol 
			while (birlesimDizi[p + benzerlik] == birlesimDizi[arama_indis + benzerlik]) {

				benzerlik = benzerlik + 1;

				if ((arama_indis + benzerlik) == birlesimDiziUzunluk) {
					benzerlik = benzerlik - 1;
					break;
				}
				if ((p + benzerlik) >= arama_indis) {
					break;
				}

			}
			// benzerlik ve uzaklik karsilastirma
			if (benzerlik > en_benzerlik) {
				en_uzaklik = p;
				en_benzerlik = benzerlik;
			}


		}           // Elde edilen verilen struck yapisi sayesinde coklu donduruluyor
		datas.offset = en_uzaklik;
		datas.length = en_benzerlik;
		datas.ch = birlesimDizi[arama_indis + en_benzerlik];
		return datas;
	}

	Data encoder(freemem::BitArray arama, int aramaUzunluk, freemem::BitArray ileri, int ileriUzunluk) {
		Data datas;
		if (aramaUzunluk == 0) {
			datas.offset = 0;
			datas.length = 0;
			if (ileri.getbit(0)) {
				datas.ch = '1';
			}
			else {
				datas.ch = '0';
			}
			
			return datas;
		}

		if (ileriUzunluk == 0) {
			datas.offset = -1;
			datas.length = -1;
			datas.ch = ' ';
			return datas;
		}


		int en_benzerlik = 0;
		int en_uzaklik = 0;

		int birlesimDiziUzunluk = aramaUzunluk + ileriUzunluk; // birlesim dizisi uzunluk
		FM_Model0 birlesimDizi_fm = FM_Model0();
		birlesimDizi_fm.SetHeapData(new byte8[birlesimDiziUzunluk + 1], birlesimDiziUzunluk + 1);
		freemem::BitArray bit_birlesimDizi = freemem::BitArray((FM_Model*)&birlesimDizi_fm, birlesimDiziUzunluk);
		bit_birlesimDizi.SetUp(birlesimDiziUzunluk);
		//char* birlesimDizi = (char*)malloc(birlesimDiziUzunluk); // iki dizinin birlestirilecek dizi

		//***********************************************************

		//iki diziyi birlestirme islemi

		for (int i = 0; i < aramaUzunluk; ++i) {
			bit_birlesimDizi.setbit(i, arama.getbit(i));
		}

		for (int i = aramaUzunluk; i < birlesimDiziUzunluk; ++i) {
			int n = i - aramaUzunluk;
			bit_birlesimDizi.setbit(i, ileri.getbit(n));
		}

		//***********************************************************

		//Arama dizisi ve ileri dizisi

		//printf("search : ' ");
		//int i;
		//for(i=0; i<aramaUzunluk; i++)
		//    printf("%c",arama[i]);
		//printf(" ' ->");
		//printf("look a head : ");
		//for(i=0; i<ileriUzunluk; i++)
		//    printf("%c",ileri[i]);

		//***********************************************************

		int arama_indis = aramaUzunluk;
		int p;
		for (p = 0; p < aramaUzunluk; p++) {

			int benzerlik = 0;
			// Benzerlik kontrol 
			while (bit_birlesimDizi.getbit(p + benzerlik) == bit_birlesimDizi.getbit(arama_indis + benzerlik)) {

				benzerlik = benzerlik + 1;

				if ((arama_indis + benzerlik) == birlesimDiziUzunluk) {
					benzerlik = benzerlik - 1;
					break;
				}
				if ((p + benzerlik) >= arama_indis) {
					break;
				}

			}
			// benzerlik ve uzaklik karsilastirma
			if (benzerlik > en_benzerlik) {
				en_uzaklik = p;
				en_benzerlik = benzerlik;
			}


		}           
		// Elde edilen verilen struck yapisi sayesinde coklu donduruluyor


		datas.offset = en_uzaklik;
		datas.length = en_benzerlik;
		if (bit_birlesimDizi.getbit(arama_indis + en_benzerlik)) {
			datas.ch = '1';
		}
		else {
			datas.ch = '0';
		}
		
		return datas;
	}

	void lz77encoder(char* txt, char* outTxt) {
		Data sonuclar;
		int x = 16;
		int maxArama = 1024;

		int maxlh = pow(2, (x - (log2(maxArama))));
		//***********************************************************
		FILE* metinBelgesi; // Okunacak metin belgesi
		fopen_s(&metinBelgesi, txt, "r");

		FILE* yazilandosya; // Sikistirilmamis dosya
		fopen_s(&yazilandosya, outTxt, "wb");

		int uzunluk = -1; // Txt Uzunlugu
		FILE* f;
		char ch;
		fopen_s(&f, txt, "r");
		while (feof(f) == NULL) {
			ch = getc(f);
			uzunluk++;
		}
		fclose(f);
		//***********************************************************
		// Metini karakter karakter okuma islemi ve diziye atama

		char  karakter;
		FM_Model0 metin_fm = FM_Model0();
		metin_fm.SetHeapData(new byte8[uzunluk+1], uzunluk+1);
		freemem::BitArray bit_metin = freemem::BitArray((FM_Model*)&metin_fm, uzunluk);
		bit_metin.SetUp(uzunluk);
		//char* metin = (char*)malloc(uzunluk);
		karakter = fgetc(metinBelgesi);
		int id = 0;
		while (karakter != EOF) {
			//metin[id] = karakter;
			bit_metin.setbit(id, karakter-'0');
			karakter = fgetc(metinBelgesi);
			id++;
		}
		fclose(metinBelgesi);
		//***********************************************************
		int aramaIterator = 0;
		int ileriIterator = 0;
		int control = 0;
		int counter = 0;
		while (ileriIterator < uzunluk) {

			int aI = aramaIterator; // Dizilerdede baslangic yerini vermek icin bu degiskenlerde tutuyoruz
			int iL = ileriIterator; // Dizilerdede baslangic yerini vermek icin bu degiskenlerde tutuyoruz

			int bA = ileriIterator - aramaIterator; // Arama Dizisini olusturmak icin boyutunu tutuyoruz
			int bI = maxlh; // Arama Dizisini olusturmak icin boyutunu tutuyoruz


			if (bI > uzunluk) {
				if (control == 0) {
					bI = uzunluk;
					control++;
				}
				else if (control > 0) {
					bI = uzunluk - bA;
				}
			}
			else if (bI <= uzunluk) {
				control++;
				if (control > 0) {
					bI = uzunluk - bA;
				}
			}
			//**********************************************************************
			// Metin dizisinin içinden mevcut pozisyonlara gore arama dizisi ve ileri dizisini dolduruyoruz
			//char* aramaDizisi = (char*)malloc(bA);
			FM_Model0 aramaDizisi_fm = FM_Model0();
			aramaDizisi_fm.SetHeapData(new byte8[bA + 1], bA + 1);
			freemem::BitArray bit_aramaDizisi = freemem::BitArray((FM_Model*)&aramaDizisi_fm, bA);
			bit_aramaDizisi.SetUp(bA);
			int k;
			for (k = 0; k < bA; k++, aI++) {
				bit_aramaDizisi.setbit(k, bit_metin.getbit(aI));
			}

			//char* ileriDizi = (char*)malloc(bI);
			FM_Model0 ileriDizi_fm = FM_Model0();
			ileriDizi_fm.SetHeapData(new byte8[bI + 1], bI + 1);
			freemem::BitArray bit_ileriDizi = freemem::BitArray((FM_Model*)&ileriDizi_fm, bI);
			bit_ileriDizi.SetUp(bI);
			int o;
			for (o = 0; o < bI; o++, iL++) {
				if (iL >= uzunluk) {
					break;
				}
				else {
					bit_aramaDizisi.setbit(o, bit_metin.getbit(iL));
				}
			}


			//**********************************************************************
			sonuclar = encoder(bit_aramaDizisi, bA, bit_ileriDizi, bI); // Lz77 icin arama islemi yapiliyor

		   //printf("\n(%d - %d - %c)\n",sonuclar.offset,sonuclar.length,sonuclar.ch);

			yaz(sonuclar, yazilandosya);

			ileriIterator = ileriIterator + sonuclar.length + 1;
			aramaIterator = ileriIterator - maxArama;

			if (aramaIterator < 0) {
				aramaIterator = 0;
			}

		}

		fclose(yazilandosya);
	}


	//delate Compress
	void deflate(char* konum, char* inDeflate, char* deflateOut) {
		huffman_alphabet(konum, inDeflate);
		lz77encoder(inDeflate, deflateOut);
	}

	void deflate_ASCII(char* konum, char* inDeflate, char* deflateOut) {
		huffman_ASCII(konum, inDeflate);
		lz77encoder(inDeflate, deflateOut);
	}
};