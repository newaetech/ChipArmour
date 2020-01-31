/*
This file is part of ChipArmour™, by NewAE Technology Inc.

ChipArmour™ is Copyright 2019 NewAE Technology Inc.

ChipArmour™ is a trademark of NewAE Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at:

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/
#include <stdint.h>

#ifndef CHIPARMOUR_H
#define CHIPARMOUR_H

/***************************************************************************
 Typedefs
 ***************************************************************************/

/**
    Pointer to a function with prototype:
       void function_to_call(void *);
*/
typedef void (*ca_fptr_voidptr_t)(void * func_argument);

/**
    Pointer to a function with prototype:
       void function_to_call(void * func_argument, uint8_t * value_array);
*/
typedef void (*ca_fptr_voidptr_array_t)(void * func_argument, uint8_t * value_array);

/**
    Pointer to a hash (or similar) function with prototype:
       void function_to_call(void * image, uint8_t * hash_result, uint32_t hash_result_len);
       
    Function is designed to be passed:
    
       void * image  : Image of file to hash. Needs to be internally cast to 
                       correct type.
       
       uint8_t * hash: Calculation of hash result stored here.
       
       uint32_t  len : Maximum length of hash buffer.
       
       return value  : Actual length of hash written to buffer, or -1 for fail.
*/
typedef int32_t (*ca_fptr_gethash_t)(void * image, uint8_t * hash, uint32_t len);


/**
    uint32_t returned by ca_ret_N functions, must be passed to comparison
     functions.
*/
typedef struct {
    uint32_t value;
    uint32_t invvalue;
} ca_uint32_t;

/**
    uint16_t returned by ca_ret_N functions, must be passed to comparison
     functions.
*/
typedef struct {
    uint16_t value;
    uint16_t invvalue;
} ca_uint16_t;

/**
    uint8_t returned by ca_ret_N functions, must be passed to comparison
     functions.
*/
typedef struct {
    uint8_t value;
    uint8_t invvalue;
} ca_uint8_t;

typedef struct {
    ca_fptr_voidptr_t value;
    ca_fptr_voidptr_t invvalue;
} ca_funcpointer_t;

/**
    Complicated return values.
*/
enum ca_return_t { 
    CA_SUCCESS = 0x5ABF0938,
    CA_FAIL    = 0x2820F02A,
    CA_BADARG  = 0x328A9201,
    CA_MEMERR  = 0x480ABFE1,
};
typedef enum ca_return_t ca_return_t;
//Sidenote: To work as enum, restricted to positive values only (make sure top
//          bit is clear).

/***************************************************************************
 User call-back functions you may define (overrides weak defaults).
 ***************************************************************************/
 
/** 
    Called when bad things are happening, such as an active FI attack.
*/
void causer_panic(void);

/***************************************************************************
 HAL functions required to be defined on your platform.
 ***************************************************************************/

void ca_hal_mpu_init(void);

/***************************************************************************
 ROP prevention assistance functions.
 ***************************************************************************/

#define CA_ROP_SET_MAX_RETURNS(functionname, maxreturns) \
    static uint32_t ca_functionname_max_returns = maxreturns;

#define CA_ROP_RETURNADDRS_ARRAY(functionname) \
    static void * ca_functionname_valid_returnaddrs[ca_functionname_max_returns];

#define CA_ROP_CHECK_VALID_RETURN(functionname) \
do { \
    /* Validate we are returning to a valid call location */ \
    void * ca_ra = __builtin_extract_return_addr(__builtin_return_address(0)); \
    uint32_t ca_loopindx; \
    for(ca_loopindx = 0; ca_loopindx < len(ca_functionname_valid_returnaddrs); ca_loopindx++){ \
        /* The zero flag indicates end of array reached, shouldn't happen */ \
        if (ca_functionname_valid_returnaddrs[ca_loopindx] == 0){ \
            ca_panic(); \
        } \
        if (ca_functionname_valid_returnaddrs[ca_loopindx] == ca_ra){ \
            break; \
        } \
    } \
    if (ca_loopindx > len(ca_functionname_valid_returnaddrs)) { \
        ca_panic(); \
    } \
 } while(0)

/***************************************************************************
 Memory space armouring macros / function.
 ***************************************************************************/

/**
    Move variable to an armoured memory space ('ca_secure1').
*/
#define CA_ATTR_SECURE1 __attribute__((section("ca_secure1")))

/**
    Lock (prevent all access) to memory space secure1.
*/
void ca_lock_secure1(void);

/**
    Unlock (allow all access) to memory space secure1.
*/
void ca_unlock_secure1(uint32_t unlock_key);

#ifndef MAX_SECURE1_RETURN_LOCS
#define MAX_SECURE1_RETURN_LOCS 10
#endif



/***************************************************************************
 System functions/macros
 ***************************************************************************/

/**
    Setup MPU to armour memory spaces, init RNG if possible, etc.
*/
void ca_init(void);


/***************************************************************************
 Testing functions (may want to disable in production build).
 ***************************************************************************/

/**
    Walk from non-secure memory space to secure, should cause a memory exception
    if this is called once the MPU is enabled.
    
    As using the wrong linker script or misconfiguring the MPU can easily cause
    the memory armouring to fail, it's important to use this function as part
    of your build testing.
*/
void ca_test_mpu(void);

/**
    Attempt a call to ca_unlock_secure function from unauthorized location.
*/

/**
    Call the panic function / response.
*/
void ca_test_panic(void);

/***************************************************************************
State machine validation
****************************************************************************/

#define CA_STATE_INIT -2944

/**
    Once called with CA_STATE_INIT, this function requires a simple step
    sequence to be presented, or it calls the panic function. Helpful to
    detect out-of-sequence calls due to attacker calling directly a function.
*/
void ca_state_machine(int statenum);


/***************************************************************************
 Data processing functions/macros
 ***************************************************************************/

ca_return_t _ca_compare_u32_eq(ca_uint32_t op1,
                  ca_uint32_t op2,
                  ca_fptr_voidptr_t equal_function,
                  void * equal_func_param,
                  ca_fptr_voidptr_t unequal_function,
                  void * unequal_func_param);
                  
uint32_t _ca_limit_u32(ca_uint32_t input, ca_uint32_t min, ca_uint32_t max);

/**
    Returns a 32-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint32_t ca_ret_u32(uint32_t value);

/**
    Returns a 32-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint32_t ca_retfast_u32(uint32_t value);

/**
    Returns a 16-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint16_t ca_ret_u16(uint16_t value);

/**
    Returns a 8-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint8_t ca_ret_u8(uint8_t value);

/**
    Take an input value and ensure it falls within the given limits, by 
    returning the limited value.
*/
inline uint32_t ca_limit_u32(uint32_t input, uint32_t min, uint32_t max)
{
    return _ca_limit_u32(ca_retfast_u32(input), ca_retfast_u32(min), ca_retfast_u32(max));
}

/**
    Compare two uint32_t numbers, and call a function if they are the same or
    another function if they are different.
    
    op1: Operand 1
    op2: Operand 2
    equal_function: Function of type 'void func(void *)' that will be called
                    with argument equal_func_param if op1 == op2. Null if you
                    don't need a function called on match.
    unequal_function: Function of type 'void func(void *)' that will be called
                    with argument unequal_func_param if op1 != op2. Null if you
                    don't need a function called on differ.
*/

inline ca_return_t ca_compare_u32_eq( uint32_t op1, 
                               uint32_t op2,
                               ca_fptr_voidptr_t equal_function,
                               void * equal_func_param,
                               ca_fptr_voidptr_t unequal_function,
                               void * unequal_func_param)
 {
    return _ca_compare_u32_eq(ca_retfast_u32(op1),
                       ca_retfast_u32(op2),
                       equal_function,
                       equal_func_param,
                       unequal_function,
                       unequal_func_param);
 }

/**************************************************************************
 Signature verification functions / macros
 **************************************************************************/

/**
   Signature verification: compares the result of a function call with some
   magic value, and calls one of two functions in response.

*/
ca_return_t ca_compare_func_eq( ca_fptr_voidptr_array_t    get_value_func,
                             void *                     get_value_func_param,
                             uint8_t *                  expected_value_array,
                             uint32_t                   expected_value_len,
                             ca_fptr_voidptr_t           equal_function,
                             void *                     equal_func_param,
                             ca_fptr_voidptr_t          unequal_function,
                             void *                     unequal_func_param);

#endif