#include <tusb.h>



void tuh_mount_cb(uint8_t dev_addr)
{
    // application set-up
    printf("A device with address %d is mounted\r\n", dev_addr);
}

void tuh_umount_cb(uint8_t dev_addr)
{
    // application tear-down
    printf("A device with address %d is unmounted \r\n", dev_addr);
}

void setup() {
    printf("\033[2J\033[H");
    tusb_init();
}

void loop() {
    tuh_task();
}
