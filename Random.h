#ifndef INC_RANDOM_H_
#define INC_RANDOM_H_
/**
@file Random.h
@author t-sakai
@date 2011/08/20 create
*/
#define RANDOM_STANDALONE
#ifdef RANDOM_STANDALONE
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <ctime>
#include <utility>
#define LASSERT(exp) assert(exp)
#else
#include "lcore.h"
#endif

namespace lcore
{
#ifdef RANDOM_STANDALONE
    typedef char Char;
    typedef unsigned char UChar;
    typedef char16_t Char16;
    typedef wchar_t WChar;
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef intptr_t  intptr_t;
    typedef uintptr_t  uintptr_t;
    typedef ptrdiff_t  ptrdiff_t;
    typedef size_t size_t;
    typedef s64 off_t;

#if defined(ANDROID) || defined(__GNUC__)
    typedef clock_t ClockType;
#else
    typedef u64 ClockType;
#endif

    template<class T>
    inline void swap(T& l, T& r)
    {
        T tmp(std::move(l));
        l = std::move(r);
        r = std::move(tmp);
    }

#endif
    u32 scramble(u32 v, u32 i);
    u64 scramble(u64 v, u64 i);

    u32 getStaticSeed();
    u32 getDefaultSeed();
    u64 getStaticSeed64();
    u64 getDefaultSeed64();

    //---------------------------------------------
    //---
    //--- Xoshiro128Star
    //---
    //---------------------------------------------
    class Xoshiro128Star
    {
    public:
        Xoshiro128Star();
        explicit Xoshiro128Star(u32 seed);
        ~Xoshiro128Star();

        /**
        @brief Initialize with a seed.
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief Generate a unsigned number in [0 0xFFFFFFFFU]
        */
        u32 rand();

        /**
        @brief Generate a float in (0, 1]
        */
        f32 frand();

        /**
        @brief Generate a float in [0, 1)
        */
        f32 frand2();
    private:
        static const u32 N = 4;
        u32 state_[N];
    };

    //---------------------------------------------
    //---
    //--- Xoshiro128+
    //---
    //---------------------------------------------
    class Xoshiro128Plus
    {
    public:
        Xoshiro128Plus();
        explicit Xoshiro128Plus(u32 seed);
        ~Xoshiro128Plus();

        /**
        @brief Initialize with a seed.
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief Generate a unsigned number in [0 0xFFFFFFFFU]
        */
        u32 rand();

        /**
        @brief Generate a float in (0, 1]
        */
        f32 frand();

        /**
        @brief Generate a float in [0, 1)
        */
        f32 frand2();
    private:
        static const u32 N = 4;
        u32 state_[N];
    };

    //---------------------------------------------
    //---
    //--- Xoroshiro128+
    //---
    //---------------------------------------------
    class Xoroshiro128Plus
    {
    public:
        Xoroshiro128Plus();
        explicit Xoroshiro128Plus(u64 seed);
        ~Xoroshiro128Plus();

        /**
        @brief Initialize with a seed.
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief Generate a unsigned number in [0 0xFFFF FFFF FFFF FFFFU]
        */
        u64 rand();

        /**
        @brief Generate a double in [0, 1)
        */
        f64 drand2();
    private:
        static const u32 N = 2;
        u64 state_[N];
    };

    //---------------------------------------------
    //---
    //--- Xoroshiro256+
    //---
    //---------------------------------------------
    class Xoroshiro256Plus
    {
    public:
        Xoroshiro256Plus();
        explicit Xoroshiro256Plus(u64 seed);
        ~Xoroshiro256Plus();

        /**
        @brief Initialize with a seed.
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief Generate a unsigned number in [0 0xFFFF FFFF FFFF FFFFU]
        */
        u64 rand();

        /**
        @brief Generate a double in [0, 1)
        */
        f64 drand2();
    private:
        static const u32 N = 4;
        u64 state_[N];
    };

    //---------------------------------------------
    //---
    //--- Xoroshiro512+
    //---
    //---------------------------------------------
    class Xoroshiro512Plus
    {
    public:
        Xoroshiro512Plus();
        explicit Xoroshiro512Plus(u64 seed);
        ~Xoroshiro512Plus();

        /**
        @brief Initialize with a seed.
        @param seed
        */
        void srand(u64 seed);

        /**
        @brief Generate a unsigned number in [0 0xFFFF FFFF FFFF FFFFU]
        */
        u64 rand();

        /**
        @brief Generate a double in [0, 1)
        */
        f64 drand2();
    private:
        static const u32 N = 8;
        u64 state_[N];
    };

    //---------------------------------------------
    //---
    //--- RandWELL
    //---
    //---------------------------------------------
    class RandWELL
    {
    public:
        RandWELL();
        explicit RandWELL(u32 seed);
        ~RandWELL();

        /**
        @brief Initialize with a seed.
        @param seed
        */
        void srand(u32 seed);

        /**
        @brief Generate a unsigned number in [0 0xFFFFFFFFU]
        */
        u32 rand();

        /**
        @brief Generate a float in (0, 1]
        */
        f32 frand();

        /**
        @brief Generate a float in [0, 1)
        */
        f32 frand2();

    private:
        static const u32 N = 16;

        u32 state_[N];
        u32 index_;
    };

    //----------------------------------------------------
    /**
    @brief [vmin, vmax)
    */
    template<class T, class U>
    U range_ropen(T& random, U vmin, U vmax)
    {
        LASSERT(vmin<=vmax);
        return static_cast<U>(random.rand() % (vmax-vmin)) + vmin;
    }

    /**
    @brief [vmin, vmax)
    */
    template<class T>
    f32 range_ropen(T& random, f32 vmin, f32 vmax)
    {
        LASSERT(vmin<=vmax);
        return (vmax-vmin)*random.frand2() + vmin;
    }

    /**
    @brief [vmin, vmax]
    */
    template<class T, class U>
    U range_rclose(T& random, U vmin, U vmax)
    {
        LASSERT(vmin<=vmax);
        return static_cast<U>(random.rand() % (vmax-vmin+1)) + vmin;
    }

    /**
    @brief [vmin, vmax]
    */
    template<class T>
    f32 range_rclose(T& random, f32 vmin, f32 vmax)
    {
        LASSERT(vmin<=vmax);
        return (vmax-vmin)*random.frand() + vmin;
    }


    /**
    @brief [0, v)
    */
    template<class T, class U>
    U range_ropen(T& random, U v)
    {
        LASSERT(0<=v);
        return static_cast<U>(random.rand() % v);
    }

    template<class T, class U>
    void shuffle(T& random, U* start, U* end)
    {
        U* s = start-1;
        for(U* i=end-1; start<i; --i){
            u32 offset = range_ropen(random, (u32)(i-s));
            lcore::swap(*i, *(start+offset));
        }
	}

    template<class T, class U>
    void shuffle(T& random, s32 num, U* v)
    {
        s32 n = num+1;
        for(s32 i=num-1; 0<i; --i){
            u32 offset = range_ropen(random, (u32)(n-i));
            lcore::swap(v[i], v[offset]);
        }
	}

    void cryptRandom(u32 size, void* buffer);
}

#endif //INC_RANDOM_H_
