
// Define message groups to be supported (Astrid.h)
#include <AstridAddrSpace.h>
#define NODE_ADDR MH1_RELAY
//#define NODE_ADDR MH2_RELAY
#define MENU_DATA
#define MH2_RELAY   //MH1_RELAY, MH2_RELAY, TK_RELAY   defined in Astrid.8
#include <Arduino.h>
#include <Wire.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <Astrid.h>
#include <VillaAstridCommon.h>
#include <SimpleTimer.h> 
#include <SmartLoop.h>

//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NETWORKID    100  //100  //the same on all nodes that talk to each other
#define NODEID        1  

//Match frequency to the hardware version of the radio on your Feather
#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
//#define FREQUENCY      RF69_915MHZ
#define ENCRYPTKEY     "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module

//*********************************************************************************************
#define SERIAL_BAUD   9600

#define RFM69_CS      10
#define RFM69_IRQ     2
#define RFM69_IRQN    0  // Pin 2 is IRQ 0!
#define RFM69_RST     9

#define LED           13  // onboard blinky

unit_type_entry Me ={"MH1T1","Terminal","T164","T164","T164","17v01",'0'}; //len = 9,5,5,5,9,byte
time_type MyTime = {2017, 1,30,12,05,30}; 
int16_t packetnum = 0;  // packet counter, we increment per xmission
boolean msgReady; 
boolean SerialFlag;

RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);
SmartLoop Smart = SmartLoop(1);
SimpleTimer timer;

void setup() {
    while (!Serial); // wait until serial console is open, remove if not tethered to computer
    Smart.begin(9600);
    //Serial.begin(SERIAL_BAUD);
    InitSoftCom();
    Serial.println("Feather RFM69HCW Receiver");
  
    // Hard Reset the RFM module
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, HIGH);
    delay(100);
    digitalWrite(RFM69_RST, LOW);
    delay(100);
  
    // Initialize radio
    radio.initialize(FREQUENCY,NODE_ADDR,NETWORKID);
    if (IS_RFM69HCW) {
       radio.setHighPower();    // Only for RFM69HCW & HW!
    }
    radio.setPowerLevel(5); // power output ranges from 0 (5dBm) to 31 (20dBm)
  
    radio.encrypt(ENCRYPTKEY);
  
    pinMode(LED, OUTPUT);

    Serial.print("\nListening at ");
    Serial.print(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(" MHz");
    timer.setInterval(10, run_10ms);
    timer.setInterval(1000, run_1000ms);

}

void loop() {
    char rx_radio_packet[20] = "";
    byte i;
    //while(1) {
    //   SendSoftCom("A5A5");
    //}
    
    //check if something was received (could be an interrupt from the radio)
    if (radio.receiveDone()) {
       //print message received to serial
       //Serial.print('[');Serial.print(radio.SENDERID);Serial.print("] ");
       //Serial.print((char*)radio.DATA);
       //Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
       for (i=0; (char*)radio.DATA[i]; i++) rx_radio_packet[i] = radio.DATA[i];
       Serial.println(rx_radio_packet);
       if (Smart.insert_radio_msg(rx_radio_packet)){ GetMsg(); }
       //check if sender wanted an ACK
       if (radio.ACKRequested()) {
           radio.sendACK();
           Serial.println(" - ACK sent");
       }
       //Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks 
    }

    radio.receiveDone(); //put radio in RX mode
    Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
    //SendSoftCom("A5A5A5A5");
}


void Blink(byte PIN, byte DELAY_MS, byte loops){
    for (byte i=0; i<loops; i++) {
        digitalWrite(PIN,HIGH);
        delay(DELAY_MS);
        digitalWrite(PIN,LOW);
        delay(DELAY_MS);
    }
}
void run_10ms(void){
   Smart.HeartBeat10ms();
   if( Smart.Monitor()) msgReady = true;
}

void run_1000ms(){
   
   if (++MyTime.second > 59 ){
         Smart.Send(SM_BROADCAST,"TK_R1","4=1");

      MyTime.second = 0;
      if (++MyTime.minute > 59 ){
         MyTime.minute = 0;
         if (++MyTime.hour > 23){
            MyTime.hour = 0;
         }
      }   
   }
}
