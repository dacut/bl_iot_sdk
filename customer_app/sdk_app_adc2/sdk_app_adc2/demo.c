//  Measure the ambient brightness with an LED configured as ADC Input.
//  Note: ADC Gain must be set to ADC_PGA_GAIN_1 in components/hal_drv/bl602_hal/bl_adc.c:
//  int bl_adc_init(int mode, int gpio_num) {
//    ...
//    adccfg.gain1=ADC_PGA_GAIN_1;  // Previously: ADC_PGA_GAIN_NONE
//    adccfg.gain2=ADC_PGA_GAIN_1;  // Previously: ADC_PGA_GAIN_NONE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cli.h>
#include <bl_adc.h>     //  For BL602 ADC Hardware Abstraction Layer
#include <bl_dma.h>     //  For BL602 DMA Hardware Abstraction Layer
#include "demo.h"

/// GPIO Pin Number that will be configured as ADC Input.
/// PineCone Blue LED is connected on BL602 GPIO 11.
/// PineCone Green LED is connected on BL602 GPIO 14.
/// Only these GPIOs are supported: 4, 5, 6, 9, 10, 11, 12, 13, 14, 15
/// TODO: Change the GPIO Pin Number for your BL602 board
#define ADC_GPIO 11

//  We set the ADC Frequency to 10 kHz according to https://wiki.analog.com/university/courses/electronics/electronics-lab-led-sensor?rev=1551786227
//  This is 10,000 samples per second.
#define ADC_FREQUENCY 10000  //  Hz

//  We shall read 1,000 ADC samples, which will take 0.1 seconds
#define ADC_SAMPLES 1000

/// Init the ADC Channel
void init_adc(char *buf, int len, int argc, char **argv) {
    //  Only these GPIOs are supported: 4, 5, 6, 9, 10, 11, 12, 13, 14, 15
    assert(ADC_GPIO==4 || ADC_GPIO==5 || ADC_GPIO==6 || ADC_GPIO==9 || ADC_GPIO==10 || ADC_GPIO==11 || ADC_GPIO==12 || ADC_GPIO==13 || ADC_GPIO==14 || ADC_GPIO==15);

    //  For Single-Channel Conversion Mode, frequency must be between 500 and 16,000 Hz
    assert(ADC_FREQUENCY >= 500 && ADC_FREQUENCY <= 16000);

    //  Init the ADC Frequency for Single-Channel Conversion Mode
    int rc = bl_adc_freq_init(1, ADC_FREQUENCY);
    assert(rc == 0);

    //  Init the ADC GPIO for Single-Channel Conversion Mode
    rc = bl_adc_init(1, ADC_GPIO);
    assert(rc == 0);

    //  Init DMA for the ADC Channel for Single-Channel Conversion Mode
    rc = bl_adc_dma_init(1, ADC_SAMPLES);
    assert(rc == 0);

    //  Configure the GPIO Pin as ADC Input, no pullup, no pulldown
    rc = bl_adc_gpio_init(ADC_GPIO);
    assert(rc == 0);

    //  Get the ADC Channel Number for the GPIO Pin
    int channel = bl_adc_get_channel_by_gpio(ADC_GPIO);

    //  Get the DMA Context for the ADC Channel
    adc_ctx_t *ctx = bl_dma_find_ctx_by_channel(ADC_DMA_CHANNEL);
    assert(ctx != NULL);

    //  Indicate that the GPIO has been configured for ADC
    ctx->chan_init_table |= (1 << channel);

    //  Start reading the ADC via DMA
    bl_adc_start();
}

/// Read the ADC Channel and compute the average value of the ADC Samples
void read_adc(char *buf, int len, int argc, char **argv) {
    //  Static array that will contain 1,000 ADC Samples
    static uint32_t adc_data[ADC_SAMPLES];

    //  Get the ADC Channel Number for the GPIO Pin
    int channel = bl_adc_get_channel_by_gpio(ADC_GPIO);
    
    //  Get the DMA Context for the ADC Channel
    adc_ctx_t *ctx = bl_dma_find_ctx_by_channel(ADC_DMA_CHANNEL);
    assert(ctx != NULL);

    //  Verify that the GPIO has been configured for ADC
    assert(((1 << channel) & ctx->chan_init_table) != 0);

    //  If ADC Sampling is not finished, try again later    
    if (ctx->channel_data == NULL) {
        printf("ADC Sampling not finished\r\n");
        return;
    }

    //  Copy the read ADC Samples to the static array
    memcpy(
        (uint8_t*) adc_data,             //  Destination
        (uint8_t*) (ctx->channel_data),  //  Source
        sizeof(adc_data)                 //  Size
    );  

    //  TODO: Compute the average value of the ADC Samples
}

///////////////////////////////////////////////////////////////////////////////
//  Command Line Interface

/// List of commands. STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"init_adc",        "Init ADC Channel",          init_adc},
    {"read_adc",        "Read ADC Channel",          read_adc},
};                                                                                   

/// Init the command-line interface
int cli_init(void)
{
   //  To run a command at startup, do this...
   //  command_name("", 0, 0, NULL);
   return 0;
}

/// TODO: We now show assertion failures in development.
/// For production, comment out this function to use the system default,
/// which loops forever without messages.
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    //  Show the assertion failure, file, line, function name
	printf("Assertion Failed \"%s\": file \"%s\", line %d%s%s\r\n",
        failedexpr, file, line, func ? ", function: " : "",
        func ? func : "");
	//  Loop forever, do not pass go, do not collect $200
	for (;;) {}
}

///////////////////////////////////////////////////////////////////////////////
//  Dump Stack

/// Dump the current stack
void dump_stack(void)
{
    //  For getting the Stack Frame Pointer. Must be first line of function.
    uintptr_t *fp;

    //  Fetch the Stack Frame Pointer. Based on backtrace_riscv from
    //  https://github.com/bouffalolab/bl_iot_sdk/blob/master/components/bl602/freertos_riscv_ram/panic/panic_c.c#L76-L99
    __asm__("add %0, x0, fp" : "=r"(fp));
    printf("dump_stack: frame pointer=%p\r\n", fp);

    //  Dump the stack, starting at Stack Frame Pointer - 1
    printf("=== stack start ===\r\n");
    for (int i = 0; i < 128; i++) {
        uintptr_t *ra = (uintptr_t *)*(unsigned long *)(fp - 1);
        printf("@ %p: %p\r\n", fp - 1, ra);
        fp++;
    }
    printf("=== stack end ===\r\n\r\n");
}

/* Output Log
# init_adc

[In darkness]

# read_adc
Average: 1415
value=1415

# read_adc
Average: 1415
value=1415

# read_adc
Average: 1414
value=1415

[In sunlight]

# read_adc
Average: 1410
value=1411

# read_adc
Average: 1411
value=1411

# read_adc
Average: 1410
value=1410

[In darkness]

# read_adc
Average: 1415
value=1416

# read_adc
Average: 1415
value=1416

# read_adc
Average: 1415
value=1416
*/