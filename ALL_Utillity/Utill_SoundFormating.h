#pragma once
#include <iostream>

using namespace std;

namespace wavform {
#define WAVE_FORMAT_UNKNOWN 0X0000; 
#define WAVE_FORMAT_PCM 0X0001; 
#define WAVE_FORMAT_MS_ADPCM 0X0002; 
#define WAVE_FORMAT_IEEE_FLOAT 0X0003; 
#define WAVE_FORMAT_ALAW 0X0006; 
#define WAVE_FORMAT_MULAW 0X0007; 
#define WAVE_FORMAT_IMA_ADPCM 0X0011; 
#define WAVE_FORMAT_YAMAHA_ADPCM 0X0016; 
#define WAVE_FORMAT_GSM 0X0031; 
#define WAVE_FORMAT_ITU_ADPCM 0X0040; 
#define WAVE_FORMAT_MPEG 0X0050; 
#define WAVE_FORMAT_EXTENSIBLE 0XFFFE; 
#define DURATION 10 
#define SAMPLE_RATE 44100 
#define CHANNEL 2
#define BIT_RATE 32 

	typedef struct
	{
		unsigned char ChunkID[4]; // Contains the letters "RIFF" in ASCII form
		unsigned int ChunkSize; // This is the size of the rest of the chunk following this number 
		unsigned char Format[4]; // Contains the letters "WAVE" in ASCII form 
	} RIFF;
	//-------------------------------------------
	// [Channel] 
	// - streo : [left][right]
	// - 3 channel : [left][right][center] 
	// - quad : [front left][front right][rear left][reat right] 
	// - 4 channel : [left][center][right][surround] 
	// - 6 channel : [left center][left][center][right center][right][surround]
	//------------------------------------------- 
	typedef struct {
		unsigned char ChunkID[4]; // Contains the letters "fmt " in ASCII form 
		unsigned int ChunkSize; // 16 for PCM. This is the size of the rest of the Subchunk which follows this number. 
		unsigned short AudioFormat; // PCM = 1 
		unsigned short NumChannels; // Mono = 1, Stereo = 2, etc. 
		unsigned int SampleRate; // 8000, 44100, etc.	
		unsigned int AvgByteRate; // SampleRate * NumChannels * BitsPerSample/8 
		unsigned short BlockAlign; // NumChannels * BitsPerSample/8 
		unsigned short BitPerSample; // 8 bits = 8, 16 bits = 16, etc 
	} FMT;

	typedef struct {
		char ChunkID[4]; // Contains the letters "data" in ASCII form 
		unsigned int ChunkSize; // NumSamples * NumChannels * BitsPerSample/8 
	} DATA;

	typedef struct {
		RIFF Riff;
		FMT Fmt;
		DATA Data;
	} WAVE_HEADER;


	typedef struct Wav {
		const char* filename;
		int duration;
		WAVE_HEADER header;
		int sample_rate;
		int bit_rate;
		short* realData_R;
		short* realData_L;
	};

	void makeWave(const char* filename, int duration) {
		FILE* f_out;
		if (fopen_s(&f_out, filename, "wb") == 0) {
			WAVE_HEADER header;
			memcpy(header.Riff.ChunkID, "RIFF", 4);
			header.Riff.ChunkSize = duration * SAMPLE_RATE * CHANNEL * BIT_RATE / 8 + 36;
			memcpy(header.Riff.Format, "WAVE", 4);
			memcpy(header.Fmt.ChunkID, "fmt ", 4);
			header.Fmt.ChunkSize = 0x10;
			header.Fmt.AudioFormat = WAVE_FORMAT_PCM;
			header.Fmt.NumChannels = CHANNEL;
			header.Fmt.SampleRate = SAMPLE_RATE;
			header.Fmt.AvgByteRate = SAMPLE_RATE * CHANNEL * BIT_RATE / 8;
			header.Fmt.BlockAlign = CHANNEL * BIT_RATE / 8;
			header.Fmt.BitPerSample = BIT_RATE;
			memcpy(header.Data.ChunkID, "data", 4);
			header.Data.ChunkSize = duration * SAMPLE_RATE * CHANNEL * BIT_RATE / 8;
			fwrite(&header, sizeof(header), 1, f_out);
			short y[1];
			double freq = 100;
			for (int i = 0; i < SAMPLE_RATE * duration * CHANNEL * BIT_RATE / 8; i++) {
				//y[0] = (short)30000 * sin(2 * 3.141592 * i * freq / SAMPLE_RATE); // 제임스님 코멘트에 따른 수정 
				double value;
				value = (double)20000 * Peak2(0.5, 2 * 3.141592 * i * freq / SAMPLE_RATE);
				//valueLimmitiing
				if (value > 30000) value = 30000;
				if (value < -30000) value = -30000;
				y[0] = (short)value;
				fwrite(&y[0], sizeof(short), 1, f_out);
			}
			fclose(f_out);
		}
	}

	Wav LoadWave(const char* filename) {
		FILE* f_out;
		Wav wav;
		if (fopen_s(&f_out, filename, "rb") == 0) {
			WAVE_HEADER header;
			fread(&header, sizeof(header), 1, f_out);
			wav.header = header;
			wav.bit_rate = wav.header.Fmt.BitPerSample;
			wav.sample_rate = wav.header.Fmt.SampleRate;
			wav.duration = (wav.header.Data.ChunkSize * 8) / (wav.bit_rate * wav.sample_rate * header.Fmt.NumChannels);
			wav.filename = filename;

			if (wav.header.Fmt.NumChannels == 2) {
				wav.realData_R = new short[wav.header.Data.ChunkSize / 2];
				wav.realData_L = new short[wav.header.Data.ChunkSize / 2];
				for (int i = 0; i < wav.header.Data.ChunkSize; i++) {
					int n = i / 2;
					if (i % 2 == 0) {
						fread(&wav.realData_R[n], sizeof(short), 1, f_out);
					}
					else {
						fread(&wav.realData_L[n], sizeof(short), 1, f_out);

					}
				}
			}
			else {
				wav.realData_R = new short[wav.header.Data.ChunkSize];
				for (int i = 0; i < wav.header.Data.ChunkSize; i++) {
					fread(&wav.realData_R[i], sizeof(int), 1, f_out);
				}
			}
			printf("Read\n");
			fclose(f_out);
		}
		return wav;
	}
}


