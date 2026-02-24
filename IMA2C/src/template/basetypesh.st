/* C mapping of AADL package base_types */ 
#ifndef __BASE_TYPES_H__ 
#define __BASE_TYPES_H__ 

#include<stdint.h> 
#include<stdbool.h> 

typedef bool     base_types_boolean; 

typedef int8_t   base_types_int8; 
typedef int16_t  base_types_int16; 
typedef int32_t  base_types_int32; 
typedef int64_t  base_types_int64; 

typedef uint8_t  base_types_uint8; 
typedef uint16_t base_types_uint16; 
typedef uint32_t base_types_uint32; 
typedef uint64_t base_types_uint64; 

typedef float    base_types_float32; /* As per IEEE 754 */ 
typedef double   base_types_float64; 
typedef char     base_types_character; 

#endif  /* __BASE_TYPES_H__ */ 