#pragma once
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <windows.h>
#include "arr_expend.h"
#include "Utill_FreeMemory2.h"
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

constexpr ui32 updateSampleCount = 1 << 9;
constexpr ui32 updateSampleCount_pow2 = 9;

constexpr float Ra[2] = { 2, 0 };
constexpr float Rb[2] = { 0, 1 };
constexpr float La[2] = { 0, -2 };
constexpr float Lb[2] = { 1, 2 };

float sin01(float x) {
    //##0 ready for execute _ define constexpr values
    constexpr float A[4] = { 0.25f, -0.25f, 0.25f, -0.25f };
    constexpr float B[4] = { 0, 0.5f, -0.5f, 1.0f };//function f(x) = /\/\ domain [0~4]; y = Ax+B
    constexpr float R[4] = { 1, 1, -1, -1 };//function g(x) = __-- domain [0~4]; y = R;
    constexpr float n3 = 0.16666666f; // 1/3!
    constexpr float n5 = 0.008f; // 1/5!
    constexpr float n7 = 0.00019047619f; // 1/7!
    constexpr float n9 = 0.0000027557f; // 1/9! for tayler approximate
    //##1
    x -= (int)x; // x = x - floor(x)  |   result : {0 <= x <= 1}
    //##2
    x = 4.0f * x; // {0 <= x <= 1} -> {0 <= x <= 4}
    //##3
    ui32 index = (int)x; // selector index of A, B, R
    index &= 3;
    //##4
    x = A[index] * x;
    //##5
    x += B[index]; // x = f(x);
    //##6
    x = 6.28318530718f * x; // x = 2 * pi * x; ---- because sin(2*pi*x) repeat in {x = 0 ~ 1}
    float preresult = x;
    //##7
    float dx = x * x;
    //##8
    float fx = dx * dx;
    float x3 = dx * x;
    //##9
    float x5 = fx * x;
    float x7 = fx * x3;
    preresult -= x3 * n3;
    //##10
    float x9 = x5 * fx;
    float temp0 = x5 * n5;
    float temp1 = x7 * n7;
    //##11
    preresult += temp0;
    float temp2 = x9 * n9;
    //##12
    preresult -= temp1;
    //##13
    preresult += temp2;
    //##14
    return R[index] * preresult;

    // 14 step func, 1.35(19/14)work per 1 step.
}

float y_equal_x(float x) {
    return x;
}

__forceinline float GetPanL(float pan) {
    ui32 PanGraphindex = (int)(pan / 0.5f);
    return La[PanGraphindex] * pan + Lb[PanGraphindex];
}

__forceinline float GetPanR(float pan) {
    ui32 PanGraphindex = (int)(pan / 0.5f);
    return Ra[PanGraphindex] * pan + Rb[PanGraphindex];
}

struct AirPressureLR {
    si16 L;
    si16 R;
}; // 4byte

struct WaveDataStruct {
    AirPressureLR* SampleBuffer = nullptr;
    ui32 bufferSize = 0; // buffer size 는 무조건 2의 거듭제곱 형태 -> %대신 &로 하기 위해

    WaveDataStruct() {};

    void Alloc(ui32 buffersiz_pow2) {
        bufferSize = 1 << buffersiz_pow2;
        SampleBuffer = (AirPressureLR*)fm->_New(sizeof(AirPressureLR) * bufferSize, true);
    }

    void Release() {
        fm->_Delete(SampleBuffer, bufferSize * sizeof(AirPressureLR));
    }
};

struct WaveParameter {
    fmlcstr name;
    float data;

    void Init(const char* param_name, float init_data) {
        name.NULLState();
        name.Init(strlen(param_name) + 1, true);
        name = (char*)param_name;
        data = init_data;
    }
};

struct PRD_Note {
    float startTime;
    float endTime;
    float frequency;
};

struct WaveScale {
    si32 frequncyCount;
    ui8 mantissa;
    fmvecarr<float> table;
    static constexpr float StandardFrequency = 60;
    static constexpr float minFrequency = 20;
    static constexpr float maxFrequency = 22500;

    void Init(ui32 fc = 21, ui32 mantissa = 3) {
        frequncyCount = fc;
        mantissa = 3;
        float startF = StandardFrequency;
        table.NULLState();
        float sf = startF;
        if (mantissa <= 3) {
            startF /= mantissa;
        }
        int totalcount = 0;

        while (true) {
            if (startF * (float)mantissa < maxFrequency) {
                totalcount += frequncyCount;
                startF = startF * (float)mantissa;
                continue;
            }
            else {
                float f = 0;
                for (int i = 0; i < frequncyCount; ++i) {
                    totalcount += 1;
                    f = startF * powf(mantissa, ((float)(i)) / (float)(frequncyCount));
                    if (f >= maxFrequency) {
                        goto CALCUL_TOTALF;
                    }
                }
            }
        }

    CALCUL_TOTALF:
        table.Init(totalcount, true);
        for (int i = 0; i < totalcount; ++i) {
            table.push_back(sf * powf((float)mantissa, (float)(i) / (float)(frequncyCount)));
        }
    }

    float GetF(ui32 height, ui32 id) {
        return table[height* frequncyCount + id];
    }

    void Release() {
        table.release();
    }
};

//Piano Roll Data
struct PRD {
    fmvecarr<PRD_Note> notes;
    WaveScale scale;
};

struct Graph2D {
    fmCombinedGraph2d<float, float> data;
};

struct WaveGraph2D {
    fmCombinedGraph2d<ui8, short> data;
};

struct WaveEnvelop {
    float Attack;
    float Decay;
    float DecayVolumeRate;
    float Sustain;
    float Release;
    fmContinuousGraph_OuterFunctionMode<float, float> graph;
    WaveParameter* controlparam = nullptr;
    // Attack + Decay + Sustain -> Release Time Point.

    void Init(float attack, float decay, float decayVol, float sustain, float release) {
        Attack = attack;
        Decay = decay;
        DecayVolumeRate = decayVol;
        Sustain = sustain;
        Release = release;
    }

    void Compiled(float (*outfunc)(float), WaveParameter* param) {
        // outerfunction continuous graph
        // 0~attack : 0 ~ 1
        // attack~Decay : 1 ~ DecayVolumeRate
        // Decay ~ Sustain : DecayVolumeRate
        // Sustain ~ Release : DecayVolumeRate ~ 0
        fmvecarr<pos2f<float, float>> poses;
        poses.NULLState();
        poses.Init(5);
        poses.push_back(pos2f<float, float>(0, 0));
        poses.push_back(pos2f<float, float>(Attack, 1));
        poses.push_back(pos2f<float, float>(Attack+Decay, DecayVolumeRate));
        poses.push_back(pos2f<float, float>(Attack + Decay+Sustain, DecayVolumeRate));
        poses.push_back(pos2f<float, float>(Attack + Decay + Sustain+Release, 0));
        graph.Compile_Graph(outfunc, poses);
        graph.graphdata.clean();

        controlparam = param;
    }

    __forceinline float operator[](float time) {
        return graph[time];
    }

    __forceinline float GetReleaseTimePoint() {
        return Attack + Decay + Sustain;
    }
};

struct WaveEffector {
    lcstr name;
    fmvecarr<WaveParameter> parameters;
    AirPressureLR* (*func)(WaveEffector*, WaveDataStruct*, ui32, ui32) = nullptr;
    // parameter : effector, wavestrut, startpivot, sampleLength
};

struct WaveEffect {
    WaveEffector* origin;
    fmvecarr<float> paramdata;

    void Release() {
        origin = 0;
        paramdata.release();
    }
};

struct WaveAutomation_Instance {
    Graph2D* origin_automation;
    WaveEffect* fx;
    float startTime;
    float startCut, endCut;
    ui16 repeatNum;
};

struct MasterChannel {
    HWAVEOUT hWaveDev;
    WAVEFORMATEX wf;
    WAVEHDR hdr;
    MMTIME pivot;
    WaveDataStruct sound;
    bool exit_signal = false;
    HANDLE threadHandle = 0;

    void Init(ui32 masterbuff_sampleLength_pow2);

    void StartPlaying_MasterChannel();

    void ReleaseMaster() {
        exit_signal = true;

        while (exit_signal) {}
        CloseHandle(threadHandle);
    }
};

MasterChannel wave_master_channel;

DWORD WINAPI WaveMasterThread(LPVOID prc) {
    //wave_master_channel.Init(23); // 3minute
    wave_master_channel.StartPlaying_MasterChannel();
    return 0;
}

struct WaveChannel {
    bool is_flowing = false;
    WaveDataStruct sound;
    unsigned int samplePlayPivot = 0;
    float Volume;
    float Pan;
    fmvecarr<WaveEffect> effectContainer;
    fmvecarr<WaveChannel*> last_output_goto;
    bool sendToMaster = false;

    void Init(ui32 pow2, bool send_to_master) {
        sound.Alloc(pow2);
        samplePlayPivot = 0;
        Volume = 1;
        Pan = 0.5f;
        effectContainer.NULLState();
        effectContainer.Init(8, true);
        last_output_goto.NULLState();
        last_output_goto.Init(8, true);
        sendToMaster = send_to_master;
        is_flowing = true;
    }

    // when static sound send to channel
    void pushWave(WaveDataStruct wave) {
        ui32 Cper = sound.bufferSize - 1;
        ui32 s = samplePlayPivot + 2 * updateSampleCount;
        for (int i = 0; i < wave.bufferSize; ++i) {
            sound.SampleBuffer[(s + i) & Cper] = wave.SampleBuffer[i];
        }
    }
    // when static sound send and combine to channel
    void pushAddWave(WaveDataStruct wave) {
        ui32 Cper = sound.bufferSize - 1;
        for (int i = 0; i < wave.bufferSize; ++i) {
            int si = (samplePlayPivot + i) & Cper;
            sound.SampleBuffer[si].L += wave.SampleBuffer[i].L;
            sound.SampleBuffer[si].R += wave.SampleBuffer[i].R;
        }
    }
    
    void ApplyEffects() {
        ui32 apply_sampleLen = updateSampleCount;
        for (int i = 0; i < effectContainer.size(); ++i) {
            effectContainer.at(i).origin->func(effectContainer.at(i).origin, &sound, samplePlayPivot, apply_sampleLen);
        }

        ui32 Cper = sound.bufferSize - 1;
        for (int i = samplePlayPivot; i < samplePlayPivot + apply_sampleLen; ++i) {
            ui32 si = i & Cper;
            sound.SampleBuffer[si].L = sound.SampleBuffer[si].L * Volume * GetPanL(Pan);
            sound.SampleBuffer[si].R = sound.SampleBuffer[si].R * Volume * GetPanR(Pan);
        }
    }

    void SendToChannels() {
        ui32 apply_sampleLen = updateSampleCount;
        for (int i = 0; i < last_output_goto.size(); ++i) {
            WaveDataStruct& w = last_output_goto.at(i)->sound;
            ui32 Cper = w.bufferSize - 1;
            ui32 ocper = sound.bufferSize - 1;
            ui32 startpivot = last_output_goto.at(i)->samplePlayPivot;
            for (int i = 0; i < apply_sampleLen; ++i) {
                ui32 si = (startpivot + i) & Cper;
                ui32 oi = (samplePlayPivot + i) & ocper;
                w.SampleBuffer[si].L += sound.SampleBuffer[oi].L;
                w.SampleBuffer[si].R += sound.SampleBuffer[oi].R;
            }
        }

        if (sendToMaster) {
            WaveDataStruct& w = wave_master_channel.sound;
            ui32 Cper = w.bufferSize - 1;
            ui32 ocper = sound.bufferSize - 1;

            ui32 startpivot = wave_master_channel.pivot.u.sample >> updateSampleCount_pow2;
            startpivot += 1;
            startpivot <<= updateSampleCount_pow2;
            
            for (int i = 0; i < apply_sampleLen; ++i) {
                ui32 si = (startpivot + i) & Cper;
                ui32 oi = (samplePlayPivot + i) & ocper;
                w.SampleBuffer[si].L += sound.SampleBuffer[oi].L;
                w.SampleBuffer[si].R += sound.SampleBuffer[oi].R;
            }
        }
    }

    void ChannelStep() {
        ApplyEffects();
        SendToChannels();

        ui32 ocper = sound.bufferSize - 1;
        for (int i = samplePlayPivot; i < samplePlayPivot + updateSampleCount; ++i) {
            sound.SampleBuffer[i & ocper].L = 0;
            sound.SampleBuffer[i & ocper].R = 0;
        }
        samplePlayPivot += updateSampleCount;
        samplePlayPivot &= (sound.bufferSize - 1);
    }

    void Release() {
        sound.Release();
        for (int i = 0; i < effectContainer.size(); ++i) {
            effectContainer[i].Release();
        }
        last_output_goto.release();
    }
};

struct WaveInstrument {
    fmvecarr<WaveGraph2D*> BasicWaveTable;
    fmvecarr<WaveEnvelop> envelops;
    ui32 minimumOutputSampleLength; // release total time
    WaveParameter volume;
    WaveParameter pan;

    void(*outFunc)(WaveDataStruct, float /*frequncy;*/) = nullptr;
    //WaveChannel* send_channel;

    void Init() {
        BasicWaveTable.NULLState();
        BasicWaveTable.Init(8, true);
        envelops.NULLState();
        envelops.Init(8, true);
        WaveEnvelop envelop;
        envelop.Init(0.1f, 0.2f, 0.7f, 3.0f, 0.5f);
        envelop.Compiled(y_equal_x, nullptr);
        envelops.push_back(envelop);
        minimumOutputSampleLength = (ui32)(44100.0f * envelop.Release);
        outFunc = nullptr;
        volume.Init("volume", 1.0f);
        pan.Init("pan", 0.5f);
    }

    WaveDataStruct Result(float frequncy, float inputTime) {
        constexpr ui32 second_interval = 44100;
        WaveEnvelop volume_env = envelops.at(0);
        WaveDataStruct r;
        ui32 holdtime = (int)((float)second_interval * inputTime);
        float releaseTimePoint = volume_env.GetReleaseTimePoint();
        ui32 releaseSampleCount = (ui32)(volume_env.Release * second_interval);
        r.bufferSize = holdtime + releaseSampleCount;
        r.SampleBuffer = (AirPressureLR*)fm->_tempNew(r.bufferSize << 2);
        fm->_tempPushLayer();
        if (outFunc != nullptr) {
            outFunc(r, frequncy);
        }
        else {
            //initial instrument. (sin wave with envelop)

            //hold
            for (int i = 0; i < holdtime; ++i) {
                float _time = (float)i / (float)second_interval;
                float value = volume_env[_time] * sin01(frequncy * _time);
                r.SampleBuffer[i].L = 32767.0f * value * GetPanL(pan.data);
                r.SampleBuffer[i].R = 32767.0f * value * GetPanR(pan.data);
            }

            //release
            if (releaseTimePoint >= inputTime) {
                for (int i = holdtime; i < r.bufferSize; ++i) {
                    float _time = (float)i / (float)second_interval;
                    float _rtime = (float)(i - holdtime) / (float)second_interval;
                    float value = volume_env[_rtime + releaseTimePoint] * sin01(frequncy * _time);
                    r.SampleBuffer[i].L = 32767.0f * value * GetPanL(pan.data);
                    r.SampleBuffer[i].R = 32767.0f * value * GetPanR(pan.data);
                }
            }
        }
        fm->_tempPopLayer();
        return r;
    }
};

struct PRD_Instance {
    PRD* origin;
    WaveInstrument* instrument;
    float startTime;
    float startCut, endCut;
    unsigned short repeatNum;
};

fmvecarr<WaveChannel> channelGroup;

void MasterChannel::Init(ui32 masterbuff_sampleLength_pow2) {
    exit_signal = false;

    hdr = { NULL, };
    hdr.dwLoops = -1; // 무한 루프?

    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);

    ui32 samplelen = pow(2, masterbuff_sampleLength_pow2);
    sound.bufferSize = samplelen;
    sound.SampleBuffer = (AirPressureLR*)fm->_New(sound.bufferSize * sizeof(AirPressureLR), true);

    hdr.lpData = (char*)sound.SampleBuffer;
    hdr.dwBufferLength = sound.bufferSize * sizeof(AirPressureLR);

    channelGroup.NULLState();
    channelGroup.Init(32, true);

    threadHandle = CreateThread(NULL, 0, WaveMasterThread, 0, 0, NULL);
}

void MasterChannel::StartPlaying_MasterChannel() {
    const si32 gap = sound.bufferSize / 2;
    const ui32 per = sound.bufferSize - 1;

    si32 fs = 0;
    si32 es = 0;

    while (true) {
        // 준비 및 출력한다.
        waveOutPrepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
        waveOutWrite(hWaveDev, &hdr, sizeof(WAVEHDR));

        // 다 재생할 때까지 대기한다.
        si32 fpivot = 0;
        while (waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
            waveOutGetPosition(hWaveDev, &pivot, sizeof(MMTIME));

            es = wave_master_channel.pivot.u.sample;
            if (es - fs > updateSampleCount) {
                for (int i = 0; i < channelGroup.size(); ++i) {
                    channelGroup[i].ChannelStep();// 43.xxxHz
                }
                fs += updateSampleCount;
            }

            if (exit_signal) {
                waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
                goto WaveMasterChannel_Playing_END;
            }

            for (int i = fpivot - gap; i < (si32)pivot.u.sample - gap; ++i) {
                sound.SampleBuffer[i & per].L = 0;
                sound.SampleBuffer[i & per].R = 0;
            }
            fpivot = pivot.u.sample;
        }
        for (int i = fpivot; i < sound.bufferSize; ++i) {
            sound.SampleBuffer[i].L = 0;
            sound.SampleBuffer[i].R = 0;
        }
        // 뒷정리한다.
        waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
        cout << "master loop" << endl;
    }

WaveMasterChannel_Playing_END:
    //free(samplebuf);
    for (int i = 0; i < channelGroup.size(); ++i) {
        channelGroup[i].Release();// 43.xxxHz
    }

    fm->_Delete(sound.SampleBuffer, sound.bufferSize * sizeof(AirPressureLR));

    waveOutClose(hWaveDev);

    exit_signal = false;
}

WaveChannel* NewChannel() {
    WaveChannel channel0;
    channel0.Init(19, true);
    channelGroup.push_back(channel0);
    return &channelGroup.last();
}

// WaveOut을 한 후에도 버퍼를 바꾸면 반영이 된다.
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
    
    cout << "PlayStart" << endl;
    
    // 다 재생할 때까지 대기한다.
    while (waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
        Sleep(100);
        MMTIME pivot;
        waveOutGetPosition(hWaveDev, &pivot, sizeof(MMTIME));
        cout << pivot.u.sample << endl;//sample = 1/44100 second. in mono sound 
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

int PlayWaveData(WaveDataStruct wd) {
    HWAVEOUT hWaveDev;
    HANDLE hFile;
    DWORD filesize;
    DWORD dwRead;
    char* samplebuf;
    WAVEFORMATEX wf;
    WAVEHDR hdr = { NULL, };
    hdr.dwLoops = -1; // 무한 루프?

    // 재생 장치를 연다.
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.wBitsPerSample = 16;
    wf.nSamplesPerSec = 44100;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, (DWORD)NULL, 0, CALLBACK_NULL);

    // 헤더에 버퍼와 길이를 지정한다.
    hdr.lpData = (char*)wd.SampleBuffer;
    hdr.dwBufferLength = wd.bufferSize * 4;

    // 준비 및 출력한다.
    waveOutPrepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));
    waveOutWrite(hWaveDev, &hdr, sizeof(WAVEHDR));

    cout << "PlayStart" << endl;

    // 다 재생할 때까지 대기한다.
    while (waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
        Sleep(100);
        MMTIME pivot;
        waveOutGetPosition(hWaveDev, &pivot, sizeof(MMTIME));
        cout << pivot.u.sample << endl;//sample = 1/44100 second. in mono sound 
    }
    // 뒷정리한다.
    waveOutUnprepareHeader(hWaveDev, &hdr, sizeof(WAVEHDR));

    waveOutClose(hWaveDev);

    return 0;
}