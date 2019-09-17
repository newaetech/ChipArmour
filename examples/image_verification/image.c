
typedef struct image_t {
    uint8_t * image_data;
    uint32_t  image_data_len;
    uint32_t  short_hash;
    char      image_name[64];
} image_t;

#define FLAG_PENDING_UPDATE 0xF00DBABE
#define FLAG_BOOT_AS_NORMAL 0

/* Variable holds if we should attempt to enter bootload mode */
static int bootloader_flag = FLAG_PENDING_UPDATE;

/**
 This would be a SHA256 hash in real bootloaders, here a dumb 32-bit thingy is
 done instead to simplify the example.
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

/** RTOS init function */
void rtos_init(void)
{
    printf("RTOS Booted!\n");
}

/** Execute tasks in regular mode */
void rtos_loop(void)
{
    return;
}

int main(void)
{
    //Check if fw update pending, apply if so
    checkfwupdate_original();
    
    //Check if fw update pending, apply if so
    //checkfwupdate_armoured();
    
    //No firmware update - start regular operations
    rtos_init();
    
    while(1){
        rtos_loop();
    }
}

/**
 Firmware update function, called once we know all images are OK.
 */
 void boot_new_image(image_t * image)
 {
     printf("Booting image %s\n", image.image_name);
     
     //Here would go code to actually do the real stuff
 }

/**
 Original function which is vulnerable to FI.
 */
int checkfwupdate_original(void)
{
    //Flag indicates new firmware file present
    if(bootloader_flag == FLAG_PENDING_UPDATE){
        
        //Check signature matches proposed hashes
        if (validate_sigature(image.short_hash, manf_public_key)) {
            
            //Check hash matches image we should boot
            if(some_hash_function(image.image_data, image.image_data_len) == image.short_hash){
                boot_new_image(image);
            } else {
                //provided image doesn't match hash
                bootloader_flag = 0;
            }
        } else {
            //signature failed
            bootloader_flag = 0;
        }
    }
}

/**
 Modified with CA calls.
 */
int checkfwupdate_armoured(void)
{
    ;
}