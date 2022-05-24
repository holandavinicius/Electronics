/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F45K22
        Driver Version    :  2.00
 */

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */

#include "mcc_generated_files/mcc.h"
#include "xlcd.h"
#include <stdio.h>
unsigned char update = 0, second = 0;
char temperatura[20], pressao[20], a_abertura[20];
float temperature, pressure, voutt, voutp, abertura, temperatureMax = 100, temperatureMin = 70, pressureMax = 80, pressureMin = 60;
unsigned int opca = 4;
uint8_t rxData = 0, i = 0;
int mostra_menu = 1, menu = 0, opcaoMenu = 0;
char menuIdx = '0', caracter_recebido = 0;
uint16_t start, inicio = 61599;

void funcaoADC(void) { //Fun��o de leitura e c�lculo dos valores do ADC
    second = 1; //Vari�vel de contagem de tempo (1 segundo)
    if (update == 0) {
        voutt = ADC_GetConversionResult();
        ADC_SelectChannel(channel_AN17);
        ADC_StartConversion();
        temperature = (5 * voutt / (1024 * 0.01)) - 50;
        update = 1;
    } else {
        voutp = ADC_GetConversionResult();
        pressure = (5 * voutp / (1024 * 0.01845)) - 10.8401084;
        update = 0;
    }
}

void LCD(char parameter1[20], char parameter2[20]) { //Fun��o de escrita no LDC
    WriteCmdXLCD(0b00000001);
    while (BusyXLCD());
    WriteCmdXLCD(0b10000001);
    while (BusyXLCD());
    putsXLCD(parameter1);
    while (BusyXLCD());
    WriteCmdXLCD(0b11000001);
    while (BusyXLCD());
    putsXLCD(parameter2);
    while (BusyXLCD());
    
}
void motor_angles(void) { //Fun��o de controle do angulo do motor
    TMR3_StopTimer();
    PORTCbits.RC2 = 0; //Desliga o sinal do motor ap�s o tempo de �ngulo

}

void interrupt_motor(void) { //Fun��o de interrup��o dos 20ms do motor
    PORTCbits.RC2 = 1; //Motor inicia a contagem em HIGH
    TMR3_WriteTimer(inicio); //In�cio da contagem do �ngulo do motor
    TMR3_StartTimer();

}

void Botao_inicio(void) {
    opca = 1; //Vari�vel de controle selecionada para o in�cio de inje��o
    inicio = 61599; //V�lvula come�a a inje��o fechada
}

void Botao_emergencia(void) {
    opca = 5; // Seleciona a fun��o de escrita "EMERGENCIA"
    inicio = 61599; // Fecha a v�lvula
    INTCON3bits.INT2IF = 0; //Desliga a flag de interrup��o do bot�o
}

void tmr0_select(void) { //Fun��o que controla o tempo das fun��es de inje��o
    ADC_SelectChannel(channel_AN15); //Sele��o do canal ADC
    ADC_StartConversion(); //Inicio da convers�o e habilitagem da interrup��o ADC
    switch (opca) { //Menu de sele��o da vari�vel de controle
        case 1: //Come�a a fase 1
            i++; // Incremento da Vari�vel de contagem por segundo
            if (i == 8) { //Fim dos 8 segundos
                i = 0; //Reinicia a vari�vel de contagem
                opca = 2; //Seleciona Fase 2
            }
            break;
        case 2: //Inicia Fase 2
            i++; //Incremento da Vari�vel de contagem por segundo
            if (i == 10) { //Fim dos 10 segundos
                i = 0; //Reinicia a vari�vel de contagem
                opca = 3; //Seleciona Fase 3
            }
            break;
        case 3: //Inicia Fase 3
            i++; //Incremento da Vari�vel de contagem por segundo
            if (i == 8) { //Fim dos 8 segundos
                i = 0; //Reinicia a vari�vel de contagem
                opca = 4; //Seleciona o aguardo de in�cio
                inicio = 61599; // Configura timer para motor em -180�
                TMR1_StartTimer(); //Motor em -180�
            }
            break;
        case 5: //Vari�vel de controle em EMERG�NCIA
            i++; //Incremento da Vari�vel de contagem por segundo
            if (i == 5) { //Fim dos 5 segundos
                i = 0; //Reinicia Vari�vel de contagem
                opca = 4; //Seleciona o aguardo de inicio
                inicio = 61599; //Configura timer para motor a -180�
                TMR1_StartTimer(); //Motor em -180�
            }
    }
}

/*
                         Main application
 */
void main(void) {
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable high priority global interrupts GIEH
    INTERRUPT_GlobalInterruptHighEnable();

    // Enable low priority global interrupts. GIEL
    INTERRUPT_GlobalInterruptLowEnable();

    // Disable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighDisable();

    // Disable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    //INTERRUPT_InterruptManagerHigh();
    
    TMR0_SetInterruptHandler(tmr0_select); //Fun��o de Interrup��o do timer 0
    TMR1_SetInterruptHandler(interrupt_motor); //Fun��o de Interrup��o do timer 1
    TMR3_SetInterruptHandler(motor_angles); //Fun��o de Interrup��o do timer 3
    INT0_SetInterruptHandler(Botao_inicio); //Fun��o de Interrup��o do botao inicio
    INT2_SetInterruptHandler(Botao_emergencia); //Fun��o de Interrup��o do botao emergencia
    ADC_SetInterruptHandler(funcaoADC); //Fun��o de Interrup��o do ADC
    //CCPR1H = 

    OpenXLCD(0b00101011); //Inicializa��o do LCD
    while (BusyXLCD());
    WriteCmdXLCD(0b00001100);
    while (BusyXLCD());
    WriteCmdXLCD(0b00010111);
    while (BusyXLCD());
    WriteCmdXLCD(0b00000001); //Limpeza do LCD
    while (BusyXLCD());


    while (1) { //Loop infinito MAIN
        if (second == 1) { //Fun��o de controle de tempo
            switch (opca) { //Sele��o de menu de opera��o

                case 5: //Opera��o EMERGENCIA
                    LCD("    EMERGENCIA    ","     !!!!!!!!          "); //Escreve no LCD
                    second = 0; //Vari�vel de tempo em 0
                    break;
                case 4: //Aguardo de inicio
                    LCD("----AGUARDANDO-----","------INICIO------"); //Escreve no LCD
                    second = 0; //Vari�vel de tempo em 0
                    break;
                case 0: //Indica��o de valores dos sensores
                    sprintf(temperatura, "TEMP: %.0f  C", temperature); 
                    sprintf(pressao, "PRESSAO: %.1f kPa", pressure);
                    LCD(temperatura,pressao); //Escrita dos valores no LCD
                    second = 0;//Vari�vel de tempo em 0
                    break;
                case 1: //Fase 1
                    if (temperatureMin <= temperature && temperature <= temperatureMax && pressureMin <= pressure && pressure <= pressureMax) { //Verifica�ao de parametros
                        TMR1_StartTimer(); //Inicializa��o do motor
                        inicio = inicio - 511; //Atualiza�ao da vari�vel do angulo
                        abertura = 360 * (((float) (61599 - inicio)) / 4094) - 180; //C�lculo do angulo
                        sprintf(a_abertura, "    %.0f GRAUS", abertura); 
                        LCD("------FASE 1------", a_abertura); //Escrita no LCD
                    }
                    if (temperatureMin >= temperature || temperature >= temperatureMax) { //Erro de temperatura
                        LCD("ERRO DE TEMPERATURA","     !!!!!!!               "); //Escrita no LCD
                        TMR1_StartTimer(); //Liga motor
                        inicio = 61599; //Motor em 180� (v�lvula fechada))
                        opca = 0; //Retorno para indica��o dos valores dos sensores
                    }
                    if (pressureMin >= pressure || pressure >= pressureMax) { //Erro de press�o
                        LCD("     ERRO DE PRESSAO","     !!!!!!!               "); //Escrita no LCD
                        TMR1_StartTimer(); //Liga motor
                        inicio = 61599; //Motor em 180� (v�lvula fechada)
                        opca = 0; //Retorno para indica��o dos valores dos sensores
                    }
                    second = 0; //Vari�vel de tempo em 0
                    break;
                case 2: //Fase 2
                    inicio = 57505; //Motor em 180� (v�lvula totalmente aberta)
                    if (temperatureMin <= temperature && temperature <= temperatureMax && pressureMin <= pressure && pressure <= pressureMax) {

                        LCD("------FASE 2------","    180 GRAUS");
                        TMR1_StartTimer();
                    }
                    if (temperatureMin >= temperature || temperature >= temperatureMax) {
                        LCD("ERRO DE TEMPERATURA","     !!!!!!!               ");
                        TMR1_StartTimer();
                        inicio = 61599;
                        opca = 0;
                    }
                    if (pressureMin >= pressure || pressure >= pressureMax) {
                        LCD("     ERRO DE PRESSAO","     !!!!!!!               ");
                        TMR1_StartTimer();
                        inicio = 61599;
                        opca = 0;
                    }
                    second = 0;
                    break;
                case 3: //Fase 3
                    
                    TMR1_StartTimer(); //Acionamento do motor
                    inicio = inicio + 511; //Parametro de decrementa��o do �ngulo de abertura
                    abertura = 360 * (((float) (61599 - inicio)) / 4094) - 180; //C�lculo angulo de abertura
                    sprintf(a_abertura, "    %.0f GRAUS          ", abertura); 
                    LCD("------FASE 2------",a_abertura); //Escrita LCD
                    second = 0; //Reinicializa��o da vari�vel de tempo
                    break;
            }
        }
        if (EUSART1_is_rx_ready()) { //In�cio da EUSART
            rxData = EUSART1_Read(); //Recebe o que foi escrito no EUSART
            EUSART1_Write(rxData); // Mostra caracter recebido devolvendo-o para EUSART
            caracter_recebido = 1; // Indica caracter recebido disponivel em rxData
        }
        switch (menu) { //Sele��o de menu EUSART
            case 0: //Menu principal
                if (mostra_menu == 1) {
                    printf("\r\n1 - Iniciar Processo de injecao");
                    printf("\r\n2 - Abortar processo de injecao");
                    printf("\r\n3 - Alterar intervalos");
                    printf("\r\nOpcao: ");
                    mostra_menu = 0; //Vari�vel de visualiza��o de menu
                }
                if (caracter_recebido == 1) { 
                    // TODO: verificar que caracter recebido � v�lido
                    // Convers�o do c�digo ASCII para inteiro: rxData - 48
                    menu = (rxData - 48); 
                    mostra_menu = 1;
                    caracter_recebido = 0;
                    printf("\r\n");
                }
                break;
            case 3: //Menu de altera��o de intervalos
                if (mostra_menu == 1) {
                    printf("\r\n1- Alterar intervalo de pressao");
                    printf("\r\n2- Alterar intervalo de temperatura");
                    printf("\r\nX - Recuar");
                    printf("\r\nOpcao: ");
                    mostra_menu = 0;
                }
                if (caracter_recebido == 1) {
                    // TODO: verificar que caracter recebido � v�lido
                    if (!(rxData == 'x') && !(rxData == 'X')) {
                        menu = 10 + (rxData - 48); //Convers�o de ASCII para inteiro
                        mostra_menu = 1;
                        caracter_recebido = 0;
                        printf("\r\n");
                    }
                }
                break;
            case 11: //Menu de intervalo de press�o
                if (mostra_menu == 1) {
                    printf("\r\nIntervalos: ");
                    printf("\r\n1 - [40;60] kPa ");
                    printf("\r\n2 - [60;80] kPa ");
                    printf("\r\n3 - [80;100] kPa ");
                    printf("\r\n4 - [100;120] kPa ");
                    printf("\r\nX - Recuar");
                    printf("\r\nOpcao: ");
                    mostra_menu = 0;
                }
                if (caracter_recebido == 1) {
                    // TODO: verificar que caracter recebido � v�lido
                    if (!(rxData == 'x') && !(rxData == 'X')) {
                        opcaoMenu = (rxData - 48);
                        switch (opcaoMenu) { //Menu backend de altera��o de intervalo de press�o
                            case 1:
                                pressureMin = 40;
                                pressureMax = 60;
                                break;
                            case 2:
                                pressureMin = 60;
                                pressureMax = 80;
                                break;
                            case 3:
                                pressureMin = 80;
                                pressureMax = 100;
                                break;
                            case 4:
                                pressureMin = 100;
                                pressureMax = 120;
                                break;
                        }
                        menu = 0;
                        mostra_menu = 1;
                        caracter_recebido = 0;
                        printf("\r\n");
                    }
                }
                break;
            case 12: 
                if (mostra_menu == 1) { //Menu intervalos de temperatura
                    printf("\r\nIntervalos: ");
                    printf("\r\n1 - [60;80]�C ");
                    printf("\r\n2 - [70;90]�C ");
                    printf("\r\n3 - [80;100]�C ");
                    printf("\r\n4 - [90;120]�C ");
                    printf("\r\nX - Recuar");
                    printf("\r\nOpcao: ");
                    mostra_menu = 0;
                }
                if (caracter_recebido == 1) {
                    // TODO: verificar que caracter recebido � v�lido
                    if (!(rxData == 'x') && !(rxData == 'X')) {
                        opcaoMenu = (rxData - 48);
                        switch (opcaoMenu) { //Menu backend de altera��o de intervalo de temperatura
                            case 1:
                                temperatureMin = 60;
                                temperatureMax = 80;
                                break;
                            case 2:
                                temperatureMin = 70;
                                temperatureMax = 90;
                                break;
                            case 3:
                                temperatureMin = 80;
                                temperatureMax = 100;
                                break;
                            case 4:
                                temperatureMin = 90;
                                temperatureMax = 120;
                                break;
                        }
                        menu = 0;
                        mostra_menu = 1;
                        caracter_recebido = 0;
                        printf("\r\n");
                    }
                    break;
                    case 2: //Interrompe o processo de inje��o
                    INTCON3bits.INT2IF = 1; //Acionamento da flag do bot�o de emerg�ncia
                    if (mostra_menu == 1) {
                        printf("\r\nProcesso de injecao Abortado! "); //Escreve no EUSART
                        mostra_menu = 1;
                        menu=0; //Volta para o Menu principal
                    }
                    break;
                    case 1: //In�cio do processo de inje��o
                    INTCONbits.INT0IF = 1; //Acionamento da flag do botao de in�cio
                    if (mostra_menu == 1) {
                        printf("\r\nProcesso de injecao Iniciado! "); //Escrita no EUSART
                        mostra_menu = 1;
                        menu=0; //Volta para o Menu principal
                    } 
                    INTCONbits.INT0IF = 0; //Flag de interrup��o do bot�o inicio a 0
                    break;
                    default: //Limita��o de escrita EUSART ao menu
                    printf("\r\nOpcao errada\r\n"); //Escrita no EUSART
                    menu = 0; //Volta ao menu principal
                    mostra_menu = 1;
                    break;
                }
        }
    }
    /**
     End of File
     * 
     * 
     *      */
}
