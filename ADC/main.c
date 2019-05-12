#include <stdio.h>
#include <string.h>
#include "UartTimer.h"

#define HAL_ADC_REF_1V25                0x00 //00: Internal reference -- 1.25V or 10: AVDD5 3.3V in this board
#define ADC_12_BIT                      0x30 //12-bit
#define ADC_EMP_SENS                    0x0b //AIN6-AIN7, 0x07: Pin7, 0x0e: Temperature Sensor, 0x0f: VDD/3

// Initialize the temperature sensor //
//***********************************//
void InitSensor(void)
{ 
   DISABLE_ALL_INTERRUPTS();     // Disable all interruptsS
   InitClock();                  // Set the system clock as 32MHz
   TR0=0x01;                     // Set 1 to connect the temperature sensor  
   ATEST=0x01;                   // Open temperature sensor
}   

//     Initialize the AD8232      //
// Differential Input: AIN6 & AIN7//
//********************************//
void InitAD8232(void)
{
    P0SEL &= ~0xc0;
    P0DIR &= ~0xc0;
}

// Get temperature //
//*****************//
float GetTemperature(void)
{ 
   unsigned int  value; 
   unsigned char tmpADCCON3 = ADCCON3;
   
   ADCIF = 0;   
   
   ADCCON3  = (HAL_ADC_REF_1V25 | ADC_12_BIT | ADC_EMP_SENS);        // Set ADC register    
   
   while(!ADCIF);      // Wait for the finish of ADC processing
   
   value = ADCL;
   value |= ADCH<<8;
   value >>= 4;

   ADCCON3 = tmpADCCON3;

   return value;                               
}

//*****************************//
//*****************************//
void main(void) 
{   
    char i; 
    float AvgTemp;   
    char strTemp[6];
    
    InitUART();                        
    InitSensor();    
    InitAD8232();
    
    while(1) 
    { 
        AvgTemp = 0;          
        for (i=0; i<128; i++) 
        {    
            AvgTemp += GetTemperature();              
        }
        
        AvgTemp = AvgTemp / 128;     
        AvgTemp = AvgTemp / 2047;
       
        memset(strTemp, 0, 6);
        sprintf(strTemp,"%.03f", AvgTemp);
        UartSendString(strTemp, 6);  
        UartSendString("\n",1);         
        //DelayMS(200);                   
    } 
}