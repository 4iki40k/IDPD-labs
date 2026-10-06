#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define BAUD     9600      /* <-- підстав baudrate свого варіанту */
#define BUF_SIZE 64

static void clock_setup(void)
{
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);   /* Blue Pill: кварц 8 МГц */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_USART1);
}

static void gpio_setup(void)
{
    /* PA9 - TX: альтернативна функція, push-pull */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
    /* PA10 - RX: плаваючий вхід */
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
    /* PC13 - світлодіод (активний 0) */
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_set(GPIOC, GPIO13);
}

static void usart_setup(void)
{
    usart_set_baudrate(USART1, BAUD);
    usart_set_databits(USART1, 8);               /* 8 біт даних */
    usart_set_parity(USART1, USART_PARITY_NONE); /* N */
    usart_set_stopbits(USART1, USART_STOPBITS_1);/* 1 */
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);
}

static void uart_puts(const char *s)
{
    while (*s) usart_send_blocking(USART1, (uint8_t)*s++);
}

int main(void)
{
    char buf[BUF_SIZE];
    unsigned n = 0;

    clock_setup();
    gpio_setup();
    usart_setup();

    while (1) {
        char c = (char)usart_recv_blocking(USART1);

        if (c == '\r') {                 /* кінець пакета */
            buf[n] = '\0';
            gpio_toggle(GPIOC, GPIO13);  /* індикація прийому */

            uart_puts("ACK:");
            uart_puts(buf);
            usart_send_blocking(USART1, '\n');
            n = 0;
        } else if (n < BUF_SIZE - 1) {
            buf[n++] = c;
        }
    }
}
