#include "Random.h"
#include <fstream>


template<class T>
void output32(lcore::u32 seed, const lcore::Char* filename)
{
    std::ofstream file(filename, std::ios::binary);
    if(!file.is_open()){
        return;
    }
    static const lcore::u32 Size = 2UL * 1024 * 1024 * 1024;
    T random(seed);
    for(lcore::u32 i=0; i<Size/sizeof(lcore::u32); ++i){
        lcore::u32 x = random.rand();
        file.write(reinterpret_cast<const char*>(&x), sizeof(lcore::u32));
    }
    file.close();
}

int main(int, char**)
{
    lcore::u32 seed = 0x1234U;
    output32<lcore::Xoshiro128Plus>(seed, "Xoshiro128Plus.byte");
    output32<lcore::RandWELL>(seed, "WELL64.byte");
    return 0;
}
