#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/input.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/usb.h>

#define DRIVER_VERSION "v0.0.1"
#define DRIVER_AUTHOR "Sevket Ozurfali sevketozurfali@gmail.com"
#define DRIVER_DESC "Tocuhpad driver for msi gf63"
#define DRIVER_LICENSE "GPL"

MODULE_AUTHOR ( DRIVER_AUTHOR )
MODULE_DESCRIPTION ( DRIVER_DESC )
MODULE_LICENSE (DRIVER_LICENSE )

#ifdef SLAB_ATOMIC
# define ATOMIC SLAB_ATOMIC
#else
# define ATOMIC GFP_ATOMIC
#endif

typedef struct touchpad {
 // Cihazın adı
 char name[128];  
 // USB gelen interrupt verisi. İşte cihazdan gelen verilere buradan erişeceğiz.
 signed char *data;  
 //TODO bunun ne anlama geldiğini bul
 char phys[64];
 // cihazın DMA adresi
 dma_addr_t data_dma;
 //USB portuna takılan cihazın logic gösterimini tutan struct
 struct usb_device *usbdev;
 // cihazdan girdileri almada kullanacağımız struct
 struct input_dev *inputdev;
 // cihazdan gelen USB Request block
 struct urb *urb;
 // cihazın açılma sayısı
 int open;
} mouse_t;
//En son olarak bu yapıdan a4r7mouse_t adında bir örnek oluşturuyoruz.


void touchpad_init(mouse_t *mouse){
    memset(mouse, 0, sizeof(mouse_t));
    mouse->inputdev = NULL;
    mouse->urb = NULL;
    mouse->data = NULL;
}

void touchpad_free(mouse_t *mouse){
    if(mouse->urb){
        usb_free_urb(mouse->urb);
    }
    if(mouse->data){
        usb_buffer_free(mouse->usbdev, 8, mouse->data, mouse->data_dma);
        kfree(mouse);
    }
}


