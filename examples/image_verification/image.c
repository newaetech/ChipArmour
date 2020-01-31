/****************************************************************************************************
 * ChipArmour Demo - Bootloader Image Validation
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "hal.h"
#include "../../inc/chiparmour.h"

int snprintf(char *, size_t, char *, ...);

/* Avoid stdio.h as not sure what platform provides */
int puts(const char * s)
{
    while(*s){
        putch(*s++);
    }
    
    return 0;
}


/******** DATA STORAGE - Following would be in FLASH normally for image ********/
/* Simulate image data that would be held in FLASH memory */
typedef struct {
    char      image_name[64];
    uint8_t   image_data[1024];
    uint32_t  image_data_len;
    uint32_t  signature;
} image_t;

image_t image = {
    "CA Demo Image", /* Name of image */
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}, /* Image data (fake) */
    256,   /* Length of image in bytes */
    0x4C6509CC /* Image signature */
};

/********* DATA STORAGE - Following would be in FLASH/EFUSE Normally **********/

//This demo doesn't use real crypto, this would be the real public key used for validating
//signature normally.
uint8_t manf_public_key[] = {92,101,9,204,92,5};


#define FLAG_PENDING_UPDATE 0xFEEDB347
#define FLAG_BOOT_AS_NORMAL 0

/* Variable holds if we should attempt to enter bootload mode. This would normally be
   in flash as part of a bootloader state, we keep it in SRAM here for this demo. */
static int bootloader_flag = FLAG_PENDING_UPDATE;

/**
 This would be a SHA256 hash in real bootloaders, here a dumb 32-bit thingy is
 done instead to simplify the example. (NOTE: THIS FUNCTION IS NOT A HASH! IT'S FOR DEMO!)
 */
uint32_t some_hash_function(uint8_t * image, uint32_t data_len)
{
    uint32_t temp;
    uint32_t hash = 0;
    uint32_t i = 0;
    
    while(data_len){
        
        temp = (hash >> 24);
        hash = hash << 8;
        hash ^= image[i++];
        hash ^= temp;
        
        data_len--;
    }

    
    return hash;
}

/**
 This would be a pub-key enc/dec function such as RSA. Here a dumb 32-bit thingy is
 done instead to simplify the example. (NOTE: THIS FUNCTION IS NOT PUBLIC KEY CRYPTO! IT'S FOR DEMO!)
*/
uint32_t some_crypto_function(uint32_t user_signature, uint8_t manf_public_key[])
{
    
    uint32_t temp;
    uint32_t hash = 0;    
    
    for(uint32_t i = 0; i < 4; i++){
        temp = (hash >> 24);
        hash = hash << 8;
        hash ^= manf_public_key[i];
        hash ^= *((uint8_t*)(&user_signature) + i);
        hash ^= temp;
    }
    
    char buf[256];
    snprintf(buf, 255, "Hash: %#lX\n", hash);
    puts(buf);
    return hash;
}

/** RTOS init function */
void rtos_init(void)
{
    puts("RTOS Booted!\n");
}

/** Execute tasks in regular mode */
void rtos_loop(void)
{
    return;
}

int checkfwupdate_original(void);
int checkfwupdate_armoured(void);

int main(void)
{
    //Check if fw update pending, apply if so
    platform_init();
    init_uart();
    checkfwupdate_original();
    
    //Check if fw update pending, apply if so
    //checkfwupdate_armoured();
    
    //No firmware update - start regular operations
    rtos_init();
    
    while(1){
        rtos_loop();
    }
}

/*****************************************************************************
 * 
 * Simple bootloader functions - highly vulnerable to Fault Injection.
 *
 *****************************************************************************/

/**
 Firmware update function, called once we know all images are OK.
 */
 void boot_new_image(image_t * image)
 {
     puts("Booting image ");
     puts(image->image_name);
     puts("\n");
     
     //Here would go code to actually do the real stuff
 }


/**
 Original validation function which is vulnerable to FI.
 */
int validate_sigature(uint32_t short_hash, uint32_t signature, uint8_t * pub_key)
{
    if (some_crypto_function(short_hash, pub_key) == signature){
        return 1;
    } else {
        return 0;
    }
}

/**
 Original function which is vulnerable to FI.
 */
int checkfwupdate_original(void)
{
    //Flag indicates new firmware file present
    if(bootloader_flag == FLAG_PENDING_UPDATE){
        
        //Check signature matches proposed hashes
        if (validate_sigature(some_hash_function(image.image_data, image.image_data_len),
                              image.signature, manf_public_key)) {
            
            boot_new_image(&image);
        } else {
            //signature failed
            bootloader_flag = 0;
        }
    }
    
    return 0;
}

/*****************************************************************************
 * 
 * Simple bootloader functions that are more resistant to fault injection.
 *
 *****************************************************************************/

void fw_update_stage1(void * image);
void fw_update_stage1_failed(void * image);
void fw_update_stage2_failed(void * image);

//CA_ROP_SET_MAX_RETURNS(fw_update_stage1, 1);
//CA_ROP_SET_MAX_RETURNS(boot_new_image, 1);

//CA_ROP_RETURNADDRS_ARRAY(fw_update_stage1);
//CA_ROP_RETURNADDRS_ARRAY(boot_new_image);

/**
 Firmware update function, called once we know all images are OK.
 */
 void boot_new_image_armoured(image_t * image)
 {
     //CA_ROP_CHECK_VALID_RETURN(boot_new_image);
     
     ca_state_machine(2);
     
     //ca_unmangle_var(image, 1);
     
     puts("Booting image ");
     puts(image->image_name);
     puts("\n");
     
     //Here would go code to actually do the real stuff
 }


/**
 We need a wrapper for our enc function which runs on the calculated hash, that
 will be compared with the known signature. 
*/
void wrapper_some_crypto_function(void * input, uint8_t * output)
{
    uint32_t expected;
    expected = some_crypto_function(*((uint32_t *)input), manf_public_key);
    
    *((uint32_t *)output) = expected;
}

/**
 Entry point to bootloader check
 */
int checkfwupdate_armoured(void)
{
    ca_state_machine(CA_STATE_INIT);
    
    //Flag indicates new firmware file present
    ca_compare_u32_eq(bootloader_flag,
                      FLAG_PENDING_UPDATE,
                      fw_update_stage1,
                      (void *)&image,
                      fw_update_stage1_failed,
                      (void *)&image);
    
    ca_state_machine(3);
    
    return 0;
}

/**
 Step 1: Magic flag set, check signature ok by public key.
 */
void fw_update_stage1(void * image)
{
    //CA_ROP_CHECK_VALID_RETURN(fw_update_stage1);
    
    ca_state_machine(1);
    
    uint32_t hash = some_hash_function(((image_t *)image)->image_data, ((image_t *)image)->image_data_len);
    
    //Possible new image - first we calculate hash of image data, then check signature
    ca_compare_func_eq(wrapper_some_crypto_function,
                       (void *)&hash,
                       (uint8_t *) (((image_t *)image)->signature),
                       4,
                       (ca_fptr_voidptr_t)boot_new_image_armoured,
                       (void *)image,
                       fw_update_stage2_failed,
                       (void *)image);
    
}

void fw_update_stage1_failed(void * image)
{
    //Prevent out of order function calls
    ca_state_machine(1);
    
    //Flag not set - boot as normal
    bootloader_flag = 0;
}

/**
 Step 2: If successful, we directly are booting new image.
 */

void fw_update_stage2_failed(void * image)
{
    ca_state_machine(2);

    //Flag not set - boot as normal
    bootloader_flag = 0;
    
    
}