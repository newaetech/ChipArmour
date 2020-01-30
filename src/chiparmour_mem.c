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

/* See header file for function description (in one place to avoid doxygen problems). */
#include "chiparmour.h"

/***************************************************************************
 Memory space 'secure1' armouring functions.
 ***************************************************************************/

// Up to MAX_SECURE1_RETURN_LOCS return addresses allowed for ca_unlock_secure1
CA_ROP_SET_MAX_RETURNS(ca_unlock_secure1, MAX_SECURE1_RETURN_LOCS);

// Must run static analysis step to mark all call locations
CA_VALID_RETURNADDRS_ARRAY(ca_unlock_secure1);
 
void ca_lock_secure1(void)
{
    ca_hal_lock();
}

void ca_unlock_secure1(uint32_t unlock_key)
{
    unsigned int matchcnt = 0;
    
    ca_landmine();
    
    while(True)
    {
        if (unlock_key == CA_SECURE1_UNLOCK_KEY){
            matchcnt++;            
        } else {
            ca_panic();
        }
        
        if (matchcnt == 3){
            if (unlock_key == CA_SECURE1_UNLOCK_KEY){
                ca_hal_unlock();
                return;
            } else {
                ca_panic();
            }
        }
    }
    ca_panic();
}
