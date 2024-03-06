/**
 * Example of OTAA device      
 * Authors: 
 *        Ivan Moreno
 *        Eduardo Contreras
 *  June 2019
 * 
 * This code is beerware; if you see me (or any other collaborator 
 * member) at the local, and you've found our code helpful, 
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 * 
 */
#include <lorawan.h>

// OTAA credentials
const char *devEui = "DF625857C791302F";
const char *appEui = "0000000000000001";
const char *appKey = "0001020304D5B3700001020304D5B370";

const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

const sRFM_pins RFM_pins = {
  .CS = 16,
  .RST = 17,
  .DIO0 = 21,
  .DIO1 = 22,
  .DIO2 = 23,
  .DIO5 = 18,
};

void setup() {
  // Setup loraid access
  Serial.begin(9600);

  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  randomSeed(analogRead(0)); 

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF10BW125);

  // set channel to random
  lora.setChannel(MULTI);
  
  // Put OTAA Key and DevAddress here
  lora.setDevEUI(devEui);
  lora.setAppEUI(appEui);
  lora.setAppKey(appKey);

  // Join procedure
  bool isJoined;
  do {
    Serial.println("Joining...");
    isJoined = lora.join();
    
    //wait for 10s to try again
    delay(10000);
  }while(!isJoined);
  Serial.println("Joined to network");
}

void loop() {
  // Check interval overflow
  if(millis() - previousMillis > interval) {
    previousMillis = millis(); 
    int CPUTemp = analogReadTemp();
    sprintf(myStr, "CPUTemp = %d", CPUTemp); 
    Serial.print("Sending: ");
    Serial.println(myStr);
    // Initialiser la trame à 0
    char data[12] = {0};
    // Initialiser la tension à 3 Vdc
    data[0] = 1;
    data[1] = 0x4D;
    // Insérer la valeur de la température du CPU
    data[2] = (CPUTemp*100) >> 8;
    data[3] = (CPUTemp*100) & 0x00FF;
    data[4] = 0;
    data[5] = (15 + random(10)) * 10;
    lora.sendUplink(data, 12, 0, 1);
  }

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.println(outStr);
  }
  
  // Check Lora RX
  lora.update();
}
