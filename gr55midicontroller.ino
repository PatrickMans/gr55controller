#include <MIDI.h>


/*
* ********************************************************
* GR 55 midi controller 
* 
* Controls the 4 sound sources of the GR55 for both on/off 
* switches and volume level.
* 
* To be placed in a small enclosure to fit the guitar 
* body.
* 
* ********************************************************
*/
MIDI_CREATE_DEFAULT_INSTANCE();

// define midi data
byte GR55_SYNTH1_SW = 0x2003; // The address of the synth1 switch
byte GR55_SYNTH2_SW = 0x2103; // The address of the synth2 switch
byte GR55_COSM_GUITAR_SW = 0x100A; // The address of the COSM guitar switch
byte GR55_NORMAL_PU_SW = 0x0232; // The address of the PU guitar switch
byte GR55_SYNTH1_CC = 0x2004; // The address of the synth1 volume
byte GR55_SYNTH2_CC = 0x2104; // The address of the synth2 volume
byte GR55_COSM_GUITAR_CC = 0x1009; // The address of the COSM guitar volume
byte GR55_NORMAL_PU_CC = 0x0233; // The address of the PU guitar volume
 
// define some values used by the faders and buttons

int PCM1VOLPin = 4;
int PCM2VOLPin = 5;
int MODGVOLPin = 6;
int GPUPVOLPin = 7;
int PCM1VOL;
int PCM2VOL;
int MODGVOL;
int GPUPVOL;
int OLDPCM1VOL = 0;
int OLDPCM2VOL = 0;
int OLDMODGVOL = 0;
int OLDGPUPVOL = 0;
int PCM1SWPin = 4;
int PCM2SWPin = 5;
int MODGSWPin = 6;
int GPUPSWPin = 7;
int PCM1SWstat = 0;
int PCM2SWstat = 0;
int MODGSWstat = 0;
int GPUPSWstat = 0;
boolean STATval1 = false;
boolean STATval2 = false;
boolean STATval3 = false;
boolean STATval4 = false;


byte GR55_device_id = 0x10;
int HYST = 2;

uint8_t MIDI_calc_Roland_checksum(uint16_t sum) {
  uint8_t checksum = 0x80 - (sum % 0x80);
  if (checksum == 0x80) checksum = 0;
  return checksum;
}

void SendSysex(int ad1, int ad0, int DATA_IN) {
//  byte *ad = (byte*)&INPIN; //Split the 32-bit address into four bytes: ad[3], ad[2], ad[1] and ad[0]
//  byte ad3 = 0x18;
//  byte ad2 = 0x00;
  int checksum = MIDI_calc_Roland_checksum(0x18 + 0x00 + ad1 + ad0 + DATA_IN); // Calculate the Roland checksum
//  byte checksum = 0x00;
  byte sysexmessage[] = { 0x41, 0x10, 0x00, 0x00, 0x53, 0x12, 0x18, 0x00, ad1, ad0, DATA_IN, checksum };
  MIDI.sendSysEx(12, sysexmessage, false);
  delay(100);
/*   Serial.write(0xF0);
   Serial.write(0x41);
    Serial.write(0x10);
    Serial.write(0x00);
    Serial.write(0x00);
    Serial.write(0x53);
    Serial.write(0x12);
    Serial.write(0x18);
    Serial.write(0x00);
    Serial.write(ad1);
    Serial.write(ad0);
    Serial.write(DATA_IN);
    Serial.write(checksum);
    Serial.write(0x7F);
*/
}

/*
byte GR55_SYNTH1_SW = 0x2003; // The address of the synth1 switch
byte GR55_SYNTH2_SW = 0x2103; // The address of the synth2 switch
byte GR55_COSM_GUITAR_SW = 0x100A; // The address of the COSM guitar switch
byte GR55_NORMAL_PU_SW = 0x0232; // The address of the PU guitar switch
byte GR55_SYNTH1_CC = 0x2004; // The address of the synth1 volume
byte GR55_SYNTH2_CC = 0x2104; // The address of the synth2 volume
byte GR55_COSM_GUITAR_CC = 0x1009; // The address of the COSM guitar volume
byte GR55_NORMAL_PU_CC = 0x0233;
 */

void SetVol(int INPIN, int VOLUME) {
//  int LEVEL;
  int addr1;
  int addr0;
  byte CCVAL;
  boolean goodread = false;
//  LEVEL = (VOLUME / 1024) * 127;
//  LEVEL = VOLUME * 0.0049;
  switch(INPIN){
      case 1:{
            CCVAL = GR55_SYNTH1_CC;
            addr1 = 0x20;
            addr0 = 0x04;
            goodread = true;
            break;
       };
       case 2:{
            CCVAL = GR55_SYNTH2_CC;
            addr1 = 0x21;
            addr0 = 0x04;
            goodread = true;
            break;
       };
       case 3:{
            CCVAL = GR55_COSM_GUITAR_CC;
            addr1 = 0x10;
            addr0 = 0x09;
            goodread = true;
            break;
       };
       case 4:{
            CCVAL = GR55_NORMAL_PU_CC;
            addr1 = 0x02;
            addr0 = 0x33;
            goodread = true;
            break;
       };
       default:{
        goodread = false;
       };
  }
  if (goodread) {SendSysex(addr1, addr0, VOLUME); };
}
/*
byte GR55_SYNTH1_SW = 0x2003; // The address of the synth1 switch
byte GR55_SYNTH2_SW = 0x2103; // The address of the synth2 switch
byte GR55_COSM_GUITAR_SW = 0x100A; // The address of the COSM guitar switch
byte GR55_NORMAL_PU_SW = 0x0232; // The address of the PU guitar switch
 */
void ToggleSW(int NR) {
int OUTVAL;
int SWKEY;
int swaddr1;
int swaddr0;
boolean good;

switch(NR){
  case 1:{
    if (STATval1) {
      STATval1 = false;
      OUTVAL = 0x01;
    } else {      
      STATval1 = true;
      OUTVAL = 0x00; 
    }
    SWKEY = GR55_SYNTH1_SW;
    swaddr1 = 0x20;
    swaddr0 = 0x03;
    good = true;
    break;
  };
  case 2:{
    if (STATval2) {
      STATval2 = false;
      OUTVAL = 0x01;
    } else {      
      STATval2 = true;
      OUTVAL = 0x00; 
    }
    swaddr1 = 0x21;
    swaddr0 = 0x03;
    SWKEY = GR55_SYNTH2_SW;
    good = true; 
    break;
  };
  case 3:{
    if (STATval3) {
      STATval3 = false;
      OUTVAL = 0x01;
    } else {      
      STATval3 = true;
      OUTVAL = 0x00; 
    }
    swaddr1 = 0x10;
    swaddr0 = 0x0A;
    SWKEY = GR55_COSM_GUITAR_SW;
    good = true;
    break;
  };
  case 4:{
    if (STATval4) {
      STATval4 = false;
      OUTVAL = 0x01;
    } else {      
      STATval4 = true;
      OUTVAL = 0x00; 
    }
    swaddr1 = 0x02;
    swaddr0 = 0x32;
    SWKEY = GR55_NORMAL_PU_SW;
    good = true;
    break;
  };
  default: {
    good = false;
  };
}
  if (good) {SendSysex(swaddr1, swaddr0, OUTVAL);}; 
  delay(100);
}
  
void setup() {
 Serial.begin(31250);
 MIDI.begin();
 pinMode(PCM1SWPin, INPUT);
 pinMode(PCM2SWPin, INPUT);
 pinMode(MODGSWPin, INPUT);
 pinMode(GPUPSWPin, INPUT);
// STARTmidi();
} 

void loop() {
  int VOLTOP;
  int VOLBOT;
    PCM1VOL = analogRead(PCM1VOLPin)/8;
    VOLTOP = OLDPCM1VOL + HYST;
    VOLBOT = OLDPCM1VOL - HYST;
    if (PCM1VOL > VOLTOP || PCM1VOL < VOLBOT) {
      OLDPCM1VOL = PCM1VOL;
      SetVol(1,PCM1VOL);
    }
    PCM2VOL = analogRead(PCM2VOLPin)/8;
    VOLTOP = OLDPCM2VOL + HYST;
    VOLBOT = OLDPCM2VOL - HYST;
    if (PCM2VOL > VOLTOP || PCM2VOL < VOLBOT) {
      OLDPCM2VOL = PCM2VOL;
      SetVol(2,PCM2VOL);
    }
    MODGVOL = analogRead(MODGVOLPin)/8;
    VOLTOP = OLDMODGVOL + HYST;
    VOLBOT = OLDMODGVOL - HYST;
    if (MODGVOL > VOLTOP || MODGVOL < VOLBOT) {
      OLDMODGVOL = MODGVOL;
      SetVol(3,MODGVOL);
    }
    GPUPVOL = analogRead(GPUPVOLPin)/8;
    VOLTOP = OLDGPUPVOL + HYST;
    VOLBOT = OLDGPUPVOL - HYST;
    if (GPUPVOL > VOLTOP || GPUPVOL < VOLBOT) {
      OLDGPUPVOL = GPUPVOL;
      SetVol(4,GPUPVOL);
    }
    PCM1SWstat = digitalRead(PCM1SWPin);  
    PCM2SWstat = digitalRead(PCM2SWPin);  
    MODGSWstat = digitalRead(MODGSWPin);  
    GPUPSWstat = digitalRead(GPUPSWPin);  
    if (PCM1SWstat == LOW) {ToggleSW(1);};
    if (PCM2SWstat == LOW) {ToggleSW(2);};
    if (MODGSWstat == LOW) {ToggleSW(3);};
    if (GPUPSWstat == LOW) {ToggleSW(4);};    
    delay(100);
}
