#ifndef CODE_DEPENDENCY_H
#define CODE_DEPENDENCY_H
typedef unsigned char ui8;
typedef char si8;
typedef unsigned short ui16;
typedef short si16;
typedef unsigned int ui32;
typedef int si32;
typedef unsigned long long ui64;
typedef long long si64;


//-------------CPUID DEPENDENCY-------------//

//Vendor : AuthenticAMD
//Brand : AMD Ryzen 9 5900HX with Radeon Graphics        

//SIMD FEATURES
#define CPUID_MMX
#define CPUID_MMXEXT
#define CPUID_SSE
#define CPUID_SSE2
#define CPUID_SSE3
#define CPUID_SSE4_1
#define CPUID_SSE4_2
#define CPUID_SSE4a
#define CPUID_SSSE3
#define CPUID_AVX
#define CPUID_AVX2
//#define CPUID_AVX512
//#define CPUID_AVX512CD
//#define CPUID_AVX512ER
//#define CPUID_AVX512F
//#define CPUID_AVX512PF
//#define CPUID_3DNOW
//#define CPUID_3DNOWEXT
#define CPUID_F16C
#define CPUID_FMA
//#define CPUID_XOP
#define CPUID_POPCNT // Count of Number of Bits 1

//BIT MANUPULATION FEATURES
#define CPUID_ABM
#define CPUID_BMI1
#define CPUID_BMI2
//#define CPUID_TBM
#define CPUID_ADX
//#define CPUID_LZCNT // Count of Highest 0 bit string

//SECURITY FEATURES
#define CPUID_AES
#define CPUID_SHA

//CACHE FEATURES
#define CPUID_CLFSH
//#define CPUID_PREFETCHWT1 // Prefetch Vector Data Into Caches with Intent to Write and T1 Hint

//Transactional memory FEATURES
//#define CPUID_HLE // TSX Hardware Lock Elision
//#define CPUID_RTM

//Hardware FEATURES
#define CPUID_MONITOR // Monitor Instructions

//EXTRA PROCESSING FEATURES
#define CPUID_CMPXCHG16B // Compare and Exchange Bytes
#define CPUID_CX8 // Compare and swap
#define CPUID_MOVBE // Move Data After Swaping Bytes
#define CPUID_ERMS // Enhanced REP MOVSB/STOSB(Move Data from String to String)
#define CPUID_FSGSBASE // Access to base of %fs and %gs???
#define CPUID_FXSR // FXSAVE, FXRSTOR instructions, CR4 bit 9
#define CPUID_INVPCID // Invalidate Process-Context Identifier
#define CPUID_LAHF // Load Flags into AH Register
#define CPUID_MSR // Model Specific Register
#define CPUID_OSXSAVE // XSAVE(Save Processor Extended States) enabled by OS (https://uops.info/html-instr/XSAVE_M4608.html)
#define CPUID_PCLMULQDQ // (carry-less multiply) instruction
#define CPUID_RDRAND // Read Random Number
#define CPUID_RDSEED // Read Random SEED
//#define CPUID_RDTSCP // Read Time-Stamp Counter and Processor ID
#define CPUID_SEP // SYSENTER and SYSEXIT fast system call instructions
//#define CPUID_SYSCALL // syscall
#define CPUID_XSAVE // Extensible processor state save/restore: XSAVE, XRSTOR, XSETBV, XGETBV instructions


//-------------CUDA DEPENDENCY-------------//

#define CUDA_EXIST
//NVIDIA Device Name : NVIDIA GeForce RTX 3060 Laptop GPU

#ifdef __INTELLISENSE__
#define CUDAFUNC /##/
#else
#define CUDAFUNC   
#endif

#ifdef CUDA_EXIST
#define CUDA_SM_COUNT 30
#define CUDA_ALU_PER_SM 32
#define CUDA_GLOBALMEM_CAPACITY_MB 6143
#define CUDA_L1CACHE_CAPACITY_KB 48
#define CUDA_2D_ARRAYPITCH_KB 2097151
#define CUDA_CONSTANTMEM_CAPACITY_KB 64
#define CUDA_DEVICE_OVERLAP
#endif


//-------------DEBUG DEPENDENCY-------------//

ui32 dbgc[128] = {};
#ifdef _DEBUG
#define $dbg(C) if(IsDebuggerPresent()){C}
#else
#define $dbg(C) /##/
#endif
#endif
