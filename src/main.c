
#include <stdio.h>
#include <stdlib.h>
#include <stm32f4xx_hal.h>
#include "send_receive.h"
#include "oled_25664.h"
#include <stdbool.h>
#include <stm32f429xx.h>
#include <gui_comm.h>
#include "measurement_logger.h"

// ----- main() ---------------------------------------------------------------


///**
// * @brief This function handles TIM1 Update interrupt and TIM10 global interrupt.
// */
//void TIM1_UP_TIM10_IRQHandler(void) {
//	// DO NOT REMOVE
//	//clear interrupt
//	TIM1->SR &= ~(TIM_IT_UPDATE);
//}
const int Lookup_Tabelle[17] = {0, 1029, 2048, 3048, 4018, 4950, 5833, 6661, 7425, 8117, 8730, 9260, 9701, 10048, 10298, 10449, 10500};
uint16_t angle = 0;
int32_t f_UEV = 8000; // Timer update event frequency 8kHz
int32_t desired_frequency = 25; // Desired frequency
int32_t alte_richtung=0;
int32_t scaler_factor = 0x10000;

int16_t old_position, new_position; // Drehposition
int16_t w_ufilt[2];
int16_t w_filt[2];
float tau_sw=0.008f;
float tau_s=0.000125f; // 1/f_UEV
float error[2];
float u[2];
int antiFlag;
float K_p = 0.8, K_i = 16;
int16_t w_ref;
int32_t measurements[3];


void led_blinken() {
 while(1) {
  GPIOG->BSRR = GPIO_BSRR_BS_2;
  HAL_Delay(500);
  GPIOG->BSRR = GPIO_BSRR_BR_2;
  HAL_Delay(500);
 }
}


uint32_t receiveNumber(USART_TypeDef* const USARTx, const char* message) {
    char buffer[10]= {0};
    int i = 0;
    sendUARTString(USART2, message);

    while(1) {
    char c = receiveUARTSync(USARTx);
        if (c >= '0' && c <= '9') {
            buffer[i++] = c;
        }
        else if (i > 0) {
            break;
        }
    }
return atoi(buffer);
}

void sendResult(USART_TypeDef* const USARTx, const uint32_t result) {
    char buffer[20];
    sprintf(buffer,"Ergebnis %lu\n", result);
    sendUARTString(USARTx, buffer);
}

void addition(){
    while (1) {
        uint32_t num1 = receiveNumber(USART2, "Geben Sie eine Zahl\n");
        uint32_t num2 = receiveNumber(USART2, "Geben Sie eine andere Zahl\n");
        uint32_t result = num1 + num2;

        sendResult(USART2, result);
        oled_25664_init();
        char result_string[10] = {0};
        sprintf(result_string, "%lu", result);
        oled_25664_ShowString(result_string, 10, 10);// Show on display

        sendUARTString(USART2,"Mochten Sie weitermachen(Y/N)\n");
        char c=receiveUARTSync(USART2);
        if(c=='N'||c=='n')break;
        sendUARTString(USART2,"\n");
    }
}

void oled_25664_SetupConnection() {
    HAL_Delay(10);
    GPIOD->BSRR = GPIO_BSRR_BS_10; // high active
    GPIOF->BSRR = GPIO_BSRR_BR_6; // low active
    HAL_Delay(10);
}

void oled_25664_Command(unsigned char Data) {
    sendSPIAsync(SPI5, (uint16_t)(Data & ~(1<<8)));
}

void oled_25664_Data(unsigned char Data) {
    sendSPIAsync(SPI5, (uint16_t)(Data | (1 <<8)));
}

float convertRawToVoltage(uint16_t raw_value) {
    return (raw_value / 4095.0) *5.0 *1001.0;
}

void displayVoltage(uint16_t voltage) {
    char buffer[10];
    sprintf(buffer, "%-3i V", voltage);
    oled_25664_ShowString(buffer, 10, 10);
}

void displayDrehposition(uint16_t angle) {
    char buffer[10];
    sprintf(buffer, "angle: %-3i ", angle);
    oled_25664_ShowString(buffer, 10, 0);
}

void displayVoltageDrehposition(uint16_t voltage, uint16_t angle) {
    char buffer[20];
    sprintf(buffer, "%-3i V, Angle: %i °", voltage, angle);
    oled_25664_ShowString(buffer, 0, 0);
}

void sendVoltageUART(uint16_t voltage) {
    char buffer[10];
    sprintf(buffer, "%-3i V\n", voltage);
    sendUARTString(USART2, buffer);
}

void read_voltage() {

    GPIOD->BSRR = GPIO_BSRR_BR_9;
    uint16_t raw_value = receiveSPISync(SPI3);
    GPIOD->BSRR = GPIO_BSRR_BS_9;

    uint16_t voltage_zwischenkreis = convertRawToVoltage(raw_value);
    displayVoltage(voltage_zwischenkreis);
    sendVoltageUART(voltage_zwischenkreis);

}

bool checkAngle(uint16_t raw_value_drehpoition) {
    // if(raw_value_drehpoition & (1<<13) || raw_value_drehpoition & (1<<14)) {
    if (raw_value_drehpoition > 8191){

        return false;
    } else {
        return true;
    }
}

float convertRawToDegree(uint16_t raw_value_drehpoition) {
    return (raw_value_drehpoition / 8191.0) * 360;
}


void read_voltage_und_drehpositon() {

    GPIOD->BSRR |= GPIO_BSRR_BR_9;
    uint16_t raw_value_spannung = receiveSPISync(SPI3);
    GPIOD->BSRR |= GPIO_BSRR_BS_9;
    float voltage_zwischenkreis = convertRawToVoltage(raw_value_spannung);
    displayVoltage(voltage_zwischenkreis);

    GPIOC->BSRR |= GPIO_BSRR_BR_13;
    uint16_t raw_value_drehpoition = receiveSPISync(SPI3);
    GPIOC->BSRR |= GPIO_BSRR_BS_13;

    float angle = 0.0;
    if(checkAngle(raw_value_drehpoition)) {
        angle = convertRawToDegree(raw_value_drehpoition);
    }
    displayDrehposition(angle);

}

void auslesen_spannung() {
    oled_25664_init();
    while(1) {
        // read_voltage();
        read_voltage_und_drehpositon();
    }
}

void setFrequency(int32_t new_frequency) {
    if (new_frequency > 50) new_frequency = 50;
    if (new_frequency < -50) new_frequency = -50;

    if ((new_frequency > 0 && alte_richtung < 0) || (new_frequency < 0 && alte_richtung > 0)) {
        // Ensure the frequency passes through 0 Hz when switching direction
        desired_frequency = 0;
        scaler_factor = 0;

    }else {
        if(abs(new_frequency) <= 25) {
            scaler_factor = 0x10000 * abs(new_frequency) /25;
            desired_frequency = new_frequency;
        }else {
            scaler_factor = 0x10000;
            desired_frequency = new_frequency;
        }
    }
    alte_richtung = new_frequency; // Update
}

void updateParameters(int32_t parameters[8]) {
    int Timescale, Drehzahl, kI, kP;;
    Timescale = parameters[0];
    Drehzahl = parameters[1];
    kI = parameters[2];
    kP = parameters[3];
    u[1]=Drehzahl;
    w_ref = Drehzahl;

    // Update frequency based on user Input
    setFrequency(Drehzahl);

    // Send values via UART
    char buffer[100];
    sprintf(buffer, "Timescale: %i, Drehzahl: %i, kI: %i, KP: %i\n", Timescale, Drehzahl, kI, kP);
    sendUARTString(USART2, buffer);

}

uint32_t interpolateSine(uint16_t angle, uint32_t scale) {

    // Determine the quadrant based on the angle
    int quadrant = (angle >> 14) & 0b11;

    // Adjust the angle if necessary. subtracting it from 0xFFFF in the second and fourth quadrants
    if (quadrant == 1 || quadrant == 3) {
        angle = 0xFFFF - angle;
    }

    // Calculate the index and fractional part
    int index = (angle >> 10) & 0b1111;
    int fraction = angle & 0b1111111111;

    // Interpolate the sine value using the lookup table
    int sinValue = Lookup_Tabelle[index] + (fraction * (Lookup_Tabelle[index + 1] - Lookup_Tabelle[index])) / 1024; // I should have shifted by >> 10 instead of deviding by  /1024

    // Scale the sine value and adjust based on the quadrant
    uint32_t result = 10500;
    if (quadrant == 0 || quadrant == 1) {
        result += (sinValue * scale) / 0x10000; // I should have shifed by >> 16 instead of deviding by /0x10000
    } else {
        result -= (sinValue * scale) / 0x10000;
    }

    return result;
}

void tiefpassfilter(){
    w_filt[0]=1/(1+2*(tau_sw/tau_s))*(w_ufilt[0]+w_ufilt[1]) - (1-2*(tau_sw/tau_s))/(1+2*(tau_sw/tau_s))*w_filt[1];
    w_filt[1]=w_filt[0];
}

void berechnungIstDrehzahl(){

    GPIOC->BSRR = GPIO_BSRR_BR_13;  //Drehgeber anschalten
    new_position = receiveSPISync(SPI3);
    GPIOC->BSRR = GPIO_BSRR_BS_13;  //Drehgeber ausschalten
    // 1. Get speed form position sample

    int16_t speed =new_position-old_position;
    // 2. Eliminate verflow
    if(speed >=4096){ // Uberlaufen in negativer Drehrichtung erkennen
        speed = speed -8192;
    }
    else if(speed <=-4096){ // Uberlaufen in positiver Drehrichtung erkennen
        speed = speed +8192;
    }
    // Skipped! 3. Eliminate missreadings

    w_ufilt[0]= (float) ((float)speed *60.0)/(8192.0 * (1.0/(float)f_UEV)); // Sampling time Ts 1/f_UEV
    old_position=new_position;
    w_ufilt[1]=w_ufilt[0];
}

void Regler_mit_Anti_Windup(){
    // Determine error
    error[0] = w_ref - w_filt[0]/60; // Regler must be in Hz

    float k_p_teil = K_p * (error[0] - error[1]);
    float k_i_teil = tau_s * K_i * 0.5 *(error[0] + error[1]);

    float stellGrosse = u[1] + k_p_teil + k_i_teil;

    // Anti-Windup
    if (k_i_teil > 35 || k_i_teil < -35) {
            u[0] = u[1] + k_p_teil;
    }else {
        u[0] = u[1] + k_p_teil + k_i_teil;
    }
    // In case dsireded_freuqunce is set to 0
    if(w_ref == 0) {
        u[0] = 0;
        error[0] = 0;
    }
    // Stellgrossen Beschrankung
    if(u[0] >70) {
        u[0] = 70;
    }else if(u[0] < -70) {
        u[0] = -70;
    }

    // Update parameters
    desired_frequency = u[0]; // Update
    error[1] = error[0]; // Update
    u[1] = u[0]; // Update

    if(abs(desired_frequency) <= 25){
        scaler_factor = 0x10000 * abs(desired_frequency) / 25;
    } else {
        scaler_factor=0x10000;
    }
}

void TIM1_UP_TIM10_IRQHandler() {
    // static uint16_t angle = 0;
    uint32_t sinValue1 = interpolateSine(angle, scaler_factor);
    uint32_t sinValue2 = interpolateSine((angle + 0x5555) & 0xFFFF, scaler_factor);
    uint32_t sinValue3 = interpolateSine((angle + 0xAAAA) & 0xFFFF, scaler_factor);

    // Aktualisiere die CCRx-Register mit den Sinuswerten
    TIM1->CCR1 = sinValue1;
    TIM1->CCR2 = sinValue2;
    TIM1->CCR3 = sinValue3;

    // Determine Ist-Drehzahl
    berechnungIstDrehzahl();
    tiefpassfilter();
    Regler_mit_Anti_Windup();
    measurements[0] = w_filt[0]; // Ist-Drehzahl
    measurements[1] = w_ref*60; //Soll-Drehzahl
    measurements[2] = w_filt[0]; // Sprungantwort
    logMeasurements(3, measurements);

    // // Compare filtered with unfiltered Drehzahl
    // int32_t measurements[2] = {w_ufilt[0], w_filt[0]};
    // logMeasurements(2, measurements);

    // // Schreibe die Sinuswerte in die Puffer für die GUI
    // int32_t measurements2[3] = {sinValue1, sinValue2, sinValue3};
    // logMeasurements(3, measurements2);

    angle += (0xFFFF * desired_frequency) / f_UEV; // Inkrementiere den Winkel 

    TIM1->SR &= ~(TIM_IT_UPDATE);
}

void messdaten_vom_STM32_in_GUI() {
    int32_t measurment[1];
    int value =0;
    while (1) {
        measurment[0] = value;
        logMeasurements(1, measurment);
        value = (value + 1) % 1200;
    }
}
int main(int argc, char *argv[]) {
    //oled_25664_init();
    //oled_25664_ShowString("10",10,10);
    // addition();
    //auslesen_spannung();
    //messdaten_vom_STM32_in_GUI();
    while(1) {

    }

}


