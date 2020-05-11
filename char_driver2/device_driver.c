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

#define GETBIT(v,n)
#define SETBIT(v,n)

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

mouse_t *mouse_new (struct usb_device *dev){
    mouse_t *mouse;
    if(!(mouse = kmalloc(sizeof(mouse_t), GFP_KERNEL))){
        return NULL;
    }
    touchpad_init(mouse);
    if((mouse->inputdev = input_allocate_device()) == NULL){
        touchpad_free(mouse);
        return NULL;
    }

    if(!(mouse->urb = usb_alloc_urb(0, GFP_KERNEL))){
        touchpad_free(mouse);
        return NULL;
    }

    if(!(mouse->data = usb_buffer_alloc(dev, 8, ATOMIC, &mouse->data_dma))){
        touchpad_free(mouse);
        return NULL;
    }

    mouse->usbdev = dev;
    return mouse;
}

input_report_key(dev, KEY_A,    GETBIT(btn,0));

void input_send_data (struct input_Dev *dev, char *data){
    char btn = data[0];
    x = data[1];
    y = data[2];
    w = data[3];


    input_report_key(dev, BTN_LEFT, GETBIT(btn, 0));
    input_report_key(dev, BTN_RIGHT, GETBIT(btn, 1));
    input_report_key(dev, BTN_MIDDLE, GETBIT(btn, 2));
    input_report_key(dev, BTN_SIDE, GETBIT(btn, 3));
    input_report_key(dev, BTN_EXTRA, GETBIT(btn, 4));
    input_report_key(dev, REL_X, x);
    input_report_key(dev, REL_Y, y);
    input_report_key(dev, REL_WHEEL, w);

}

static void usb_mouse_handle(struct urb *urb){
    mouse_t *mouse = urb->context;
    signed char *data = mouse->data;
    struct input_dev *inputdev = mouse->inputdev;

    if(urb->status){
        return ;
    }

    input_send_data(inputdev, data);
    input_sync(inputdev);
    usb_submit_urb(urb, ATOMIC);
}


static int usb_touchpad_open(struct input_dev *dev){
    mouse_t *mouse = input_get_drvdata(dev);
    if(mouse->open++){
        return 0;
    }

    mouse->urb->dev = mouse->usbdev;
    if(usb_submit_urb(mouse->urb, GFP_KERNEL)){
        mouse->open--;
        return -EIO;
    }

    return 0;
}


static void usb_touchpad_close(struct input_dev *dev){
    mouse_t mouse = input_get_drvdata(dev);
    if(!--mouse->open){
        usb_kill_urb(mouse->urb);
    }
}


static struct usb_driver usb_touchpad_driver = {
    .name = "msi_touchpad",
    .probe = usb_mouse_probe,
    .disconnect = usb_mouse_disconnect,
    .id_table = usb_mouse_id_table
};


static struct usb_device_id usb_mouse_id_table [] = {
{    USB_DEVICE(0x09da, 0x021f) },
{ }
};

MODULE_DEVICE_TABLE (usb, usb_mouse_id_table); 

static void usb_mouse_disconnect(struct usb_interface *intf){
    mouse_t *mouse = usb_get_intfdata(intf);
    usb_set_intfdata(intf, NULL);
    if(mouse){
        usb_kill_urb(mouse->urb);
        input_unregister_device(mouse->inputdev);
        touchpad_free(mouse);
    }
}

static int __init usb_touchpad_init(void){
    int rv;
    rv = usb_register(&usb_touchpad_driver);
    return rv;
}

rv = usb_register(&usb_touchpad_driver);


static void __exit usb_touchpad_exit(void){
    usb_deregister(&usb_touchpad_driver);
}

module_init(usb_touchpad_init);
module_exit(usb_touchpad_exit);

static int usb_a4r7mouse_probe(struct usb_interface *intf, const struct usb_device_id *id) {?}