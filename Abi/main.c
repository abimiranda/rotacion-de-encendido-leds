/**************************
 * File:   main.c                                                           *
 * Author: Abigail Miranda                                                    *
 **************************/


// PARPADEO DEL LED //
#include <xc.h>
#include "confbits.h"       //no se debe de modificar este archivo para
//garantizar el funcionamiento del BOOTLOADER

#include "Tecnica1.h"       //segun el shield habilita distintas librerias

#include "ConfShield2_1.h" //cambiar para el teclado // esta viene x defecto
//el teclado utiliza los mismos pines que el entrenador pero la
//incializaci´on esta disponible en el Shield1.3 no en el 2.1.
#include "ConfShield1_3.h"
#include "lcd.h"
#include "teclado.h"
#include "EncoderIncremental.h"

#define LED1  LATDbits.LD1
#define LED2  LATDbits.LD0

long int tiempo_ventana_peso=9, activador_tiempo_ventana_peso=0; //variables definida en segundos.
long int tiempo_parpadeo_on=1, activador_on=0; //tiempo de encendido=1seg. activador es el flag
long int tiempo_parpadeo_off=1, activador_off=0;
long int casos_parpadeo=0; //switch en base a parpadeo del led. Comienza en caso 0


void parpadeoLed(void); 
void main(void) {

    PicIni21();

    TRISDbits.RD1 = 0;
    TRISDbits.RD0 = 0;
    
    TIMER0_INI(); //llamo a la funcion tmimer 0
    TMR0ON = 1; //lo prendo
    TIMER1_INI();
    TMR1ON = 1;
    ei(); //habilito las interrupciones
    tecladoIni13(); //inicializo teclado y conexiones etc

    LCD_init(); //inicializo el lcd;   



    while (1) {
        
//        if(tecla()==1){
            parpadeoLed(); //llamo al a funcion para realizar el parpadeo
//        }
    }
}

void __interrupt myISR(void) {
    
    //TIMMER 0 PARA SEGUNDOS
    if (TMR0IF == 1) { //termino termino de contar el timer?
        TMR0IF = 0; //bajamos el flag
        TMR0L = 0xD2; //el timer contará 47 fosc/4 * 256 = 12032 * 0,0833us
        TMR0H = 0x48; //en total aprox 1.0022ms  casi 1ms
          //    des =~ des;
 
         if(activador_tiempo_ventana_peso==1){ //si el flag (activador) se activó, comienzo a decrementar el tiempo
            if(tiempo_ventana_peso!=0){
                tiempo_ventana_peso--;
            }
        }
        
         if(activador_on == 1){                //si el flag (activador) se activó, comienzo a decrementar el tiempo
            if(tiempo_parpadeo_on != 0){
                tiempo_parpadeo_on--;
            }
        }
        
         if(activador_off==1){                 //si el flag (activador) se activó, comienzo a decrementar el tiempo
            if(tiempo_parpadeo_off !=0){
                tiempo_parpadeo_off--;
            }
        }
        
 
        
    }

    if (TMR1IF == 1) { //TIMER 1 PARA ms
        TMR1IF = 0; //bajamos el flag
        TMR1L = 0x1B; //el timer contará 1200 fosc/4 * 1 = 12032 * 0,0833us
        TMR1H = 0xD1; //en total aprox 99.96us  casi 100us
        tic_teclado();
        tic_LCD();
        if (RBIF == 1) { //si RBIF es 1
            marca_tecla(); //llamo a la funcion marca_tecla
        }
        RBIF = 0;
        
        
    }

}



void parpadeoLed() {//declaro la función parpadeoLed

    if (tiempo_ventana_peso != 0) { // si el tiempo no llegó a 0
        activador_tiempo_ventana_peso = 1; // activo el flag para que ahi comience el temporizador a decrementar
        switch (casos_parpadeo) { //switch respecto a casos parpadeo
            case 0://primer caso
            {
                if (tiempo_parpadeo_on != 0) { // si el tiempo de encendido del LED1 no llegó a 0

                    activador_on = 1; // activo el flag para qu recien ahi comience a contar el temporizador de ese led
                    LED1 = 1; // activo el led
                    set_CURSOR(0x01); //escribo en el lcd
                    msg2LCD("LED1 ENCENDIDO");
                    
                }
                if (tiempo_parpadeo_on == 0) { // si llegó a 0
                    activador_on = 0; // pongo el flag en 0 para que deje de contar
                    clear_LCD(); // limpio pantalla lcd
                    casos_parpadeo = 1; // paso al siguiente caso
                }
                }
                break; 
            case 1://segundo caso
            {
                if (tiempo_parpadeo_on == 0) { // si el contador llego a 0
                    LED1 = 0; // apago el lec
                    clear_LCD(); //limpio pantalla
                    tiempo_parpadeo_on = 3; // cargo el contador a 1SEG paqra la proxima vez
                    casos_parpadeo=2; //paso al sig caso
                }
            }
                break;

            case 2:
            {
                if(tiempo_parpadeo_off!=0){ // si el temporizador del segundo led  no llego a 0
                    activador_off=1; // activo el contador de este led
                    LED2=1; // enciendo el led
                    set_CURSOR(0x01); //escribo en el lcd
                    msg2LCD("LED2 ENCENDIDO");
                  
                }
                if(tiempo_parpadeo_off==0){ // si el tiempo llegó a 0
                    activador_off=0; //apago el flag
                    casos_parpadeo=3; //paso al sig caso
                    clear_LCD(); // limpio pantalla
                }
            }
            break;

            case 3:
            {
                if (tiempo_parpadeo_off == 0) { // si el tiempo llegó a 0
                    LED2 = 0; // apago el led
                    clear_LCD(); //limpio pantalla
                    tiempo_parpadeo_off = 3; //cargo para la proxiima vez
                    casos_parpadeo=0; //vuelvo al primer caso

                }
            }
        }
    }
    if(tiempo_ventana_peso==0){ // si el contador principal de 5 seg llegó a 0
        activador_tiempo_ventana_peso=0; //entonces apago el flag para que no entre a la maquina de estados
        LED1=0; //apago los leds
        LED2=0;
        set_CURSOR(0x01); //escribo en el lcd
        msg2LCD(" LEDS APAGADOS ");
        
    }
}
