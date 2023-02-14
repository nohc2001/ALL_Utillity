#pragma once
#include <iostream>
#include "Utill_FreeMemory.h"

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

	class WaveController {
	public:
		freemem::FM_Model0 wave_memory;
		int wave_mem_size = 52920000; // 5 minute

		static constexpr int sample_rate = 44100; // 88200 = 86KB
		static constexpr int channel_num = 2;
		static constexpr int sample_size = 2; // short
		static constexpr int max_pressure = 32767;

		float max_sec = 0;

		WaveController() {
			wave_memory.SetHeapData(new byte8[wave_mem_size], wave_mem_size);
		}

		virtual ~WaveController() {

		}

		void InitSec(float MaxSec) {
			max_sec = MaxSec;
			wave_memory.Fup = (int)(MaxSec * (float)(sample_rate * channel_num * sample_size));
		}

		float GetValue(int i, int channel) {
			int re = (short)wave_memory.Data[i * 4 + channel * 2];
			float f = (float)re / (float)max_pressure;
			return f;
		}

		void SetValue(int i, int channel, float value) {
			int re = (int)(value * (float)max_pressure);
			short* v = (short*)&wave_memory.Data[i * 4 + channel * 2];
			*v = (short)re;
		}

		void MakeWaveFile(const char* filename) {
			FILE* f_out;
			if (fopen_s(&f_out, filename, "wb") == 0) {
				WAVE_HEADER header;
				memcpy(header.Riff.ChunkID, "RIFF", 4);
				header.Riff.ChunkSize = (int)(max_sec * (float)(SAMPLE_RATE * CHANNEL * BIT_RATE / 8)) + 36;
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
				header.Data.ChunkSize = (int)(max_sec * (float)(SAMPLE_RATE * CHANNEL * BIT_RATE / 8));
				fwrite(&header, sizeof(header), 1, f_out);
				short y[1];
				double freq = 100;
				for (int i = 0; i < (int)(max_sec * (float)(SAMPLE_RATE * CHANNEL * BIT_RATE / 8)); i++) {
					//y[0] = (short)30000 * sin(2 * 3.141592 * i * freq / SAMPLE_RATE); // 제임스님 코멘트에 따른 수정 
					double value;
					value = (double)32767 * GetValue(i / 2, i % 2);
					//valueLimmitiing
					if (value > 32767) value = 32767;
					if (value < -32766) value = -32766;
					y[0] = (short)value;
					fwrite(&y[0], sizeof(short), 1, f_out);
				}
				fclose(f_out);
			}
		}

		void LoadWaveFile(const char* filename) {
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
					InitSec(wav.duration);
					for (int i = 0; i < wav.header.Data.ChunkSize; i++) {
						int n = i / 2;
						short sh = 0;
						fread(&sh, sizeof(short), 1, f_out);
						SetValue(n, i % 2, sh);
					}
				}
				printf("Read\n");
				fclose(f_out);
			}
		}
	};
}


