#pragma once
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

//목표 ->
/*
* 플레이하던거 정지시키기
* 현재 플레이하고 있는 샘플인덱스, 시간 얻기
* 버퍼 데이터 얻어서 수정하기
* 
*/

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

int PlaySoundFile(const wchar_t* filename)
{
    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD filesize;
    DWORD dwRead;
    char* samplebuf;
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
    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD dwRead;
    char* samplebuf;
    DWORD bufsize;
    WAVEFORMATEX wf;
    WAVEHDR hdr = { NULL, };
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);
    SetFilePointer(hFile, 44, NULL, SEEK_SET);

    bufsize = wf.nAvgBytesPerSec;
    samplebuf = (char*)malloc(bufsize);
    hdr.lpData = samplebuf;

    do {
        ReadFile(hFile, samplebuf, bufsize, &dwRead, NULL);
        printf("Read %d\n", dwRead);
        hdr.dwBufferLength = dwRead;
        waveOutPrepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
        waveOutWrite(hWaveDev, &hdr, sizeof(WAVEHDR));

        Sleep(1000);
    } while (dwRead == bufsize);

    waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));

    free(samplebuf);

    waveOutClose(hWaveDev);

    CloseHandle(hFile);

    return 0;
}