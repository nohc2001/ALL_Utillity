#pragma once
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <windows.h>
#include "arr_expend.h"
#pragma comment(lib, "winmm.lib")

//��ǥ ->
/*
* �÷����ϴ��� ������Ű��
* ���� �÷����ϰ� �ִ� �����ε���, �ð� ���
* ���� ������ �� �����ϱ�
* 
*/

//fl studio �� ������ ������ ��.
// 1. �� ���� ���� �ִ��� Ž��
// 2. ��ó�� ����� ����� Ȯ��.
// 3. Effect�� Controller�� ����

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
    vecarr<WaveChannel*> last_output_goto; // ü�� ����� �ݿ��� ��.
};

//WaveOut�� �� �Ŀ��� ���۸� �ٲٸ� �ݿ��� �ȴ�.
// ������ �츮�� �׳� ���带 Ʋ�����, �߰��ϰ��� �ϴ� ���尡 �ִٸ� �ǹ��� �ڿ� ���带 �Է����ָ� �ȴ�.
// �ǽð� ����Ʈ�� �׳� ���ù��ۿ� ����Ʈ�� �����Ű�� �ȴ�. (����Ʈ�� �ڵ�� ����)
// ���� �Լ� ����ϴ� ������� ���� ���尡 �ִٸ�, ���� �����͸� ���� �ΰ�, ���� ���ۿ� �Լ��ؼ� �� �ִ� �����̸� �ȴ�.
// �� ���۵��� ä���̶� �����ϰ� ����� �ɰ� ����.

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
    printf("��ġ ���� = %d\n", wavenum);
    WAVEOUTCAPS cap;
    for (UINT i = 0; i < wavenum; i++) {
        waveOutGetDevCaps(i, &cap, sizeof(WAVEOUTCAPS));
        WideCharToMultiByte(CP_ACP, 0, cap.szPname, -1, devname, 128, NULL, NULL);
        printf("%d�� : %d ä��,���� ����=%x,���=%x,�̸�=%s\n",
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

    // ���̺� ������ ����.
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("file not found");
        return r;
    }

    // ��� ��ġ�� ����.
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);

    // ����� �ǳ� �ٰ� ���ۿ� ���� �����͸� �о���δ�.
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
    hdr.dwLoops = -1; // ���� ����?

    // ���̺� ������ ����.
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("file not found");
        return -1;
    }

    // ��� ��ġ�� ����.
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);

    // ����� �ǳ� �ٰ� ���ۿ� ���� �����͸� �о���δ�.
    SetFilePointer(hFile, 44, NULL, SEEK_SET);
    filesize = GetFileSize(hFile, NULL) - 44;
    samplebuf = (char*)malloc(filesize);
    ReadFile(hFile, samplebuf, filesize, &dwRead, NULL);

    // ����� ���ۿ� ���̸� �����Ѵ�.
    hdr.lpData = samplebuf;
    hdr.dwBufferLength = filesize;

    // �غ� �� ����Ѵ�.
    waveOutPrepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
    waveOutWrite(hWaveDev, &hdr, sizeof(WAVEHDR));

    // �� ����� ������ ����Ѵ�.
    while (waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
        Sleep(20);
    }

    // �������Ѵ�.
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

    // ���̺� ������ ����.
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("file not found");
        return -1;
    }

    // ��� ��ġ�� ����.
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