#include "SoftwareSerial.h"

#define SERIAL_DEBUG          // comment out this to remove serial debug
#define SMS_ENABLED           // comment out this to disable sms sending, to save from expenses
#define SIMULATE_BY_SERIAL    // comment out this to use the actual SIM800L

SoftwareSerial SIM800L(11, 12);  // Arduino D11(as Rx pin) to SIM800L Tx pin
                                 // Arduino D12(as Tx pin) to SIM800L Rx pin
#define RED_LED   7   // Pin assignments for LED and Switch
#define GRN_LED   6
#define BLU_LED   5
#define SW_PIN    4

#define BUFFER_SIZE   127       // incoming data buffer size
char bufferData[BUFFER_SIZE];   // holds the char data
char bufferIndex;               // holds the position of the char in the buffer

// This variable holds the SMS sender mobile number, it will be updated when new sms arrived
char sender_num[14] = {'+', '6', '3', '9', '7', '5', '9', '2', '3', '1', '2', '4', '3' };

// This variable holds the mobile number of the owner, emergency sms will be sent to this
// when the event is triggered lets say the button press
char default_num[14] = {'+', '6', '3', '9', '7', '5', '9', '2', '3', '1', '2', '4', '3' };

char sender_cmd[7];     // holds the specific command when a new SMS is received

bool CMT_OK = false;    // CMT is for processing the SMS senders mobile number
int cmt_pos_index = 0;  
int cmt_pos_count = 0;

bool CMD_OK = false;    // CMD is for processing the parsing the sms command
int cmd_pos_index = 0;
int cmd_pos_count = 0;

bool NUM_OK = false;    // NUM is for processing the CHANGE NUMBER option
int num_pos_index = 0;
int num_pos_count = 0;

bool redLEDState = false;   // holds the state of LEDs
bool grnLEDState = false;   //    true:  means ON
bool bluLEDState = false;   //    false: means OFF

bool newData = false;

bool isReplyOn = false;     // hold the settings for sending the status update for every sms 
                            //    true:  reply to every sms
                            //    false: reply only to some sms request like request for status

bool currState = HIGH;            // this variables are for the button debouncing
bool prevState = HIGH;
unsigned long startMillis = 0;
unsigned long debounceDelay = 50;

/*
 *  Function: void setup()
 *  Purpose:
 *    This function is for initializations
 */
void setup(){
  Serial.begin(9600);
  Serial.println("Initializing...");
  SIM800L.begin(9600);
  delay(2000);      // gives enough time for the GSM module to bootup
  initializeGSM();  // set the necessary initializations for the GSM module
  pinMode(RED_LED, OUTPUT);           // set the pin directions of other pins
  pinMode(GRN_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  digitalWrite(RED_LED, redLEDState); // set the current states of LEDs
  digitalWrite(GRN_LED, grnLEDState);
  digitalWrite(BLU_LED, bluLEDState);
  memset(bufferData, 0, sizeof(bufferData)); // Initialize the string     
  bufferIndex = 0; 
  Serial.println("Setup done");

}

/*
 *  Function: void loop()
 *  Purpose:
 *    This is the main loop
 */
void loop(){
  manageButton();
  manageSIM800();
  delay(500);
}

/*
 *  Function: void initializeGSM()
 *  Purpose:
 *    This function is for the SIM800L initializations
 */
void initializeGSM() {
#ifdef SIMULATE_BY_SERIAL
  Serial.println("AT");                   // Sends an ATTENTION command, reply should be OK
  Serial.println("AT+CMGF=1");            // Configuration for sending SMS
  Serial.println("AT+CNMI=1,2,0,0,0");    // Configuration for receiving SMS
  Serial.println("AT&W");                 // Save the configuration settings
#else // actual
  SIM800L.println("AT");                  // Sends an ATTENTION command, reply should be OK
  delay(1000);
  SIM800L.println("AT+CMGF=1");           // Configuration for sending SMS
  delay(1000);
  SIM800L.println("AT+CNMI=1,2,0,0,0");   // Configuration for receiving SMS
  delay(1000);
  Serial.println("AT&W");                 // Save the configuration settings
  delay(1000);
#endif // SIMULATE_BY_SERIAL
}

/*
 *  Function: void manageButton()
 *  Purpose:
 *    This is for polling the state of the button.
 *    Additional button debouncing is use to prevent
 *    false button presses.
 */
void manageButton() {
  bool currRead = digitalRead(SW_PIN);  

  if (currState != prevState) {
    startMillis = millis();
  }
  if ((millis() - startMillis) > debounceDelay) {
    if (currRead != currState) {
      currState = currRead;
      if (currState == LOW) {
#ifdef SERIAL_DEBUG
        Serial.println("Alert!");
#endif //SERIAL_DEBUG
        sendSMS("Alert! Event triggered", default_num);
      }
    }
  }
  prevState = currState;
}

/*
 *  Function: void manageSIM800()
 *  Purpose:
 *    This is for parsing the incoming serial data from SIM800L
 *    so that we can use it for controlling something.
 */
void manageSIM800() {
 
#ifdef SIMULATE_BY_SERIAL
  while (Serial.available() > 0) {
    bufferData[bufferIndex] = Serial.read();
    // Find the string "CMD:"
    //    if found, parse the command for processing
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'D') && 
        (bufferData[bufferIndex] == ':')      )  {               
      //Serial.println("CMD"); 
      CMD_OK = true;      
      memset(sender_cmd, 0, sizeof(sender_cmd));
      cmd_pos_index = bufferIndex;  // get the position
      cmd_pos_count = 0;            // reset pos counter   
    }    
    // Finds the string "NUM:"
    //    if found, parse the command for processing
    if( //(bufferData[bufferIndex-3] == 'N') &&
        //(bufferData[bufferIndex-2] == 'U') && 
        (bufferData[bufferIndex-1] == 'M') && 
        (bufferData[bufferIndex] == ':')      )  {              
      //Serial.println("NUM"); 
      NUM_OK = true;
      memset(default_num, 0, sizeof(default_num));
      num_pos_index = bufferIndex;  // get the position
      num_pos_count = 0;            // reset pos counter 
      
    } 
    // Finds the string "CMT:"
    //    if found, parse the command for processing
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'T') && 
        (bufferData[bufferIndex] == ':')      )  {            
      //Serial.println("CMT");  
      CMT_OK = true;
      memset(sender_num, 0, sizeof(sender_num));    
      cmt_pos_index = bufferIndex;  // get the position
      cmt_pos_count = 0;            // reset pos counter 
    }    
    // String "CMT:" is found, 
    //  parse the sender number for the reply
    //
    if ( ( (bufferIndex - cmt_pos_index) > 2 ) && (CMT_OK) ) {
      if ( cmt_pos_count < 13 ) {
        sender_num[cmt_pos_count] =  bufferData[bufferIndex];
        cmt_pos_count++;
      } else {
        //Serial.println(sender_num);
        CMT_OK = false; // done
      }
    }
    
    // String "NUM:" is found
    //  parse it then save it as default number then send a reply
    //
    if ( ( (bufferIndex - num_pos_index) > 1 ) && (NUM_OK) ) {
      if ( bufferData[bufferIndex] != '<' ) {
        default_num[num_pos_count] =  bufferData[bufferIndex];
        num_pos_count++;
      } else {
        char msg[32] = "Default # is now ";
        sendSMS((strcat( msg, default_num )), sender_num);                          // send it to the recipient number.
        NUM_OK = false; // done
        break;
      }
    } 
    // String "CMD:" is found,
    //  parse the command, then execute the command
    //
    if ( ( (bufferIndex - cmd_pos_index) > 1 ) && (CMD_OK)) {
      if (bufferData[bufferIndex] == '<') {
        //Serial.println(sender_cmd);
        processCommand();
        CMT_OK = false; // done
        break;
      } else {
        sender_cmd[cmd_pos_count] =  bufferData[bufferIndex];
        cmd_pos_count++;
      }
    } 
    bufferIndex++;                              
  }           

  memset(bufferData, 0, sizeof(bufferData));    // Initialize the string     
  bufferIndex = 0;
#else // actual SIM800L GSM Module
  while (SIM800L.available() > 0) {
    bufferData[bufferIndex] = SIM800L.read();
    // Find the string "CMD:"
    //    if found, parse the command for processing
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'D') && 
        (bufferData[bufferIndex] == ':')      )  {               
      //Serial.println("CMD"); 
      CMD_OK = true;      
      memset(sender_cmd, 0, sizeof(sender_cmd));
      cmd_pos_index = bufferIndex;  // get the position
      cmd_pos_count = 0;            // reset pos counter   
    }    
    // Finds the string "NUM:"
    //    if found, parse the command for processing
    if( //(bufferData[bufferIndex-3] == 'N') &&
        //(bufferData[bufferIndex-2] == 'U') && 
        (bufferData[bufferIndex-1] == 'M') && 
        (bufferData[bufferIndex] == ':')      )  {              
      //Serial.println("NUM"); 
      NUM_OK = true;
      memset(default_num, 0, sizeof(default_num));
      num_pos_index = bufferIndex;  // get the position
      num_pos_count = 0;            // reset pos counter 
    } 
    // Finds the string "CMT:"
    //    if found, parse the command for processing
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'T') && 
        (bufferData[bufferIndex] == ':')      )  {            
      //Serial.println("CMT");  
      CMT_OK = true;
      memset(sender_num, 0, sizeof(sender_num));    
      cmt_pos_index = bufferIndex;  // get the position
      cmt_pos_count = 0;            // reset pos counter 
    }    
    // String "CMT:" is found, 
    //  parse the sender number for the reply
    // +CMT: "+639175291539"
    if ( ( (bufferIndex - cmt_pos_index) > 2 ) && (CMT_OK) ) {
      if ( cmt_pos_count < 13 ) {
        sender_num[cmt_pos_count] =  bufferData[bufferIndex];
        cmt_pos_count++;
      } else {
        //Serial.println(sender_num);
        CMT_OK = false; // done
      }
    }
    // String "NUM:" is found
    //  parse it then save it as default number then send a reply
    //
    if ( ( (bufferIndex - num_pos_index) > 1 ) && (NUM_OK) ) {
      if ( bufferData[bufferIndex] != '<' ) {
        default_num[num_pos_count] =  bufferData[bufferIndex];
        num_pos_count++;
      } else {
        char msg[32] = "Default # is now ";
        sendSMS((strcat( msg, default_num )), sender_num);                          // send it to the recipient number.
        NUM_OK = false; // done
        break;
      }
    } 
    // String "CMD:" is found,
    //  parse the command, then execute the command
    //  CMD: GRN 0<
    if ( ( (bufferIndex - cmd_pos_index) > 1 ) && (CMD_OK)) {
      if (bufferData[bufferIndex] == '<') {
        //Serial.println(sender_cmd);
        processCommand();
        CMT_OK = false; // done
        break;
      } else {
        sender_cmd[cmd_pos_count] =  bufferData[bufferIndex];
        cmd_pos_count++;
      }
    } 
    bufferIndex++;                              
  }           
  memset(bufferData, 0, sizeof(bufferData));    // Initialize the string     
  bufferIndex = 0;
#endif //SIMULATE_BY_SERIAL
}

/*
 *  Function: void processCommand()
 *  Purpose:
 *    This function execute the receive commands via SMS
 *    CMD: RED 1<
 */   
void processCommand() {
  if        (strcmp(sender_cmd, "RED 1") == 0) {
    redLEDState = true;
    if (isReplyOn)
      sendSMS("Red LED On", sender_num);
  } else if (strcmp(sender_cmd, "RED 0") == 0) {
    redLEDState = false;
    if (isReplyOn)
      sendSMS("Red LED Off", sender_num);
  } else if (strcmp(sender_cmd, "GRN 1") == 0) {
    grnLEDState = true;
    if (isReplyOn)
      sendSMS("Green LED On", sender_num);
  } else if (strcmp(sender_cmd, "GRN 0") == 0) {
    grnLEDState = false;
    if (isReplyOn)
      sendSMS("Green LED Off", sender_num);
  } else if (strcmp(sender_cmd, "BLU 1") == 0) {
    bluLEDState = true;
    if (isReplyOn)
      sendSMS("Blue LED On", sender_num);
  } else if (strcmp(sender_cmd, "BLU 0") == 0) {
    bluLEDState = false;
    if (isReplyOn)
      sendSMS("Blue LED Off", sender_num);
  } else if (strcmp(sender_cmd, "ALL 0") == 0) {
    redLEDState = false;
    grnLEDState = false;
    bluLEDState = false;
    if (isReplyOn)
      sendSMS("All LED Off", sender_num);
  } else if (strcmp(sender_cmd, "ALL 1") == 0) {
    redLEDState = true;
    grnLEDState = true;
    bluLEDState = true;
    if (isReplyOn)
      sendSMS("All LED On", sender_num);
  } else if (strcmp(sender_cmd, "REP 1") == 0) {       // This command is to turn ON the reply to every sms
    isReplyOn = true;
#ifdef SMS_ENABLED
    sendSMS("Reply is now ON", sender_num);
#endif //SMS_ENABLED
  } else if (strcmp(sender_cmd, "REP 0") == 0) {      // This command is to turn OFF the reply to every sms
    isReplyOn = false;
#ifdef SMS_ENABLED
    sendSMS("Reply is now OFF", sender_num);
#endif //SMS_ENABLED
  } else if (strcmp(sender_cmd, "GET S") == 0) {    // This command sends the current status of all LEDs
    String statusSMS = "";                               // Create the status reply
    statusSMS = "Red LED is ";
    if (redLEDState) {
      statusSMS += "ON";
    } else statusSMS += "OFF";
    statusSMS += ". Green LED is ";
    if (grnLEDState) {
      statusSMS += "ON";
    } else statusSMS += "OFF";
    statusSMS += ". Blue LED is ";
    if (bluLEDState) {
      statusSMS += "ON";
    } else statusSMS += "OFF";
#ifdef SMS_ENABLED
    sendSMS(statusSMS, sender_num);
#endif //SMS_ENABLED
  } 
  digitalWrite(RED_LED, redLEDState);   // Sets the LED states according
  digitalWrite(GRN_LED, grnLEDState);   //  according to the received
  digitalWrite(BLU_LED, bluLEDState);   //  commands

}

/*
 *  Function: void sendSMS(String message, String mobile)
 *  Purpose:
 *    This function is use for sending the reply SMS
 */
void sendSMS(String message, String mobile) {
#ifdef SIMULATE_BY_SERIAL
  Serial.println("AT+CMGS=\"" + mobile + "\"\r"); //Mobile phone number to send message
  Serial.println(message);                        // This is the message to be sent.
  Serial.println((char)26);                       // ASCII code of CTRL+Z to finalized the sending of sms
#else // actual
  //SIM800L.println("AT+CMGF=1");                    //Sets the GSM Module in Text Mode
  //delay(1000);
  SIM800L.println("AT+CMGS=\"" + mobile + "\"\r"); //Mobile phone number to send message
  delay(1000);
  SIM800L.println(message);                        // This is the message to be sent.
  delay(1000);
  SIM800L.println((char)26);                       // ASCII code of CTRL+Z to finalized the sending of sms
  delay(1000);
#endif // SIMULATE_BY_SERIAL
}
