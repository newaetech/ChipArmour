/*
This file is part of ChipArmour™, by NewAE Technology Inc.

ChipArmour™ is Copyright 2019-2020 NewAE Technology Inc.

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
#include "../inc/chiparmour.h"
#ifdef __GNUC__
#pragma GCC optimize("O0")
#else
#error "Only GCC supported"
#endif //__GNUC__

#define ca_ret_u32(value)  _ca_ret_u32(value, cp_get_magic())

static uint32_t _ca_sram_FEED7431 = 0xFEED7431;
static const uint32_t _ca_flash_55A88519 = 0x55A88519;
static uint32_t _ca_panicflag = 0;


#define ca_true()  (_ca_sram_FEED7431 == 0xFEED7431)
#define ca_false() (_ca_sram_FEED7431 == 0xFE000000)

#define ca_panic() {_ca_panicflag++; _ca_panic();}

/** 
  Jumps to the panic function if one of two comparisons fail.
  */

#define ca_landmine() { if(_ca_sram_FEED7431 != 0xFEED7431){ca_panic();} \
                        if(_ca_flash_55A88519 != 0x55A88519){ca_panic();} \
                        if(_ca_sram_FEED7431 == _ca_flash_55A88519){ca_panic();} }

/**
  Returns an unsigned 32-bit value, but adds armour around the return
  function to catch fault injection attempts. 
*/
ca_uint32_t _ca_ret_u32(ca_uint32_t value, ca_uint32_t magic, uint32_t maxdelay)
{
    ca_landmine();
    uint32_t delay = ca_get_delay();
    uint32_t i = 0;
    uint32_t local_value;    
    ca_landmine();
    
    if (value.invvalue != ~value.value){
        ca_panic();
    }
    
    local_value = value.value - delay;
    
    if (value.invvalue != ~value.value){
        ca_panic();
    }
    
    ca_uint32_t invalid_rv;
    invalid_rv.value = 0;
    invalid_rv.invvalue = 0;
    
    while(ca_true()){
        i++;
        local_value++;
        if (i > delay) { ca_panic(); }
        
        ca_landmine();
        
        if (i == delay){
            ca_uint32_t rv;
            rv.value = local_value;
            rv.invvalue = ~local_value;
            return rv;
        }
        if (i == delay){return invalid_rv;}
        
        ca_landmine();
        if (i == delay){return invalid_rv;}
        
        if (i > delay) { ca_panic(); }
    }
    
    ca_landmine();
    ca_panic();
    
    return invalid_rv;
}

typedef void (*ca_funcpointer)(void *);

uint32_t _ca_limit_u32(ca_uint32_t input, ca_uint32_t min, ca_uint32_t max)
{
    ca_landmine();
    
    //Quick version - just have multiple checks
    
    if (input.value < min.value){
        input.value = min.value;
        input.invvalue = min.invvalue;
    }
    
    if (input.value > max.value){
        input.value = max.value;
        input.invvalue = max.invvalue;
    }
  
    if (input.invvalue != ~input.value){
        ca_panic();
    }
    if (input.value < min.value){
        input.value = min.value;
        input.invvalue = min.invvalue;
    }
    
    if (input.value > max.value){
        input.value = max.value;
        input.invvalue = max.invvalue;
    }
    
    if (input.invvalue != ~input.value){
        ca_panic();
    }
    if (input.value < min.value){
        input.value = min.value;
        input.invvalue = min.invvalue;
    }
    
    if (input.value > max.value){
        input.value = max.value;
        input.invvalue = max.invvalue;
    }
  
    if (input.invvalue != ~input.value){
        ca_panic();
    }
    ca_landmine();
    ca_panic();
    return input.value;
}

#define CA_CMP_LOOPS 3

/*
  Compares two numbers, jumps to a function if they are the same or different. Commonly used for
  verifing a signature.
*/
ca_return_t _ca_compare_u32_eq(ca_uint32_t op1,
                   ca_uint32_t op2,
                  ca_fptr_voidptr_t  equal_function,
                  void * equal_func_param,
                  ca_fptr_voidptr_t  unequal_function,
                  void * unequal_func_param)
{
    ca_landmine();
    
    //Mask values we'll jump to, make later FI skips increase chance we jump
    //to some invalid value.
    equal_function = (CA_CMP_LOOPS << 15)^(uint32_t)equal_function;
    equal_func_param = (CA_CMP_LOOPS << 15)^(uint32_t)equal_func_param;
    ca_landmine();
    unequal_function = (CA_CMP_LOOPS << 15)^(uint32_t)unequal_function;
    unequal_func_param = (CA_CMP_LOOPS << 15)^(uint32_t)unequal_func_param;
    
    uint32_t equal = 0;
    uint32_t unequal = 0;
    
    int i = -1000;
    ca_landmine();
    
    goto CA_DO_LOOP;
    
    ca_panic();
    ca_landmine();
    ca_panic();
    ca_landmine();
    ca_panic();
    
CA_DO_COMPARE:
    if (i == CA_CMP_LOOPS) {
        
        ca_atmine();
        ca_atwait();
        
        if (equal == CA_CMP_LOOPS){
            ca_atmine();
            ca_atwait();
            if (equal == CA_CMP_LOOPS){
                if(equal_function) {
                    equal_function(equal_func_param);
                }
                return CA_SUCCESS;
            } else {
                ca_fullpanic();
            }
        }
        
        if (unequal == CA_CMP_LOOPS){
            ca_atmine();
            ca_atwait();            
            if (unequal == CA_CMP_LOOPS){
                if(unequal_function){
                    unequal_function(unequal_func_param);
                }                
                return CA_FAIL;
            } else {
                ca_fullpanic();
            }
        }

        ca_fullpanic();        
        
    } else {
        ca_fullpanic();
    }
    
    ca_panic();

    return -1;
    
CA_DO_LOOP:
    i = 0;
    while(1)
    {
        if (op1.value == op2.value) {equal++;}
        else {unequal++;}
        
        ca_fastwait();        
        i++;

        /*if (op1.invvalue == op2.invvalue) {equal++;}
        else {unequal++;}
        i++;*/
        
        ca_landmine();
        if ((i != equal) && (i != unequal)){ ca_panic(); }
        
        if(i == CA_CMP_LOOPS) { 
            ca_landmine();
            if (i == equal) {
                equal_function = (equal << 15) ^ (uint32_t)equal_function;
                equal_func_param = (equal << 15) ^ (uint32_t)equal_func_param;
                goto CA_DO_COMPARE;
            } else if (i == unequal) {
                unequal_function = (unequal << 15) ^ (uint32_t)unequal_function;
                unequal_func_param = (equal << 15) ^ (uint32_t)unequal_func_param;
                goto CA_DO_COMPARE;
            } else {
                ca_panic();
            }
        }
        
        if (i > CA_CMP_LOOPS){ca_panic();}
        
        ca_landmine();
    }
    
    ca_panic();
    ca_panic();
    ca_panic();
}

//UNFINISHED
ca_return_t ca_compare_func_eq( ca_fptr_voidptr_array_t    get_value_func,
                             void *                     get_value_func_param,
                             uint8_t *                  get_value_func_return,
                             uint8_t *                  expected_value_array,
                             uint32_t                   expected_value_len,
                             ca_fptr_voidptr_t           equal_function,
                             void *                     equal_func_param,
                             ca_fptr_voidptr_t          unequal_function,
                             void *                     unequal_func_param)
{
    ca_landmine();
    
    get_value_func(get_value_func_param, get_value_func_return);
    //Mask values we'll jump to, make later FI skips increase chance we jump
    //to some invalid value.
    equal_function = (CA_CMP_LOOPS << 15)^(uint32_t)equal_function;
    equal_func_param = (CA_CMP_LOOPS << 15)^(uint32_t)equal_func_param;
    ca_landmine();
    unequal_function = (CA_CMP_LOOPS << 15)^(uint32_t)unequal_function;
    unequal_func_param = (CA_CMP_LOOPS << 15)^(uint32_t)unequal_func_param;
    
    uint32_t equal = 0;
    uint32_t unequal = 0;
    
    int i = -1000;
    ca_landmine();
    
    goto CA_DO_LOOP;
    
    ca_panic();
    ca_landmine();
    ca_panic();
    ca_landmine();
    ca_panic();
    
CA_DO_COMPARE:
    if (i == CA_CMP_LOOPS) {
        
        ca_atmine();
        ca_atwait();
        
        if (equal == CA_CMP_LOOPS){
            ca_atmine();
            ca_atwait();
            if (equal == CA_CMP_LOOPS){
                if(equal_function) {
                    equal_function(equal_func_param);
                }
                return CA_SUCCESS;
            } else {
                ca_fullpanic();
            }
        }
        
        if (unequal == CA_CMP_LOOPS){
            ca_atmine();
            ca_atwait();            
            if (unequal == CA_CMP_LOOPS){
                if(unequal_function){
                    unequal_function(unequal_func_param);
                }                
                return CA_FAIL;
            } else {
                ca_fullpanic();
            }
        }

        ca_fullpanic();        
        
    } else {
        ca_fullpanic();
    }
    
    ca_panic();

    return -1;
    
CA_DO_LOOP:
    i = 0;
    while(1)
    {
        int j = 0;
        int op_unequal = 0;
        for (j = 0; j < expected_value_len; j++) {
            volatile int a = expected_value_array[j];
            if (get_value_func_return[j] != expected_value_array[j]) {
                op_unequal = 1;
                break;
            }
                
        }
        if (!op_unequal) {equal++;}
        else {unequal++;}
        
        ca_fastwait();        
        i++;
        
        ca_landmine();
        if ((i != equal) && (i != unequal)){ ca_panic(); }
        
        if(i == CA_CMP_LOOPS) { 
            ca_landmine();
            if (i == equal) {
                equal_function = (equal << 15) ^ (uint32_t)equal_function;
                equal_func_param = (equal << 15) ^ (uint32_t)equal_func_param;
                goto CA_DO_COMPARE;
            } else if (i == unequal) {
                unequal_function = (unequal << 15) ^ (uint32_t)unequal_function;
                unequal_func_param = (equal << 15) ^ (uint32_t)unequal_func_param;
                goto CA_DO_COMPARE;
            } else {
                ca_panic();
            }
        }
        
        if (i > CA_CMP_LOOPS){ca_panic();}
        
        ca_landmine();
    }
    
    ca_panic();
    ca_panic();
    ca_panic();
}
void ca_state_machine(int statenum)
{
    static int ca_stored_state;
    
    if (statenum == CA_STATE_INIT) {
        ca_stored_state = 0;
        return;
    }
    
    if (++ca_stored_state != statenum){
        ca_panic();
    }
    
    if (ca_stored_state != statenum){
        ca_panic();
    }
    
    return;
}

int ca_atmine(void)
{}

int ca_atwait(void)
{}

int ca_fastwait(void)
{}

ca_uint32_t ca_retfast_u32(uint32_t value)
{
    ca_uint32_t ret = {value, ~value};
    return ret;
}

int ca_fullpanic(void)
{puts("FULL PANIC!"); while(1);}