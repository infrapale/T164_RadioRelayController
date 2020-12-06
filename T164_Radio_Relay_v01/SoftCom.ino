//<#IO41x>, x=0|1
#include <SoftwareSerial.h>
#define rxPin 8
#define txPin 7
#define SCOM_BUFF_LEN 8
#define UNIT_ADDR_1 'I'
#define UNIT_ADDR_2 'O'
#define UNIT_ADDR_3 '1'





SoftwareSerial softCom(rxPin, txPin); // RX, TX

byte softComState;
char softComBuff[SCOM_BUFF_LEN];
short int softComBuffIndx =0;
short int i;
boolean softComFlag;

boolean SoftComFlag(void){
  return softComFlag;
}

void InitSoftCom (void){
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  softCom.begin(9600);
  softComState = 0;
  softComBuffIndx =0;
  for (i=0;i<SCOM_BUFF_LEN;i++){
    softComBuff[i]= '0'+i;
  }
  softCom.listen();
  softComFlag = false;
}

void SendSoftCom(char *b){
  //delay(1000);
  Serial.println(b);
  softCom.print('<');
  softCom.print(b);
  softCom.println(">");
 
}

 void DebugSoftCom(void){
 
 if (softCom.available()>0){
     char c;
     c = softCom.read();
     Serial.print(c);
  }
 }
void SoftComMonitor(void) {
  char c;
  //softCom.print("X");
  softCom.listen();
  

  if (softCom.available()>0){
     c = softCom.read();
     switch(softComState){
     case 0:
       if (c=='<') {
         softComState = 1;
       }
       break;
     case 1:
       if (c=='>') {
         softComState = 2;
       } else {
         softComBuff[softComBuffIndx] = c;
         softComBuffIndx++;
         if (softComBuffIndx >= SCOM_BUFF_LEN) {
            InitSoftCom;
         }
       }     
       break;
     case 2:
       if (c=='\r') {
          if (softComBuff[0] =='#'){
             if ((softComBuff[1] == UNIT_ADDR_1 ) && (softComBuff[2] == UNIT_ADDR_2 ) && 
                 (softComBuff[3] == UNIT_ADDR_3 ) && (softComBuff[4] == 'I' )) {
                if ((softComBuff[5] >='1') && (softComBuff[5] <='4'))  {
                   if ((softComBuff[6] =='0') || (softComBuff[6] =='1'))  {
                      i = softComBuff[5]-'1' ; 
                      //if ( softComBuff[6] =='1') inp[i]=HIGH; else inp[i]=LOW; 
                      softComFlag = true;
                      
                   }
                }   
             }
          
          }
       
       }     
       softComState = 0;
            
       break;
     
       
     }
  
  }    
      

}

