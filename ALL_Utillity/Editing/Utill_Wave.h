#pragma once
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

//��ǥ ->
/*
* �÷����ϴ��� ������Ű��
* ���� �÷����ϰ� �ִ� �����ε���, �ð� ���
* ���� ������ �� �����ϱ�
* 
*/

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

int PlaySoundFile(const wchar_t* filename)
{
    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD filesize;
    DWORD dwRead;
    char* samplebuf;
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