/*!
 * \file   ADC.principal
 * \date   2024-05-03
 * \author Adrian Sánchez-Kevin Vernaza-Camilo Castro 
 * \brief Configuraciones del módulo ADC y lecturas de sensores.
 *
 * Este archivo contiene la configuración del microcontrolador y las funciones para leer 
 * varios sensores analógicos, incluidos el sensor de temperatura, sensor de luz, micrófono
 * y potenciómetro.
 */




#include <pic16f887.h>
#include "config.h"
#include <math.h>
#include "LCD.h"
#include <stdio.h>
#include <xc.h>
//#include <string.h>

void leer_temperatura(void);
void leer_luz(void);
void leer_micro(void);
void leer_potenciometro(void);
void comparacion(void);

#define LIGHT_MIN 0
#define LIGHT_MAX 1023
#define beta 4090




float temp = 0.0;
int pot = 0;
int luz = 0;
int mic = 0;

float tempC;
float A= 0.001129148, B=0.000234125, C=0.0000000076741, Var1= 0.04058;
void main(void) {
    OSCCON = 0x71; // reloj interno de 8M - Configura el oscilador
    
    TRISA0 = 1; // Configura RA0 como entrada    
    TRISE1 = 1; // Configura E1 como entrada
    TRISB1 = 1; // Configura RB1 como entrada
    TRISB5 = 1; // Configura RB5 como entrada
    
    TRISA3 = 0;
    TRISA4 = 0;
    TRISA5 = 0;
    
   
    
    ANSEL = 0x41; // Configura el Puerto como Entrada Analógica.
    ANSELH = 0x24; 

    
    
    ADCON1bits.ADFM = 0; // Justificación Izquierda (modo-8bits).
    ADCON1bits.VCFG0 = 0; // Selecciona Voltajes de Referencia (5v-0v).
    ADCON1bits.VCFG1 = 0; // Selecciona Voltajes de Referencia (5v-0v). 
    ADCON0bits.ADCS = 0b01; // Tiempo de Conversión Fosc/8.
    
    
    LCD_Init(); //Inicializa el LCD
    LCD_String_xy(0,0,"Bievenido");
    __delay_ms(2000);
     
  /**
 *@brief Ciclo infinito donde se llaman las funciones y se hace la
   * conversión a cadena de texto
 */   
   
    while (1) {
        leer_temperatura();
        leer_luz();
        leer_micro();
        leer_potenciometro();
        comparacion();
        
        char buferT[7];
        sprintf(buferT, "T:%.2f", tempC);//convertir a cadena de texto
        
        char buferL[7];
        sprintf(buferL, "L:%d", luz);
        
        char buferM[7];
        sprintf(buferM, "M:%d", mic);
        
        char buferP[7];
        sprintf(buferP, "P:%d", pot);
        
        LCD_Clear();
        LCD_String_xy(0,0,buferT);
        LCD_String_xy(0,10,buferL);
        LCD_String_xy(2,0,buferM);
        LCD_String_xy(2,10,buferP);
        __delay_ms(1000);
          
    }
}

    
/*!
 * \brief   Compara la temperatura con un umbral y activa los pines de salida correspondientes.
 * 
 * Si la temperatura es mayor o igual a 23°C, activa el pin RA3.
 * Si la temperatura es menor o igual a 22°C, activa el pin RA4.
 * En otros casos, activa el pin RA5.
 */

void comparacion(void) {
        if (tempC >= 23.0) {
              PORTAbits.RA3 = 1;
              PORTAbits.RA4 = 0; 
              PORTAbits.RA5 = 0; 
        } 
        else if(tempC <= 22.0) {  
              PORTAbits.RA3 = 0;
              PORTAbits.RA4 = 1; 
              PORTAbits.RA5 = 0;      
        }
        else {
              PORTAbits.RA3 = 0;
              PORTAbits.RA4 = 0; 
              PORTAbits.RA5 = 1;
        }
} 
        
    

/*!
 * \brief   Lee el sensor de temperatura y calcula la temperatura en grados Celsius.
 * 
 * Utiliza el canal 0 del ADC para leer la tensión del sensor de temperatura y la convierte
 * en temperatura en grados Celsius utilizando la ecuación de Steinhart-Hart.
 */



void leer_temperatura(void) {
    ADCON0bits.CHS = 0b0000; // Selecciona el Canal Analógico AN0.
    ADCON0bits.ADON = 1; // Habilita el Módulo AD.
   __delay_us(30);
    ADCON0bits.GO = 1; // Inicia la Conversión AD.
    while (ADCON0bits.GO); // Espera a que termine la conversión AD.
        
    unsigned int Vo = ADRESH; // Lee el valor del conversor AD
    Vo= 1023.0 - (Vo*4);
    tempC= Vo*(Var1);
    
}

/*!
 * \brief   Lee el sensor de luz y calcula el porcentaje de luz ambiente.
 * 
 * Utiliza el canal 6 del ADC para leer la tensión del sensor de luz y la convierte
 * en un porcentaje de luz ambiente.
 */


void leer_luz (void){
    ADCON0bits.CHS = 0b0110; // Selecciona el Canal Analógico 
    ADCON0bits.ADON = 1; // Habilita el Módulo AD
    __delay_us(30);
    ADCON0bits.GO = 1; // Inicia la Conversión AD.    
    while (ADCON0bits.GO); // Espera a que termine la conversión AD.
    
    unsigned int luzx = ADRESH * 4; // Lee el valor del conversor AD para el sensor de luz
    float porcentaje = ((float)(luzx - LIGHT_MIN) / (LIGHT_MAX - LIGHT_MIN)) * 100.0;
    // Calcula el porcentaje de luz
    luz = (int)porcentaje;
}

/*!
 * \brief   Lee el sensor de micrófono y obtiene el valor de entrada analógica.
 * 
 * Utiliza el canal 10 del ADC para leer el valor de entrada analógica del sensor de micrófono.
 */


void leer_micro (void){
    
    ADCON0bits.CHS = 0b1010; // Selecciona el Canal Analógico
    ADCON0bits.ADON = 1; // Habilita el Módulo AD.
   __delay_us(30);
    ADCON0bits.GO = 1; // Inicia la Conversión AD
    while (ADCON0bits.GO); // Espera a que termine la conversión AD.
    
    unsigned int microfono = ADRESH;
    ADCON0bits.ADON = 0; // apaga el Módulo AD.
    mic = microfono; 
}



/*!
 * \brief   Lee el valor del potenciómetro y guarda el resultado en la variable global `potenciometro_valor`.
 * 
 * Utiliza el canal 13 del ADC para leer el valor del potenciómetro y guarda el resultado en la variable global `potenciometro_valor`.
 */

void leer_potenciometro (void){
    ADCON0bits.CHS = 0b1101; // Selecciona el Canal Analógico 
    ADCON0bits.ADON = 1; // Habilita el Módulo AD.
   __delay_us(30);
    ADCON0bits.GO = 1; // Inicia la Conversión AD.
    while (ADCON0bits.GO); // Espera a que termine la conversión AD.
    
    unsigned int potenciometro = ADRESH;
    
    ADCON0bits.ADON = 0; // apaga el Módulo AD.
    pot = potenciometro;
}


