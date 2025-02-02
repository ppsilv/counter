/*
    Autor.: Paulo da Silva (pgordao)
    Data..: 25/01/2025
    Versão: 2504
    Descrição: 
    Esse programa configura um ou mais pinos GPIO em uma maquina de estado da PIO,
    com isso podemos escrever no TX FIFO da máquina de estado e a PIO lê esse dado
    e o coloca no(os) pino(s) configurados para recebe-lo(s).
    A maquina de estado fica parada no modo de leitura do TX FIFO quando ele estiver vazio. 

    Esse programa foi totalmente baseado no programa hello.pio
    que consta nos exemplos do Raspiberry Pi Pico. 
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "counter.pio.h"      // Esse arquivo é auto gerado.

#undef PIO_PIN 

#define PIO_PIN 0
#define TOTAL_PINOS 8

// Check the pin is compatible with the platform
#if PIO_PIN >= NUM_BANK0_GPIOS
#error ERRO: o número do pino tem que ser menor ou igual a 26
#endif
/*
    Esta função inicializa a PIO.
    O nome dessa função tem que obrigatóriamente iniciar com o nome do programa + underscore,
    nesse caso como o programa chama-se hello tem-se que acrescentar o prefixo hello_ ao nome da
    função ficando hello_program_init(...).
*/
static inline void init_pio(PIO pio, uint sm, uint offset, uint pin) 
{
    // Busca a configuration padrão da máquina de estado(state machine)
    pio_sm_config c = counter_program_get_default_config(offset);

    // Map the state machine's OUT pin group to one pin, namely the `pin`
    // parameter to this function.
    // Mapeia (o)s pino(s), no caso de mais de um pino mapear o primeiro. 
    sm_config_set_out_pins(&c, pin, TOTAL_PINOS);

    // Set this pin's GPIO function (connect PIO to the pad)
    // Inicializa o(s) pino(s) 
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin+1);
    pio_gpio_init(pio, pin+2);
    pio_gpio_init(pio, pin+3);
    pio_gpio_init(pio, pin+4);
    pio_gpio_init(pio, pin+5);
    pio_gpio_init(pio, pin+6);
    pio_gpio_init(pio, pin+7);


    // Configura a direção do(s) pino(s) se input(=false) ou output(=true)
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+1, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+2, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+3, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+4, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+5, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+6, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin+7, 1, true);

    // Carrega a configuração e pula para o inicio do programa.
    pio_sm_init(pio, sm, offset, &c);

    // Habilita o estado da state machi como "running".
    pio_sm_set_enabled(pio, sm, true);
}

int main() {
    PIO pio;
    uint sm;
    uint offset;
    uint counter=0;

    // Busca uma state machine vazia dentre as disponíveis.
    // Observe que as variáveis pipo sm e offset foram passadas como ponteiros
    // permitindo assim que a função altere seus valores para que possamos
    // usar a máquina de estado.
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&counter_program, &pio, &sm, &offset, PIO_PIN, 1, true);
    // Somente valida o retorno se é um dos possíveis.
    hard_assert(success);

    // Chama a função init_pio definida no hello.pio, para configurar e iniciar o programa.
    init_pio(pio, sm, offset, PIO_PIN);

    // Nesse ponto a máquina de estado está executando suas tarefas, e a primeira
    // é ler o TX FIFO que nesse momento não tem nada assim ela está parada na instrução pull
    // do nosso program hello.pio, quando enviarmos dados que nesse caso é um contador de 0 a 255
    // o valor do contador aparecerá nos pinos configurados, nesse caso no GPIO0 a GPIO7
    while (getchar_timeout_us(0) == PICO_ERROR_TIMEOUT) {
        // Escreve o valor no TX FIFO
        pio_sm_put_blocking(pio, sm, counter++);
        sleep_ms(500);
    }

    // Essa é a forma correta de terminar algo com a PIO
    //pio_remove_program_and_unclaim_sm(&hello_program, pio, sm, offset);

}
