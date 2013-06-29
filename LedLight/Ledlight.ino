#include "global_settings.h"

#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>
#include <WS2811Driver.h>



#if defined(__MSP430G2452__) || defined(__MSP430G2553__) || defined(__MSP430G2231__) 
#else
#error Board not supported
#endif

#define CMD P2_0
#define CSN P2_1
#define IRQ P2_2


#define SIMPLETYPE true
//Note, pin 1_0 and 2_3 cannot be used for pwm
#define LED_DRIVER P1_2
#define BLINK_DELAY 5
#define POWERNAP 10
#define TOPCOUNTER 500

//ENABLE SERIAL AND PRINT DEBUGGIN
#define VERBOSE 0

Enrf24 radio(CMD, CSN, IRQ);

const uint8_t rxaddr[] = LED_CONTROLLER1;
const uint8_t txaddr[] = SERVER_ADDRESS;


uint8_t i = 0;
uint32_t average = 0;
uint32_t values[SAMPLES];

uint16_t loopCounter=0;

/*
 *  Prototypes
 */
 
void dump_radio_status_to_serialport(uint8_t);

/*
 *   Mandatory logic
 */
 
 
void setup() {
#if VERBOSE > 0
  //SERIAL SETTINGS
  Serial.begin(9600);
  Serial.println("Beginning setup");
#endif
  //ANALOG SETTINGS
  analogReference(INTERNAL1V5);
  analogRead(TEMPSENSOR); // first reading usually wrong

  //SPI SETTINGS
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(1); // MSB-first
  
  //RADIO SETTINGS
  radio.begin(RADIO_SPEED, RADIO_CHANNEL);  // Defaults 1Mbps, channel 0, max TX power
  radio.setRXaddress((void*)rxaddr);
  radio.setTXaddress((void*)txaddr);  
#if VERBOSE > 0
  dump_radio_status_to_serialport(radio.radioState());
#endif
  radio.deepsleep();
  
  //LEDS Settings 
  pinMode(RED_LED, OUTPUT);
  pinMode(LED_DRIVER, OUTPUT);
  analogWrite(LED_DRIVER, 15);

  #ifdef RGBTYPE  
      //DIGITAL LED(S) Settings
      pinMode(P2_3, OUTPUT);
      ledStrip.begin();
      ledStrip.setPinMask(P2_3);
      ledStrip.setLEDCount(1);
      ledStrip.write(initColor, 3);
  #endif
  delay(1000);
  
  //Blink redled to indicate 
  blinkRed();
  pingServer();
}

void blinkRed()
{
  digitalWrite(RED_LED, HIGH);
  delay(BLINK_DELAY);
  digitalWrite(RED_LED, LOW);
  delay(BLINK_DELAY);
}


void loop() {
   char inbuf[32];
  //Go listen
  radio.enableRX();  // Start listening
  
  if(radio.available(true))
   {      
   if (radio.read(inbuf)) {
    #ifdef SIMPLETYPE
         //valid type of package
         if (inbuf[0] == PACKET_BRIGHTNESS){
           analogWrite(LED_DRIVER, inbuf[1]);
           blinkRed();
         }
    #endif 
    
     #ifdef RGBTYPE
         // 3 bytes, they represent R,G,B
         //controllers take led as rbg :S
         unsigned char r,g,b,led[3];
         r=inbuf[0];
         g=inbuf[1];
         b=inbuf[2];
         led[0]=r;
         led[1]=b;
         led[2]=g;
         ledStrip.write(led, 3);
     #endif
    }
   }
  /**
  *
  * Blink periodically and ping server
  *
  */
  
  loopCounter++;
  
  if(loopCounter == TOPCOUNTER){
    //reset counter
    loopCounter = 0;
    blinkRed();
    blinkRed();
    blinkRed();
    blinkRed();
   
   //send 'keep-alive' 
     pingServer();
     
 
  }
  
  
  delay(POWERNAP);  
}

void printPackage(char p[], int length){
  uint8_t it = 0;
  Serial.println("\npackage contence: ");

  for (it=0; it< length; it++){
    Serial.print(p[it], HEX);
    Serial.print("-");
  }

 }
  
uint32_t readTemperature(){
   // Formula: http://www.43oh.com/forum/viewtopic.php?p=18248#p18248
  for (i=0;i<SAMPLES;i++)
  {
    average -= values[i];
    values[i] = ((uint32_t)analogRead(TEMPSENSOR)*27069 - 18169625) *10 >> 16;
    average += values[i];
  } 
  return (average/SAMPLES);
}

uint16_t readBattery(){
 uint16_t adc = analogRead(A1); 
 uint32_t volt = (adc >> 10) * 1500;
 return volt; 
}

#if VERBOSE > 0

void printDec(uint32_t ui) {
  Serial.print(ui/10, DEC);
  Serial.print(".");
  Serial.print(ui%10, DEC);
}

void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
  
}

#endif


void pingServer(){
   //send a package with Temperature
  char packetBuffer[8];
  packetBuffer[0] = rxaddr[0];
  packetBuffer[1] = rxaddr[1];
  packetBuffer[2] = rxaddr[2];
  packetBuffer[3] = rxaddr[3];
  packetBuffer[4] = rxaddr[4];
  packetBuffer[5] = PACKET_MYADDR;
  radio.print(packetBuffer);
  radio.flush();
}
