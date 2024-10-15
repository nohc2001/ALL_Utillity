#pragma once
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <windows.h>
#include "arr_expend.h"
#pragma comment(lib, "winmm.lib")

//목표 ->
/*
* 플레이하던거 정지시키기
* 현재 플레이하고 있는 샘플인덱스, 시간 얻기
* 버퍼 데이터 얻어서 수정하기
* 
*/

//fl studio 의 구조를 복사한 것.
// 1. 더 나은 구조 있는지 탐색
// 2. 패처를 만들고 기능의 확장.
// 3. Effect와 Controller를 통일

struct AirPressureLR {
    short L;
    short R;
}; // 4byte

struct WaveDataStruct {
    AirPressureLR* SampleBuffer = nullptr;
    unsigned int bufferSize = 0;
};

struct WaveParameter {
    lcstr name;
    float data;
};

struct PRD_Note {
    float startTime;
    float endTime;
    unsigned int frequncy_id;
};

struct WaveScale {
    int frequncyCount;
    unsigned char mantissa;
};

//Piano Roll Data
struct PRD {
    vecarr<PRD_Note> notes;
    WaveScale scale;
};

struct Graph2D {

};

struct WaveEnvelop {
    float Attack;
    float Decay;
    float DecayVolumeRate;
    float Hold;
    float Release;
};

struct WaveSender {
    WaveDataStruct data;
    unsigned int pivot;
};

struct WaveInstrument {
    vecarr<Graph2D*> BasicWaveTable;
    vecarr<WaveEnvelop> envelops;
    WaveDataStruct(*outFunc)(float /*frequncy;*/) = nullptr;
    vecarr<WaveSender> waveSender;
    WaveDataStruct Result(unsigned int ReadLength) {
        WaveDataStruct r;
        r.bufferSize = ReadLength;
        r.SampleBuffer = (AirPressureLR*)malloc(ReadLength << 2);
        for (int i = 0; i < waveSender.size(); ++i) {
            unsigned int limit = waveSender.at(i).data.bufferSize - waveSender.at(i).pivot;
            unsigned int lmax = max(ReadLength, limit);
            if (lmax == limit) {
                // erase WaveSender;
                free(waveSender.at(i).data.SampleBuffer); // waveSender.at(i).data.bufferSize
                waveSender.erase(i);
                i--;
            }
            for (int k = 0; k < lmax; ++k) {
                AirPressureLR ap = waveSender.at(i).data.SampleBuffer[waveSender.at(i).pivot + k];
                r.SampleBuffer[k].L += ap.L;
                r.SampleBuffer[k].R += ap.R;
            }
        }
    }
};

struct WaveAutomation {
    Graph2D g;
};

struct PRD_Instance {
    PRD* origin;
    WaveInstrument* instrument;
    float startTime;
    float startCut, endCut;
    unsigned short repeatNum;
};

struct WaveEffector {
    lcstr name;
    vecarr<WaveParameter> parameters;
    AirPressureLR* (*func)(WaveEffector*, AirPressureLR*) = nullptr;
};

struct WaveEffect {
    WaveEffector* origin;
    vecarr<float> paramdata;
};

struct WaveAutomation_Instance {
    WaveAutomation* origin;
    WaveEffect* fx;
    float startTime;
    float startCut, endCut;
    unsigned short repeatNum;
};

struct WaveChannel {
    WaveDataStruct sound;
    unsigned int samplePlayPivot = 0;
    float Volume;
    float Pan;
    vecarr<WaveEffect> effectContainer;
    vecarr<WaveChannel*> last_output_goto; // 체널 출력이 반영될 곳.
};

//WaveOut을 한 후에도 버퍼를 바꾸면 반영이 된다.
// 때문에 우리는 그냥 사운드를 틀어놓고, 추가하고자 하는 사운드가 있다면 피벗의 뒤에 사운드를 입력해주면 된다.
// 실시간 이펙트는 그냥 셈플버퍼에 이펙트를 적용시키면 된다. (이펙트를 코드로 만들어서)
// 만약 게속 써야하는 배경음악 같은 사운드가 있다면, 원본 데이터를 따로 두고, 메인 버퍼에 게속해서 써 넣는 형식이면 된다.
// 이 버퍼들을 채널이라 생각하고 만들면 될것 같다.

//waveOutRestart(hWaveDev);
//waveOutPause(hWaveDev);
//waveOutGetVolume(hWaveDev, &vol);
//waveOutSetVolume(hWaveDev, vol);
//waveOutGetPosition
//waveOutGetPosition
//waveOutGetPitch
//waveOutSetPitch

typedef unsigned long long ui64;

void ShowSoundDevices()
{
    UINT wavenum;
    char devname[128];
    wavenum = waveOutGetNumDevs();
    printf("장치 개수 = %d\n", wavenum);
    WAVEOUTCAPS cap;
    for (UINT i = 0; i < wavenum; i++) {
        waveOutGetDevCaps(i, &cap, sizeof(WAVEOUTCAPS));
        WideCharToMultiByte(CP_ACP, 0, cap.szPname, -1, devname, 128, NULL, NULL);
        printf("%d번 : %d 채널,지원 포맷=%x,기능=%x,이름=%s\n",
            i, cap.wChannels, cap.dwFormats, cap.dwSupport, devname);
    }
}

WaveDataStruct CreateWaveFromFile(const wchar_t* filename) {
    WaveDataStruct r;
    r.bufferSize = 0;
    r.SampleBuffer = nullptr;

    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD filesize;
    DWORD dwRead;
    char* samplebuf;
    WAVEFORMATEX wf;

    // 웨이브 파일을 연다.
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("file not found");
        return r;
    }

    // 재생 장치를 연다.
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);

    // 헤더는 건너 뛰고 버퍼에 샘플 데이터를 읽어들인다.
    SetFilePointer(hFile, 44, NULL, SEEK_SET);
    filesize = GetFileSize(hFile, NULL) - 44;
    samplebuf = (char*)malloc(filesize);
    BOOL b = ReadFile(hFile, samplebuf, filesize, &dwRead, NULL);
    if (b) {
        r.bufferSize = filesize >> 2; // number of sample (airpressure)
        r.SampleBuffer = (AirPressureLR*)samplebuf;
    }
    return r;
}

int PlaySoundFile(const wchar_t* filename)
{
    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD filesize;
    DWORD dwRead;
    char* samplebuf;
    WAVEFORMATEX wf;
    WAVEHDR hdr = { NULL, };
    hdr.dwLoops = -1; // 무한 루프?

    // 웨이브 파일을 연다.
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("file not found");
        return -1;
    }

    // 재생 장치를 연다.
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);

    // 헤더는 건너 뛰고 버퍼에 샘플 데이터를 읽어들인다.
    SetFilePointer(hFile, 44, NULL, SEEK_SET);
    filesize = GetFileSize(hFile, NULL) - 44;
    samplebuf = (char*)malloc(filesize);
    ReadFile(hFile, samplebuf, filesize, &dwRead, NULL);

    // 헤더에 버퍼와 길이를 지정한다.
    hdr.lpData = samplebuf;
    hdr.dwBufferLength = filesize;

    // 준비 및 출력한다.
    waveOutPrepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
    waveOutWrite(hWaveDev, &hdr, sizeof(WAVEHDR));

    // 다 재생할 때까지 대기한다.
    while (waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
        Sleep(20);
    }

    // 뒷정리한다.
    waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));

    free(samplebuf);

    waveOutClose(hWaveDev);

    CloseHandle(hFile);

    return 0;
}

int PlaySoundFileStream(const wchar_t* filename)
{
    static std::chrono::steady_clock::time_point et, ft;
    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD dwRead;
    char* samplebuf;
    DWORD bufsize;
    WAVEFORMATEX wf;
    WAVEHDR hdr = { NULL, };

    // 웨이브 파일을 연다.
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("file not found");
        return -1;
    }

    // 재생 장치를 연다.
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);
    SetFilePointer(hFile, 44, NULL, SEEK_SET);

    bufsize = wf.nAvgBytesPerSec;
    samplebuf = (char*)malloc(bufsize);
    hdr.lpData = samplebuf;

    et = std::chrono::high_resolution_clock::now();
    do {
        ft = et;
        BOOL succ = ReadFile(hFile, samplebuf, bufsize, &dwRead, NULL);
        if (succ) {
            printf("Read %d\n", dwRead);
            hdr.dwBufferLength = dwRead;
            waveOutPrepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
            waveOutWrite(hWaveDev, &hdr, sizeof(WAVEHDR));
            et = std::chrono::high_resolution_clock::now();
            std::chrono::duration < double >codetime = et - ft;
            double delta = codetime.count();
            while (delta < 1.0) {
                et = std::chrono::high_resolution_clock::now();
                codetime = et - ft;
                delta = codetime.count();
            }
        }
    } while (dwRead == bufsize);

    waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));

    free(samplebuf);

    waveOutClose(hWaveDev);

    CloseHandle(hFile);

    return 0;
}