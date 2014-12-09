/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
    perror(s);
    abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1000000;
static uint16_t delay;

static void transfer(int fd, uint8_t * tx, uint8_t * rx, uint8_t len)
{
    int ret;
    
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");
/*
    for (ret = 0; ret < len; ret++) {
        if (!(ret % 6))
            puts("");
        if (ret > 3)
            printf("%.2X ", rx[ret]);
    }
    puts("");*/
}


int main(int argc, char *argv[])
{
    int ret = 0;
    int fd;

    uint8_t spi_tx[256];
    uint8_t spi_rx[256];

    FILE *soundfile;

    soundfile = fopen("t.raw", "rb");


    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    mode |= SPI_CPHA;
    mode |= SPI_CPOL;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    int i;
    for (i =0; i<256; i++) {
        spi_tx[i] = 0;
        spi_rx[i] = 0;
    }

    // command and address
 //   spi_tx[0] = 0x03;
 //   spi_tx[1] = 0x00;
 //   spi_tx[2] = 0x00;
  //  spi_tx[3] = 0x00;
    /*
    puts("erasing flash...");
    spi_tx[0] = 0x06;  //wren
    transfer(fd, spi_tx, spi_rx, 1);
    spi_tx[0] = 0xc7;  // bulk erase
    transfer(fd, spi_tx, spi_rx, 1);
    sleep(5);  // wait for it
    puts("erased.");
*/

// write some stuff
    for (i =0; i<256; i++) 
        spi_tx[i] = 'o';

    spi_tx[0] = 0x06;  //wren
    transfer(fd, spi_tx, spi_rx, 1);

    spi_tx[0] = 0x02; // write
    spi_tx[1] = 0;
    spi_tx[2] = 0;
    spi_tx[3] = 0;
    transfer(fd, spi_tx, spi_rx, 128);

    


    //for (i = 0; i < 100; i++) {
  /*  for(i=0; i<1000; i++){
        spi_tx[0] = 0x03;
        spi_tx[1] = i>>16;
        spi_tx[2] = i>>8;
        spi_tx[3] = i;

       transfer(fd, spi_tx, spi_rx, 128);
        fwrite(spi_rx, 128, 1, write_ptr);
    }*/
    fclose(soundfile);
    //}

    close(fd);

    return ret;
}
