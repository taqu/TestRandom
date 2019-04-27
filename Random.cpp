/**
@file Random.cpp
@author t-sakai
@date 2011/09/04
*/
#include "Random.h"

#ifdef _WIN32
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>

#else //_WIN32
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#endif //_WIN32

namespace lcore
{
    namespace
    {
#ifdef RANDOM_STANDALONE
    ClockType getPerformanceCounter()
    {
#if defined(_WIN32)
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return count.QuadPart;
#else
        clock_t t = 0;
        t = clock();
        return t;
#endif
    }

    u32 getTimeMilliSec()
    {
#if defined(_WIN32)
        DWORD time = timeGetTime();
        return static_cast<u32>(time);
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);

        return static_cast<u32>(tv.tv_sec*1000 + tv.tv_usec/1000);
#endif
    }

#endif
        u32 getRandomBasedOnDisckUsage()
        {
            u32 t = getTimeMilliSec();
            lcore::ClockType counter = getPerformanceCounter();

#ifdef _WIN32
            LARGE_INTEGER freeBytesAvalable;
            LARGE_INTEGER totalBytes;
            LARGE_INTEGER totalFreeBytes;

            u32 discBytes = 0;
            BOOL ret = GetDiskFreeSpaceEx(
                NULL,
                (PULARGE_INTEGER)&freeBytesAvalable,
                (PULARGE_INTEGER)&totalBytes,
                (PULARGE_INTEGER)&totalFreeBytes);

            if(ret){
                discBytes = totalFreeBytes.LowPart;
            }

            MEMORYSTATUSEX memstat;

            GlobalMemoryStatusEx(&memstat);
            u32 memoryBytes = static_cast<u32>(memstat.ullAvailPhys & 0xFFFFFFFFU);
#else
            struct sysinfo info;
            sysinfo(&info);
            u32 memoryBytes = static_cast<u32>(info.freeram & 0xFFFFFFFFU);

            u32 discBytes = 0;
            struct statvfs statFs;
            if(0 == statvfs("/", &statFs)){
                discBytes = static_cast<u32>(statFs.f_bfree & 0xFFFFFFFU);
            }
#endif
            u32 other = ( (discBytes<<16) | (memoryBytes & 0xFFFFU));
            return ((t<<16) | static_cast<u32>(counter&0xFFFFU)) ^ other;
        }

        u64 getRandomBasedOnDisckUsage64()
        {
            u32 t = getTimeMilliSec();
            lcore::ClockType counter = getPerformanceCounter();

#ifdef _WIN32
            LARGE_INTEGER freeBytesAvalable;
            LARGE_INTEGER totalBytes;
            LARGE_INTEGER totalFreeBytes;

            s64 discBytes = 0;
            BOOL ret = GetDiskFreeSpaceEx(
                NULL,
                (PULARGE_INTEGER)&freeBytesAvalable,
                (PULARGE_INTEGER)&totalBytes,
                (PULARGE_INTEGER)&totalFreeBytes);

            if(ret){
                discBytes = totalFreeBytes.QuadPart;
            }

            MEMORYSTATUSEX memstat;

            GlobalMemoryStatusEx(&memstat);
            u64 memoryBytes = memstat.ullAvailPhys;
#else
            struct sysinfo info;
            sysinfo(&info);
            u64 memoryBytes = static_cast<u64>(info.freeram & 0xFFFFFFFFU);

            u64 discBytes = 0;
            struct statvfs statFs;
            if(0 == statvfs("/", &statFs)){
                discBytes = static_cast<u64>(statFs.f_bfree & 0xFFFFFFFU);
            }
#endif
            u64 other = ( (discBytes<<32) | (memoryBytes & 0xFFFFFFFFU));
            return ((counter<<32) | t) ^ other;
        }

    }

    u32 scramble(u32 v, u32 i)
    {
        return (1812433253 * (v^(v >> 30)) + i);
    }

    u64 scramble(u64 v, u64 i)
    {
        return (1812433253 * (v^(v >> 41)) + i);
    }

    u32 getStaticSeed()
    {
        return 13249876;
    }

    u32 getDefaultSeed()
    {
        return getRandomBasedOnDisckUsage();
    }

    u64 getStaticSeed64()
    {
        return 1181783497276652981ULL;
    }

    u64 getDefaultSeed64()
    {
        return getRandomBasedOnDisckUsage64();
    }

    namespace
    {
        inline u32 rotl(u32 x, s32 k)
        {
            return (x << k) | (x >> (32 - k));
        }

        inline u64 rotl(u64 x, s32 k)
        {
            return (x << k) | (x >> (64 - k));
        }

        // Return (0, 1]
        inline f32 toF32_0(u32 x)
        {
            static const u32 m0 = 0x3F800000U;
            static const u32 m1 = 0x007FFFFFU;
            x = m0|(x&m1);
            return (*(f32*)&x)- 0.999999881f;
        }

        // Return [0, 1)
        inline f32 toF32_1(u32 x)
        {
            static const u32 m0 = 0x3F800000U;
            static const u32 m1 = 0x007FFFFFU;
            x = m0|(x&m1);
            return (*(f32*)&x) - 1.000000000f;
        }

        // Return [0, 1)
        inline f64 toF64(u64 x)
        {
            union U
            {
                u64 i_;
                f64 d_;
            };
            U u;
            u.i_ = u64(0x3FF) << 52 | x >> 12;
            return u.d_ - 1.0;
        }
    }

    //---------------------------------------------
    //---
    //--- Xoshiro128Star
    //---
    //---------------------------------------------
    Xoshiro128Star::Xoshiro128Star()
        :state_{123456789, 362436069, 521288629, 88675123}
    {}

    Xoshiro128Star::Xoshiro128Star(u32 seed)
    {
        srand(seed);
    }
    Xoshiro128Star::~Xoshiro128Star()
    {}

    void Xoshiro128Star::srand(u32 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (1812433253 * (state_[i-1]^(state_[i-1] >> 30)) + i);
        }
    }

    u32 Xoshiro128Star::rand()
    {
        const u32 result = rotl(state_[0] * 5, 7) * 9;

        const u32 t = state_[1] << 9;

        state_[2] ^= state_[0];
        state_[3] ^= state_[1];
        state_[1] ^= state_[2];
        state_[0] ^= state_[3];

        state_[2] ^= t;

        state_[3] = rotl(state_[3], 11);
        return result;
    }

    f32 Xoshiro128Star::frand()
    {
        return toF32_0(rand());
    }

    f32 Xoshiro128Star::frand2()
    {
        return toF32_1(rand());
    }

    //---------------------------------------------
    //---
    //--- Xoshiro128Plus
    //---
    //---------------------------------------------
    Xoshiro128Plus::Xoshiro128Plus()
        :state_{123456789, 362436069, 521288629, 88675123}
    {}

    Xoshiro128Plus::Xoshiro128Plus(u32 seed)
    {
        srand(seed);
    }
    Xoshiro128Plus::~Xoshiro128Plus()
    {}

    void Xoshiro128Plus::srand(u32 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (1812433253 * (state_[i-1]^(state_[i-1] >> 30)) + i);
        }
    }

    u32 Xoshiro128Plus::rand()
    {
        const u32 result = state_[0] + state_[3];
        const uint32_t t = state_[1] << 9;

        state_[2] ^= state_[0];
        state_[3] ^= state_[1];
        state_[1] ^= state_[2];
        state_[0] ^= state_[3];

        state_[2] ^= t;

        state_[3] = rotl(state_[3], 11);

        return result;
    }

    f32 Xoshiro128Plus::frand()
    {
        return toF32_0(rand());
    }

    f32 Xoshiro128Plus::frand2()
    {
        return toF32_1(rand());
    }

    //---------------------------------------------
    //---
    //--- Xoroshiro128Plus
    //---
    //---------------------------------------------
    Xoroshiro128Plus::Xoroshiro128Plus()
        :state_{123456789123456789, 362436069362436069}
    {}

    Xoroshiro128Plus::Xoroshiro128Plus(u64 seed)
    {
        srand(seed);
    }

    Xoroshiro128Plus::~Xoroshiro128Plus()
    {}

    void Xoroshiro128Plus::srand(u64 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (18124332531812433253ULL * (state_[i-1]^(state_[i-1] >> 60)) + i);
        }
    }

    u64 Xoroshiro128Plus::rand()
    {
        const u64 s0 = state_[0];
        u64 s1 = state_[1];
        const u64 result = s0 + s1;

        s1 ^= s0;
        state_[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
        state_[1] = rotl(s1, 37);

        return result;
    }

    f64 Xoroshiro128Plus::drand2()
    {
        return toF64(rand());
    }

    //---------------------------------------------
    //---
    //--- Xoroshiro256Plus
    //---
    //---------------------------------------------
    Xoroshiro256Plus::Xoroshiro256Plus()
        :state_{123456789123456789, 362436069362436069, 521288629521288629, 8867512388675123}
    {}

    Xoroshiro256Plus::Xoroshiro256Plus(u64 seed)
    {
        srand(seed);
    }

    Xoroshiro256Plus::~Xoroshiro256Plus()
    {}

    void Xoroshiro256Plus::srand(u64 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (18124332531812433253ULL * (state_[i-1]^(state_[i-1] >> 60)) + i);
        }
    }

    u64 Xoroshiro256Plus::rand()
    {
        const u64 result = state_[0] + state_[3];
        const u64 t = state_[1] << 17;

        state_[2] ^= state_[0];
        state_[3] ^= state_[1];
        state_[1] ^= state_[2];
        state_[0] ^= state_[3];

        state_[2] ^= t;

        state_[3] = rotl(state_[3], 45);

        return result;
    }

    f64 Xoroshiro256Plus::drand2()
    {
        return toF64(rand());
    }

    //---------------------------------------------
    //---
    //--- Xoroshiro512Plus
    //---
    //---------------------------------------------
    Xoroshiro512Plus::Xoroshiro512Plus()
        :state_{123456789123456789ULL, 362436069362436069ULL, 521288629521288629ULL, 8867512388675123ULL
                ,123456789123456789ULL, 362436069362436069ULL, 521288629521288629ULL, 8867512388675123ULL}
    {}

    Xoroshiro512Plus::Xoroshiro512Plus(u64 seed)
    {
        srand(seed);
    }

    Xoroshiro512Plus::~Xoroshiro512Plus()
    {}

    void Xoroshiro512Plus::srand(u64 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (18124332531812433253ULL * (state_[i-1]^(state_[i-1] >> 60)) + i);
        }
    }

    u64 Xoroshiro512Plus::rand()
    {
        const u64 result = state_[0] + state_[2];

        const u64 t = state_[1] << 11;

        state_[2] ^= state_[0];
        state_[5] ^= state_[1];
        state_[1] ^= state_[2];
        state_[7] ^= state_[3];
        state_[3] ^= state_[4];
        state_[4] ^= state_[5];
        state_[0] ^= state_[6];
        state_[6] ^= state_[7];

        state_[6] ^= t;

        state_[7] = rotl(state_[7], 21);

        return result;
    }

    f64 Xoroshiro512Plus::drand2()
    {
        return toF64(rand());
    }

    //---------------------------------------------
    //---
    //--- RandWELL
    //---
    //---------------------------------------------
    RandWELL::RandWELL()
        :index_(0)
    {
    }

    RandWELL::RandWELL(u32 seed)
        :index_(0)
    {
        srand(seed);
    }

    RandWELL::~RandWELL()
    {
    }

    void RandWELL::srand(u32 seed)
    {
        state_[0] = seed;
        for(u32 i=1; i<N; ++i){
            state_[i] = (1812433253 * (state_[i-1]^(state_[i-1] >> 30)) + i); 
        }
    }

    u32 RandWELL::rand()
    {
        u32 a, b, c, d;

        a = state_[index_];
        c = state_[(index_+13)&15];
        b = a^c^(a<<16)^(c<<15);
        c = state_[(index_+9)&15];
        c ^= c>>11;
        a = state_[index_] = b^c;
        d = a^((a<<5)&0xDA442D24UL);
        index_ = (index_ + 15) & 15;
        a = state_[index_];
        state_[index_] = a^b^d^(a<<2)^(b<<18)^(c<<28);
        return state_[index_];
    }

    f32 RandWELL::frand()
    {
        return toF32_0(rand());
    }

    f32 RandWELL::frand2()
    {
        return toF32_1(rand());
    }

    //---------------------------------------------
    void cryptRandom(u32 size, void* buffer)
    {
        LASSERT(NULL != buffer);
#if _WIN32
        HMODULE handle = LoadLibrary("Advapi32.dll");
        if(NULL != handle){
            FARPROC procAddress = GetProcAddress(handle, "SystemFunction036");
            BOOLEAN result = (*(BOOLEAN (*)(PVOID, ULONG))procAddress)(buffer, size);
            FreeLibrary(handle);
            if(result){
                return;
            }
        }

        RandWELL random(lcore::getDefaultSeed());
        u32 count = size/sizeof(u32);
        u32* pu32 = static_cast<u32*>(buffer);
        for(u32 i=0; i<count; ++i){
            *pu32 = random.rand();
            ++pu32;
        }
        count = size - count*sizeof(u32);
        u8* pu8 = reinterpret_cast<u8*>(pu32);
        for(u32 i=0; i<count; ++i){
            *pu8 = static_cast<u8>(random.rand());
            ++pu8;
        }
#else
        s32 fd = open("/dev/random", O_RDONLY);
        if(fd<0){
            return;
        }
        read(fd, buffer, size);
        close(fd);
#endif
    }
}
