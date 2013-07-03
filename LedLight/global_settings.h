//Overall settings
// Author: Vincent Gijsen

/**
*    Packet Format |PACKET_COUNTER/IDENT 1BYTE|NODES ADDRESS 2BYTE|PACKETTYPE 1BYTE|PAYLOAYD rest of bytes|
**/

#ifndef GLOBAL_SETTINGS_H_
#define GLOBAL_SETTINGS_H_


#define REG_PACK_COUNTER 0
#define REG_PACK_ADDRMSB 1
#define REG_PACK_ADDRLSB 2
#define REG_PACK_TYPESET 3
#define REG_PACK_VAL0 4
#define REG_PACK_VAL1 5
#define REG_PACK_VAL2 6


#define LISTEN_ADDRESS   {'M','U','L','T','I'}

#define C1 {'L','1'}
#define C2 {'L','2'}
#define C3 {'L','3'}
#define C4 {'L','4'}


//Radio Settings
#define RADIO_SPEED 1000000
#define RADIO_CHANNEL 125
#define CMD P2_0
#define CSN P2_1
#define IRQ P2_2


//Packet Types
#define PACKET_TEMP 'T'
#define PACKET_BATT 'B'
#define PACKET_BRIGHTNESS 'd'
#define PACKET_RGB 'r'
#define PACKET_SWITCH 's'
#define PACKET_PING 'p'

//Sampling setting (temperature)
#define SAMPLES 4


//Note, pin 1_0 and 2_3 cannot be used for pwm
#define LED_DRIVER P1_2
#define LED_DRIVER_R P2_3
#define LED_DRIVER_G P2_4
#define LED_DRIVER_B P2_5

//Definitions for Sleep and fading times
#define BLINK_DELAY 5
#define POWERNAP 2
#define TOPCOUNTER 500
#define FADEUPDATE 10



#if SIMPLETYPE
  #define LEDLENGTH 1
  #pragma message  ("Compiling for board: SIMPLE")
#else
  #define LEDLENGTH 3
  #pragma message  ("Compiling for board: RGB")
#endif

#endif /* SETTINGS */

#if defined(__MSP430G2452__) || defined(__MSP430G2553__) || defined(__MSP430G2231__) 
#pragma message  ("Board supported, CPU")
#else
#error Board not supported
#endif

