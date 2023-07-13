#include <IRremote.h>
#define RE_pin 9
#define LED_BPIN 6
#define LED_RPIN 5
#define LED_GPIN 10
IRrecv irrecv(RE_pin);
decode_results results;
unsigned long key_value = 0;
int B_L = 0;
int R_L = 0;
int G_L = 0;
bool POWER_ON = true;
bool R_ON = true ;
bool G_ON = true ;
bool B_ON = true ; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(LED_BPIN,OUTPUT);
  pinMode(LED_GPIN,OUTPUT);
  pinMode(LED_RPIN,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (POWER_ON == true && R_ON == true){
    analogWrite(LED_RPIN,R_L);  
  }
  else if(POWER_ON == false || R_ON == false){
    analogWrite(LED_RPIN,0);
    }
  if (POWER_ON == true && B_ON == true){
    analogWrite(LED_BPIN,B_L);
    }
  else if(POWER_ON == false || B_ON == false){
    analogWrite(LED_BPIN,0);
    
  }
  if (POWER_ON == true && G_ON == true){
    analogWrite(LED_GPIN,G_L);
  }
  else if(POWER_ON == false || G_ON == false){
    analogWrite(LED_GPIN,0);
    
  }
  if (irrecv.decode(&results)){
    ledControls();
  }
  
    
}


void ledControls(){
  if(results.value == 0xFFFFFFFF){
    results.value = key_value;
  }
  else{
    switch(results.value){
      case 0x80BF3BC4:
      if (POWER_ON == true){
        POWER_ON = false;
        Serial.println("POWER IS OFF");
      }
      else if(POWER_ON == false){
        POWER_ON = true;
        Serial.println("POWER IS ON");
        
      }
      break;
      
      case 0x80BF49B6:
      if (R_ON == true){
        R_ON = false;
        Serial.println(" RED IS OFF");
      }
      else if(R_ON == false){
        R_ON = true;
        Serial.println("RED IS ON");
      }
      break;
      
      case 0x80BFC936:
      if (B_ON == true){
        B_ON = false;
        Serial.println("BLUE IS OFF");
      }
      else if(B_ON == false){
        B_ON = true;
        Serial.println("BLUE IS ON");      
      }
      break;
      
      case 0x80BF33CC:
      if (G_ON == true){
        G_ON = false;
        Serial.println("GREEN IS OFF");
      }
      else if(G_ON == false){
        G_ON = true;
        Serial.println("GREEN IS ON");
      }
      break;
      
      case 0x80BFBB44:
      if (B_L>=0 && B_L<254){
        B_L+=51;        
      }
      break;
      
      case 0x80BF31CE:
      if (B_L<=255 && B_L>1){
        B_L-=51;
      }
      break;
      
      case 0x80BF19E6:
      if (G_L>=0 && G_L<254){        
        G_L+=51;
      }
      break;
      
      case 0x80BFE916:
      if (G_L<=255 && G_L>1){
        G_L-=51;
      }
      break;
      
      case 0x80BF53AC:
      if (R_L>=0 && R_L<254){
        R_L+=51;
      }
      break;
      
      case 0x80BF4BB4:
      if (R_L<=255 && R_L>1){
        R_L-=51;
      }
      break;
    }
    
  }
  irrecv.resume();
}
