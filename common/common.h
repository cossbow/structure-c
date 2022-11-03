//
// Created by j30036461 on 2022/11/1.
//

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>

typedef union {
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    float f32;
    int64_t i64;
    uint64_t u64;
    double f64;
    void *ptr;
} MixType;

typedef struct {
    bool exists;
    MixType data;
} Optional;


typedef int (*MixTypeComparator)(MixType, MixType);

typedef void (*MixTypeFreer)(MixType);



#endif //COMMON_H
