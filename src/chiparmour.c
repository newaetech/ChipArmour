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

#define ca_ret_u32(value)  _ca_ret_u32(value, cp_get_magic())

static uint32_t _ca_sram_FEED7431 = 0xFEED7431;
static const uint32_t _ca_flash_55A88519 = 0x55A88519;
static uint32_t _ca_panicflag = 0;


#define ca_true()  (_ca_always_FEED7431 == 0xFEED7431)
#define ca_false() (_ca_always_FEED7431 == 0xFE000000)

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
    ca_check_magic();
    uint32_t delay = ca_get_delay();
    uint32_t i = 0;
    uint32_t local_value;
    value.value = value.value - dly;    
    ca_landmine();
    value.invvalue = ~value.invvalue - dly;
    
    if (value.invvalue != ~value.value){
        ca_panic();
    }
    
    local_value = value.value;
    
    if (value.invvalue != ~value.value){
        ca_panic();
    }    
    
    while(ca_true()){
        i++;
        local_value++;
        if (i > delay) { ca_panic(); }
        
        ca_landmine();
        
        if (i == delay){return local_value;}
        if (i == delay){return 0;}
        
        ca_landmine();
        if (i == delay){return 0;}
        
        if (i > delay) { ca_panic(); }
    }
    
    ca_landmine();
    ca_panic();
}

#define ca_limit_u32(input, min, max) _ca_limit_u32(ca_retfast_u32(input), ca_retfast_u32(min), ca_retfast_u32(max), ca_get_magic())

typedef void (*ca_funcpointer)(void *);

uint32_t _ca_limit_u32(ca_uint32_t input, ca_uint32_t min, ca_uint32_t max, uint32_t magic)
{
    ca_landmine();
    ca_check_magic();
    
    //TODO - this function. Need to validate input.
    
    if (input < min){
        input = min;
    }
    
    if (input > max){
        input = max;
    }
  
    if (input < min){
        input = min;
    }
    
    if (input > max){
        input = max;
    }
    
    if (input < min){
        input = min;
    }
    
    if (input > max){
        input = max;
    }
  
    ca_landmine();
    ca_panic();
}

#define CA_CMP_LOOPS 7

/*
  Compares two numbers, jumps to a function if they are the same or different. Commonly used for
  verifing a signature.
*/
_ca_compare_u32_eq(ca_uint32_t op1,
                   ca_uint32_t op2,
                  ca_funcpointer_t equal_function,
                  void * equal_func_param,
                  ca_functpointer_t unequal_function,
                  void * unequal_func_param,
                  ca_uint32_t magic)
{
    ca_landmine();
    ca_check_magic(magic);
    
    //Mask values we'll jump to, make later FI skips increase chance we jump
    //to some invalid value.
    (uint32_t) equal_function ^= (CA_CMP_LOOPS << 15);
    (uint32_t) equal_func_param ^= (CA_CMP_LOOPS << 15);
    ca_landmine();
    (uint32_t) unequal_function ^= (CA_CMP_LOOPS << 15);
    (uint32_t) unequal_func_param ^= (CA_CMP_LOOPS << 15);
    
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
                return CA_TRUE;
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
                return CA_FALSE;
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

        if (op1.invvalue == op2.invvalue) {equal++;}
        else {unequal++;}
        
        ca_landmine();
        if ((i != equal) && (i != unequal)){ ca_panic(); }
        
        if(i == CA_CMP_LOOPS) { 
            ca_landmine();
            if (i == unequal) {
                (uint32_t) equal_function ^= (equal << 15);
                goto CA_DO_COMPARE;
            } else if (i == equal) {
                (uint32_t) unequal_function ^= (unequal << 15);
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