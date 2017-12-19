/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#include "modbus.h"

/* The goal of this program is to check all major functions of
   libmodbus:
   - write_coil
   - read_bits
   - write_coils
   - write_register
   - read_registers
   - write_registers
   - read_registers

   All these functions are called with random values on a address
   range defined by the following defines.
*/
#define SERVER_ID                                   0xFE

#define NUM_MEASUREMENTS_FOR_CALIBRATION            1
#define NUM_ANALOG_CHANNELS                         8

#define ADDRESS_START                               0xB86
#define ADDRESS_END                                 ADDRESS_START + (NUM_ANALOG_CHANNELS -1) * 2
#define SLEEP_TIME                                  300000

#define THEORETICAL_VALUE_LOW_SCALE                 750UL
#define THEORETICAL_VALUE_HIGH_SCALE                15000UL

//Variables
uint16_t tab_rp_registers[NUM_ANALOG_CHANNELS][NUM_MEASUREMENTS_FOR_CALIBRATION*2];
uint16_t calibration_a[NUM_ANALOG_CHANNELS];
uint16_t calibration_b[NUM_ANALOG_CHANNELS];
uint16_t address_list[] = {0x1628, 0x162A, 0x162C, 0x1637, 0x1639, 0x163B};
uint16_t address_list_dates[] = {0x161F, 0x1622, 0x1625, 0x162E, 0x1631, 0x1634};
char * text_list [] = {"Test Coil A Max open current", "Test Coil B Max open current", "Test Coil C Max open current",
                        "Test Coil A Max close current", "Test Coil B Max close current", "Test Coil C Max close current"};
char * text_list_dates [] = {"Test Coil A Max open current Date", "Test Coil B Max open current Date", "Test Coil C Max open current Date",
                        "Test Coil A Max close current Date", "Test Coil B Max close current Date", "Test Coil C Max close current Date"};

uint16_t single_registers[] = {0x0001, 0x0001, 0x0};

//functions
void print_Analog_Measurements (void);
void Calibration_calculate_a (void);
void Calibration_calculate_b (void);
void print_current_time_with_ms (void);

/* At each loop, the program works in the range ADDRESS_START to
 * ADDRESS_END then ADDRESS_START + 1 to ADDRESS_END and so on.
 */
int main(int num_opt, char * argv)
{
    modbus_t *ctx;
    int rc;
    int nb_fail;
    int nb_loop;
    int addr;
    int nb;
    uint16_t i;
    uint16_t read_reg[3] = {0};
    uint32_t read_var = 0;
    float *p_value = NULL;
    char convert[4] = {0};
    char *p_char = NULL;

    /* RTU */
/*
    ctx = modbus_new_rtu("/dev/ttyUSB0", 19200, 'N', 8, 1);
    modbus_set_slave(ctx, SERVER_ID);
*/
    print_current_time_with_ms();

    /* TCP */
    ctx = modbus_new_tcp("10.3.35.15", 502);
    modbus_set_slave(ctx, SERVER_ID);
    //modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Allocate and initialize the different memory spaces */
    memset(tab_rp_registers, 0, sizeof(tab_rp_registers));

#if 0 //Read Test Opening current
    
            /* SINGLE REGISTER */    
          for (i = 0; i <6; i++) {
            rc = modbus_read_input_registers(ctx, address_list[i], 1, &read_reg[0]);
            if (rc != 1) {
                printf("ERROR modbus_read_registers single (%d)\n", rc);
                printf("Address = %x\n", address_list[i]);
                nb_fail++;
            }
            rc = modbus_read_input_registers(ctx, address_list[i]+1, 1, &read_reg[1]);
            if (rc != 1) {
                printf("ERROR modbus_read_registers single (%d)\n", rc);
                printf("Address = %x\n", address_list[i]);
                nb_fail++;
            }
            p_char = &read_reg[0];
            convert[0] = (char)(*(p_char+2));
            convert[1] = (char)(*(p_char+3));
            convert[2] = (char)(*(p_char+0));
            convert[3] = (char)(*(p_char+1));
            p_value = &convert[0];
            //printf("Reg 0x%x %s = 0x%x%x\n", address_list[i], text_list[i], read_reg[0], read_reg[1]);
            printf("Reg 0x%x %s = %g\n", address_list[i], text_list[i], *p_value);
          }
#endif

          printf("\n\n\n");

#if 0 //Read Test Opening current dates
    
            /* SINGLE REGISTER */    
          for (i = 0; i <6; i++) {
            rc = modbus_read_input_registers(ctx, address_list_dates[i], 1, &read_reg[0]);
            if (rc != 1) {
                printf("ERROR modbus_read_registers single (%d)\n", rc);
                printf("Address = %x\n", address_list_dates[i]);
                nb_fail++;
            }
            rc = modbus_read_input_registers(ctx, address_list_dates[i]+1, 1, &read_reg[1]);
            if (rc != 1) {
                printf("ERROR modbus_read_registers single (%d)\n", rc);
                printf("Address = %x\n", address_list_dates[i]);
                nb_fail++;
            }
            rc = modbus_read_input_registers(ctx, address_list_dates[i]+2, 1, &read_reg[2]);
            if (rc != 1) {
                printf("ERROR modbus_read_registers single (%d)\n", rc);
                printf("Address = %x\n", address_list_dates[i]);
                nb_fail++;
            }
            p_char = &read_reg[0];
            //convert[0] = (char)(*(p_char+2));
            //convert[1] = (char)(*(p_char+3));
            //convert[2] = (char)(*(p_char+0));
            //convert[3] = (char)(*(p_char+1));
            //p_value = &convert[0];
            printf("Reg 0x%x %s = 0x%02x%02x%02x\n", address_list_dates[i], text_list_dates[i], read_reg[0], read_reg[1],read_reg[2]);
            
          }
#endif

#if 1 //Send Calibration message to Driving Electronics Board
    /* Single register */
    printf("modbus_write_register(ctx, 0x1F42, 1, 0x0001)\n");
    rc = modbus_write_registers(ctx, 0x1F42, 1, &single_registers[0]);
    if (rc != 1) {
                printf("ERROR modbus_write_register single (%d)\n", rc);
                printf("Address = %d\n", 0x1F42);
                nb_fail++;
            }
    printf("modbus_write_registers(ctx,  1,0x1FE4, 0x0001)\n");
    rc = modbus_write_registers(ctx, 0x1FE4, 1, &single_registers[1]);
    if (rc != 1) {
                printf("ERROR modbus_write_register single (%d)\n", rc);
                printf("Address = %d\n", 0x1FE4);
                nb_fail++;
            }
    sleep(2);
    printf("modbus_write_register(ctx, 0x1F42, 1, 0x0001)\n");
    rc = modbus_write_registers(ctx, 0x1F42, 1, &single_registers[2]);
    if (rc != 1) {
                printf("ERROR modbus_write_register single (%d)\n", rc);
                printf("Address = %d\n", 0x1F42);
                nb_fail++;
            }
    printf("modbus_write_registers(ctx,  1,0x1FE4, 0x0001)\n");
    rc = modbus_write_registers(ctx, 0x1FE4, 1, &single_registers[1]);
    if (rc != 1) {
                printf("ERROR modbus_write_register single (%d)\n", rc);
                printf("Address = %d\n", 0x1FE4);
                nb_fail++;
            }
#endif
#if 0
    nb_loop = nb_fail = 0;
    for (nb_loop = 0; nb_loop < NUM_MEASUREMENTS_FOR_CALIBRATION; nb_loop++){
        for (addr = ADDRESS_START+1, i = 0; addr < ADDRESS_END+1; addr=addr+2, i++) {
            /* SINGLE REGISTER */
            printf(".");
            rc = modbus_read_input_registers(ctx, addr, 1, &tab_rp_registers[i][nb_loop]);
            if (rc != 1) {
                printf("ERROR modbus_read_registers single (%d)\n", rc);
                printf("Address = %d\n", addr);
                nb_fail++;
            }
        }
        usleep(SLEEP_TIME);
    }
    printf("\n");

    /* Free the memory */
    //free(tab_rp_registers);

    print_Analog_Measurements();
    Calibration_calculate_a();
    //Calibration_calculate_b();

#endif
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}

void print_Analog_Measurements (void)
{
  int index_channel=0, index_sample=0;

  for (index_sample = 0; index_sample < NUM_MEASUREMENTS_FOR_CALIBRATION; index_sample++){
      for (index_channel = 0; index_channel < NUM_ANALOG_CHANNELS; index_channel++) {
          /* SINGLE REGISTER */
          printf ("Chn %d: %d\n", index_channel, tab_rp_registers[index_channel][index_sample]);
      }
  }
}

void Calibration_calculate_b (void)
{
  //int index_channel=0, index_sample=0;


}

void Calibration_calculate_a (void)
{
  int index_channel=0, index_sample=0;
  double a = 0;
  unsigned long long int sumNum1 = 0, sumNum2_1 = 0, sumNum2_2 = 0, sumDen1 = 0, sumDen2 = 0;

  for (index_channel = 0; index_channel < NUM_ANALOG_CHANNELS; index_channel++) {
    sumNum1 = 0; sumNum2_1 = 0; sumNum2_2 = 0; sumDen1 = 0; sumDen2 = 0;
    for (index_sample = 0; index_sample < NUM_MEASUREMENTS_FOR_CALIBRATION; index_sample++){
      sumNum1 += (THEORETICAL_VALUE_HIGH_SCALE * tab_rp_registers[index_channel][index_sample]);
      sumNum2_1 += THEORETICAL_VALUE_HIGH_SCALE;
      sumNum2_2 += tab_rp_registers[index_channel][index_sample];
      sumDen1 += (tab_rp_registers[index_channel][index_sample] * tab_rp_registers[index_channel][index_sample]);
      sumDen2 += (tab_rp_registers[index_channel][index_sample]);
    }
    //a = (double)((NUM_MEASUREMENTS_FOR_CALIBRATION*sumNum1)-(sumNum2_1*sumNum2_2))/((NUM_MEASUREMENTS_FOR_CALIBRATION*sumDen1)-(sumDen2*sumDen2));
    printf ("Chn %d: %lld %lld %lld %lld %lld a=%g\n", index_channel, sumNum1, sumNum2_1, sumNum2_2, sumDen1, sumDen2, a);
      //printf ("%g %g\n", (double)(NUM_MEASUREMENTS_FOR_CALIBRATION*sumNum1), (double)(sumNum2_1*sumNum2_2));
  }
}

void print_current_time_with_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    printf("Current time: %"PRIdMAX".%03ld seconds since the Epoch\n",
           (intmax_t)s, ms);
}
