// InstructionSet.cpp
// Compile by using: cl /EHsc /W4 InstructionSet.cpp
// processor: x86, x64
// Uses the __cpuid intrinsic to get information about
// CPU extended instruction set support.

#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>
#include <fstream>

#include "cuda_runtime.h"
using namespace std;

class InstructionSet
{
    // forward declarations
    class InstructionSet_Internal;

public:
    // getters
    static std::string Vendor(void) { return CPU_Rep.vendor_; }
    static std::string Brand(void) { return CPU_Rep.brand_; }

    static bool SSE3(void) { return CPU_Rep.f_1_ECX_[0]; }
    static bool PCLMULQDQ(void) { return CPU_Rep.f_1_ECX_[1]; }
    static bool MONITOR(void) { return CPU_Rep.f_1_ECX_[3]; }
    static bool SSSE3(void) { return CPU_Rep.f_1_ECX_[9]; }
    static bool FMA(void) { return CPU_Rep.f_1_ECX_[12]; }
    static bool CMPXCHG16B(void) { return CPU_Rep.f_1_ECX_[13]; }
    static bool SSE41(void) { return CPU_Rep.f_1_ECX_[19]; }
    static bool SSE42(void) { return CPU_Rep.f_1_ECX_[20]; }
    static bool MOVBE(void) { return CPU_Rep.f_1_ECX_[22]; }
    static bool POPCNT(void) { return CPU_Rep.f_1_ECX_[23]; }
    static bool AES(void) { return CPU_Rep.f_1_ECX_[25]; }
    static bool XSAVE(void) { return CPU_Rep.f_1_ECX_[26]; }
    static bool OSXSAVE(void) { return CPU_Rep.f_1_ECX_[27]; }
    static bool AVX(void) { return CPU_Rep.f_1_ECX_[28]; }
    static bool F16C(void) { return CPU_Rep.f_1_ECX_[29]; }
    static bool RDRAND(void) { return CPU_Rep.f_1_ECX_[30]; }

    static bool MSR(void) { return CPU_Rep.f_1_EDX_[5]; }
    static bool CX8(void) { return CPU_Rep.f_1_EDX_[8]; }
    static bool SEP(void) { return CPU_Rep.f_1_EDX_[11]; }
    static bool CMOV(void) { return CPU_Rep.f_1_EDX_[15]; }
    static bool CLFSH(void) { return CPU_Rep.f_1_EDX_[19]; }
    static bool MMX(void) { return CPU_Rep.f_1_EDX_[23]; }
    static bool FXSR(void) { return CPU_Rep.f_1_EDX_[24]; }
    static bool SSE(void) { return CPU_Rep.f_1_EDX_[25]; }
    static bool SSE2(void) { return CPU_Rep.f_1_EDX_[26]; }

    static bool FSGSBASE(void) { return CPU_Rep.f_7_EBX_[0]; }
    static bool BMI1(void) { return CPU_Rep.f_7_EBX_[3]; }
    static bool HLE(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[4]; }
    static bool AVX2(void) { return CPU_Rep.f_7_EBX_[5]; }
    static bool BMI2(void) { return CPU_Rep.f_7_EBX_[8]; }
    static bool ERMS(void) { return CPU_Rep.f_7_EBX_[9]; }
    static bool INVPCID(void) { return CPU_Rep.f_7_EBX_[10]; }
    static bool RTM(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[11]; }
    static bool AVX512F(void) { return CPU_Rep.f_7_EBX_[16]; }
    static bool RDSEED(void) { return CPU_Rep.f_7_EBX_[18]; }
    static bool ADX(void) { return CPU_Rep.f_7_EBX_[19]; }
    static bool AVX512PF(void) { return CPU_Rep.f_7_EBX_[26]; }
    static bool AVX512ER(void) { return CPU_Rep.f_7_EBX_[27]; }
    static bool AVX512CD(void) { return CPU_Rep.f_7_EBX_[28]; }
    static bool SHA(void) { return CPU_Rep.f_7_EBX_[29]; }

    static bool PREFETCHWT1(void) { return CPU_Rep.f_7_ECX_[0]; }

    static bool LAHF(void) { return CPU_Rep.f_81_ECX_[0]; }
    static bool LZCNT(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_ECX_[5]; }
    static bool ABM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[5]; }
    static bool SSE4a(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[6]; }
    static bool XOP(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[11]; }
    static bool TBM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[21]; }

    static bool SYSCALL(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[11]; }
    static bool MMXEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[22]; }
    static bool RDTSCP(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[27]; }
    static bool _3DNOWEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[30]; }
    static bool _3DNOW(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[31]; }

private:
    static const InstructionSet_Internal CPU_Rep;

    class InstructionSet_Internal
    {
    public:
        InstructionSet_Internal()
            : nIds_{ 0 },
            nExIds_{ 0 },
            isIntel_{ false },
            isAMD_{ false },
            f_1_ECX_{ 0 },
            f_1_EDX_{ 0 },
            f_7_EBX_{ 0 },
            f_7_ECX_{ 0 },
            f_81_ECX_{ 0 },
            f_81_EDX_{ 0 },
            data_{},
            extdata_{}
        {
            //int cpuInfo[4] = {-1};
            std::array<int, 4> cpui;

            // Calling __cpuid with 0x0 as the function_id argument
            // gets the number of the highest valid function ID.
            __cpuid(cpui.data(), 0);
            nIds_ = cpui[0];

            for (int i = 0; i <= nIds_; ++i)
            {
                __cpuidex(cpui.data(), i, 0);
                data_.push_back(cpui);
            }

            // Capture vendor string
            char vendor[0x20];
            memset(vendor, 0, sizeof(vendor));
            *reinterpret_cast<int*>(vendor) = data_[0][1];
            *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
            *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
            vendor_ = vendor;
            if (vendor_ == "GenuineIntel")
            {
                isIntel_ = true;
            }
            else if (vendor_ == "AuthenticAMD")
            {
                isAMD_ = true;
            }

            // load bitset with flags for function 0x00000001
            if (nIds_ >= 1)
            {
                f_1_ECX_ = data_[1][2];
                f_1_EDX_ = data_[1][3];
            }

            // load bitset with flags for function 0x00000007
            if (nIds_ >= 7)
            {
                f_7_EBX_ = data_[7][1];
                f_7_ECX_ = data_[7][2];
            }

            // Calling __cpuid with 0x80000000 as the function_id argument
            // gets the number of the highest valid extended ID.
            __cpuid(cpui.data(), 0x80000000);
            nExIds_ = cpui[0];

            char brand[0x40];
            memset(brand, 0, sizeof(brand));

            for (int i = 0x80000000; i <= nExIds_; ++i)
            {
                __cpuidex(cpui.data(), i, 0);
                extdata_.push_back(cpui);
            }

            // load bitset with flags for function 0x80000001
            if (nExIds_ >= 0x80000001)
            {
                f_81_ECX_ = extdata_[1][2];
                f_81_EDX_ = extdata_[1][3];
            }

            // Interpret CPU brand string if reported
            if (nExIds_ >= 0x80000004)
            {
                memcpy(brand, extdata_[2].data(), sizeof(cpui));
                memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
                memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
                brand_ = brand;
            }
        };

        int nIds_;
        int nExIds_;
        std::string vendor_;
        std::string brand_;
        bool isIntel_;
        bool isAMD_;
        std::bitset<32> f_1_ECX_;
        std::bitset<32> f_1_EDX_;
        std::bitset<32> f_7_EBX_;
        std::bitset<32> f_7_ECX_;
        std::bitset<32> f_81_ECX_;
        std::bitset<32> f_81_EDX_;
        std::vector<std::array<int, 4>> data_;
        std::vector<std::array<int, 4>> extdata_;
    };
};

// Initialize static member data
const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;

void CPUID_DEFINER(ostream& os, std::string isa_feature, bool is_supported) {
    if (is_supported == false) {
        os << "//";
    }
    os << "#define CPUID_" << isa_feature;
    os << endl;
}

void WRITE_CPUID_DPENDENCY(ofstream& ofs) {
    ofs << "//-------------CPUID DEPENDENCY-------------//" << endl;
    ofs << endl;
    ofs << "//Vendor : " << InstructionSet::Vendor() << endl;
    ofs << "//Brand : " << InstructionSet::Brand() << endl;
    
    ofs << endl;
    ofs << "//SIMD FEATURES" << endl;
    CPUID_DEFINER(ofs, "MMX", InstructionSet::MMX());
    CPUID_DEFINER(ofs, "MMXEXT", InstructionSet::MMXEXT());
    CPUID_DEFINER(ofs, "SSE", InstructionSet::SSE());
    CPUID_DEFINER(ofs, "SSE2", InstructionSet::SSE2());
    CPUID_DEFINER(ofs, "SSE3", InstructionSet::SSE3());
    CPUID_DEFINER(ofs, "SSE4_1", InstructionSet::SSE41());
    CPUID_DEFINER(ofs, "SSE4_2", InstructionSet::SSE42());
    CPUID_DEFINER(ofs, "SSE4a", InstructionSet::SSE4a());
    CPUID_DEFINER(ofs, "SSSE3", InstructionSet::SSSE3());
    CPUID_DEFINER(ofs, "AVX", InstructionSet::AVX());
    CPUID_DEFINER(ofs, "AVX2", InstructionSet::AVX2());
    bool b = InstructionSet::AVX512CD();
    b = b || InstructionSet::AVX512ER();
    b = b || InstructionSet::AVX512F();
    b = b || InstructionSet::AVX512PF();
    CPUID_DEFINER(ofs, "AVX512", b);
    CPUID_DEFINER(ofs, "AVX512CD", InstructionSet::AVX512CD());
    CPUID_DEFINER(ofs, "AVX512ER", InstructionSet::AVX512ER());
    CPUID_DEFINER(ofs, "AVX512F", InstructionSet::AVX512F());
    CPUID_DEFINER(ofs, "AVX512PF", InstructionSet::AVX512PF());
    CPUID_DEFINER(ofs, "3DNOW", InstructionSet::_3DNOW());
    CPUID_DEFINER(ofs, "3DNOWEXT", InstructionSet::_3DNOWEXT());
    CPUID_DEFINER(ofs, "F16C", InstructionSet::F16C());
    CPUID_DEFINER(ofs, "FMA", InstructionSet::FMA());
    CPUID_DEFINER(ofs, "XOP", InstructionSet::XOP());
    CPUID_DEFINER(ofs, "POPCNT // Count of Number of Bits 1", InstructionSet::POPCNT());
    
    ofs << endl;
    ofs << "//BIT MANUPULATION FEATURES" << endl;
    CPUID_DEFINER(ofs, "ABM", InstructionSet::ABM());
    CPUID_DEFINER(ofs, "BMI1", InstructionSet::BMI1());
    CPUID_DEFINER(ofs, "BMI2", InstructionSet::BMI2());
    CPUID_DEFINER(ofs, "TBM", InstructionSet::TBM());
    CPUID_DEFINER(ofs, "ADX", InstructionSet::ADX());
    CPUID_DEFINER(ofs, "LZCNT // Count of Highest 0 bit string", InstructionSet::LZCNT());

    ofs << endl;
    ofs << "//SECURITY FEATURES" << endl;
    CPUID_DEFINER(ofs, "AES", InstructionSet::AES());
    CPUID_DEFINER(ofs, "SHA", InstructionSet::SHA());

    ofs << endl;
    ofs << "//CACHE FEATURES" << endl;
    CPUID_DEFINER(ofs, "CLFSH", InstructionSet::CLFSH());
    CPUID_DEFINER(ofs, "PREFETCHWT1 // Prefetch Vector Data Into Caches with Intent to Write and T1 Hint", InstructionSet::PREFETCHWT1());

    ofs << endl;
    ofs << "//Transactional memory FEATURES" << endl;
    CPUID_DEFINER(ofs, "HLE // TSX Hardware Lock Elision", InstructionSet::HLE());
    CPUID_DEFINER(ofs, "RTM", InstructionSet::RTM());

    ofs << endl;
    ofs << "//Hardware FEATURES" << endl;
    CPUID_DEFINER(ofs, "MONITOR // Monitor Instructions", InstructionSet::MONITOR());

    ofs << endl;
    ofs << "//EXTRA PROCESSING FEATURES" << endl;
    CPUID_DEFINER(ofs, "CMPXCHG16B // Compare and Exchange Bytes", InstructionSet::CMPXCHG16B());
    CPUID_DEFINER(ofs, "CX8 // Compare and swap", InstructionSet::CX8());
    CPUID_DEFINER(ofs, "MOVBE // Move Data After Swaping Bytes", InstructionSet::MOVBE());
    CPUID_DEFINER(ofs, "ERMS // Enhanced REP MOVSB/STOSB(Move Data from String to String)", InstructionSet::ERMS());
    CPUID_DEFINER(ofs, "FSGSBASE // Access to base of %fs and %gs???", InstructionSet::FSGSBASE());
    CPUID_DEFINER(ofs, "FXSR // FXSAVE, FXRSTOR instructions, CR4 bit 9", InstructionSet::FXSR());
    CPUID_DEFINER(ofs, "INVPCID // Invalidate Process-Context Identifier", InstructionSet::INVPCID());
    CPUID_DEFINER(ofs, "LAHF // Load Flags into AH Register", InstructionSet::LAHF());
    CPUID_DEFINER(ofs, "MSR // Model Specific Register", InstructionSet::MSR());
    CPUID_DEFINER(ofs, "OSXSAVE // XSAVE(Save Processor Extended States) enabled by OS (https://uops.info/html-instr/XSAVE_M4608.html)", InstructionSet::OSXSAVE());
    CPUID_DEFINER(ofs, "PCLMULQDQ // (carry-less multiply) instruction", InstructionSet::PCLMULQDQ());
    CPUID_DEFINER(ofs, "RDRAND // Read Random Number", InstructionSet::RDRAND());
    CPUID_DEFINER(ofs, "RDSEED // Read Random SEED", InstructionSet::RDSEED());
    CPUID_DEFINER(ofs, "RDTSCP // Read Time-Stamp Counter and Processor ID", InstructionSet::RDTSCP());
    CPUID_DEFINER(ofs, "SEP // SYSENTER and SYSEXIT fast system call instructions", InstructionSet::SEP());
    CPUID_DEFINER(ofs, "SYSCALL // syscall", InstructionSet::SYSCALL());
    CPUID_DEFINER(ofs, "XSAVE // Extensible processor state save/restore: XSAVE, XRSTOR, XSETBV, XGETBV instructions", InstructionSet::XSAVE());
}
// Print out supported instruction set extensions

void WRITE_CUDA_DEPENDENCY(ofstream& ofs) {
    ofs << "//-------------CUDA DEPENDENCY-------------//" << endl;
    ofs << endl;
    cudaDeviceProp prop;
    int device;
    cudaError_t cudaStatus = cudaGetDevice(&device);
    if (cudaStatus == cudaSuccess) {
        ofs << "#define CUDA_EXIST" << endl;
        cudaGetDeviceProperties(&prop, device);
        ofs << "//NVIDIA Device Name : " << prop.name << endl;
    }

    ofs << endl;

    ofs << "#ifdef __INTELLISENSE__" << endl;
    ofs << "#define CUDAFUNC /##/" << endl;
    ofs << "#else" << endl;
    ofs << "#define CUDAFUNC   " << endl;
    ofs << "#endif" << endl;

    ofs << endl;
    ofs << "#ifdef CUDA_EXIST" << endl;
    if (cudaStatus == cudaSuccess) {
        ofs << "#define CUDA_SM_COUNT " << prop.multiProcessorCount << endl;
        ofs << "#define CUDA_ALU_PER_SM " << prop.warpSize << endl;
        ofs << "#define CUDA_GLOBALMEM_CAPACITY_MB " << prop.totalGlobalMem / (1024 * 1024) << endl;
        ofs << "#define CUDA_L1CACHE_CAPACITY_KB " << prop.sharedMemPerBlock / 1024 << endl;
        ofs << "#define CUDA_2D_ARRAYPITCH_KB " << prop.memPitch / 1024 << endl;
        ofs << "#define CUDA_CONSTANTMEM_CAPACITY_KB " << prop.totalConstMem / 1024 << endl;
        if (prop.deviceOverlap == false) {
            ofs << "//";
        }
        ofs << "#define CUDA_DEVICE_OVERLAP" << endl;
    }
    else {
        ofs << "#define CUDA_SM_COUNT " << 0 << endl;
        ofs << "#define CUDA_ALU_PER_SM " << 0 << endl;
        ofs << "#define CUDA_GLOBALMEM_CAPACITY_MB " << 0 << endl;
        ofs << "#define CUDA_L1CACHE_CAPACITY_KB " << 0 << endl;
        ofs << "#define CUDA_2D_ARRAYPITCH_KB " << 0 << endl;
        ofs << "#define CUDA_CONSTANTMEM_CAPACITY_KB " << 0 << endl;
        ofs << "//#define CUDA_DEVICE_OVERLAP" << endl;
    }
    ofs << "#endif" << endl;
}

void WRITE_DEBUG_DEPENDENCY(ofstream& ofs) {
    ofs << "//-------------DEBUG DEPENDENCY-------------//" << endl;
    ofs << endl;

    ofs << "ui32 dbgc[128] = {};" << endl;
    ofs << "#ifdef _DEBUG" << endl;
    ofs << "#define $dbg(C) if(IsDebuggerPresent()){C}" << endl;
    ofs << "#else" << endl;
    ofs << "#define $dbg(C) /##/" << endl;
    ofs << "#endif" << endl;
}

int main()
{
    ofstream ofs;
    ofs.open("CodeDependency.h");

    ofs << "#ifndef CODE_DEPENDENCY_H" << endl;
    ofs << "#define CODE_DEPENDENCY_H" << endl;

    ofs << "typedef unsigned char ui8;" << endl;
    ofs << "typedef char si8;" << endl;
    ofs << "typedef unsigned short ui16;" << endl;
    ofs << "typedef short si16;" << endl;
    ofs << "typedef unsigned int ui32;" << endl;
    ofs << "typedef int si32;" << endl;
    ofs << "typedef unsigned long long ui64;" << endl;
    ofs << "typedef long long si64;" << endl;
    
    ofs << endl; ofs << endl;
    WRITE_CPUID_DPENDENCY(ofs); // cpuid dependency
    ofs << endl; ofs << endl;
    WRITE_CUDA_DEPENDENCY(ofs); // cuda dependency

    ofs << endl; ofs << endl;
    WRITE_DEBUG_DEPENDENCY(ofs); // debuging dependency

    ofs << "#endif" << endl;
}