
/*
 *   This code is an implementation os LDG Electronics protocol to work
 *   with LDG AT200pc an very useful antenna tunner to all HF and 50mhz band
 *   
 *   This code is based on LDG  at200pc-serial-docs-v17.pdf
 *   
 *   Author: Henrique Brancher Gravina henrique(at)gravina.com.br ( PU3IKE )
 *   
 *   Version 0.1: 21/07/2018
 *   
 * 
 */

// Code returned by at200pc
#define CMD_NOOP          0  // none none No Operation - Also sent to indicateAT200PC has woken up
#define CMD_INDVAL        1  // Value 0-127 none Inductor Value
#define CMD_CAPVAL        2  // Value 0-127 none Capacitor Value
#define CMD_HILOZ         3  // 0 or 1 none HiLoZ relay value
#define CMD_ANTENNA       4  // 0 or 1 none Antenna Selection
#define CMD_FWDPWR        5  // MSB LSB Forward Power, in hundredths of watts.I.e. a value of 1000 indicates 10.0 watts
#define CMD_REVPWR        18 // MSB LSB Reflected Power, in hundredths of watts.
#define CMD_SWR           6  // none LSB SWR (see text)
#define CMD_TXFREQ        7  // MSB LSB Transmit period (see text)
#define CMD_TUNEPASS      9  // none none Indicates that the requested tuning operationsucceeded.
#define CMD_TUNEFAIL      10 // Reason 0-2 none Indicates that the requested tuning operationfailed. Byte 2 indicates reason forfailure. See table 3.3.
#define CMD_VERSION       11 // ProdID MajMin Indicates product and major and minorversion number.
#define CMD_CLEAR_DONE    12 // none none Indicates that the EEPROM erase operationhas completed.
#define CMD_INSTANDBY     13 // none none Indicates that the tuner has been placedin standby mode.
#define CMD_ACTIVE        14 // none none Indicates that the tuner has restored thesettings that were saved prior to enteringstandby mode.
#define CMD_STORE_OK      15 // none none Indicated that the requested store tomemory operation has completed. Notethat this will be indicated whether thestore operation failed or succeeded.
#define CMD_SWRTHRESH     16 // Thresh 0-6 none Indicates the currently active SWRthreshold used for tuning. See table 2.1for values.
#define CMD_AUTO_STATUS   17 // 0 or 1 none Report Auto Tune setting. See table 3.4for values.
#define CMD_UPDATE_STATUS 19 // 0 or 1 none Indicate Live Update on/of

// comand code to ldg at200pc:
#define REQ_NOOP           0 // No Operation
#define REQ_INDUP          1 // Inductor Up. Increase the current inductor value by one.
#define REQ_INDDN          2 // Inductor Down. Decrease the current inductor value byone.
#define REQ_CAPUP          3 // Capacitor Up. Increase the current capacitor value by one.
#define REQ_CAPDN          4 // Capacitor Down. Decrease the current capacitor value byone.
#define REQ_MEMTUNE        5 // Request Memory Tune. Attempt to tune via a built-in stored tune value for this f#define REQuency. Will fall through to a full tune if the memory tune fails.
#define REQ_FULLTUNE       6 // Request Full Tune. Do a complete tuning sequence.
#define REQ_HIZ            8 // Set the HiLoZ relay to High Impedance.
#define REQ_LOZ            9 // Set the HiLoZ relay to Low Impedance.
#define REQ_ANT1          10 // Select Antenna 1
#define REQ_ANT2          11 // Select Antenna 2
#define REQ_ALLUPDATE     40 // Request that the tuner send an update of current relay settings.
#define REQ_VERSION       41 // Ask the tuner for its product ID and version number.
#define REQ_ARM_CLEAR     42 // Arm the memory clear routine. This must be sent immediately before a #define REQ_CLEAR MEM command.
#define REQ_CLEAR_MEM     43 // Clear EEPROM memory data. The memory clear routine must first be armed by sending a #define REQ_ARM CLEAR command. If any intervening commands are sent, the memory clear armed status is unset, and the memory clear will not take place.
#define REQ_TUNER_STANDBY 44 // Place the tuner in standby mode.
#define REQ_TUNER_ACTIVE  45 // Take the tuner out of standby mode.
#define REQ_MANUAL_STORE  46 // Store Tune. Stores the current inductor and capacitor relays settings at the memory location corresponding to the last f#define REQuency transmitted on.
#define REQ_SWR11         50 // Set SWR threshold for “good tune” to 1.1:1 or lower.
#define REQ_SWR13         51 // Set SWR threshold for “good tune” to 1.3:1 or lower.
#define REQ_SWR15         52 // Set SWR threshold for “good tune” to 1.5:1 or lower.
#define REQ_SWR17         53 // Set SWR threshold for “good tune” to 1.7:1 or lower.
#define REQ_SWR20         54 // Set SWR threshold for “good tune” to 2.0:1 or lower.
#define REQ_SWR25         55 // Set SWR threshold for “good tune” to 2.5:1 or lower.
#define REQ_SWR30         56 // Set SWR threshold for “good tune” to 3.0:1 or lower.
#define REQ_RESET         57 // Reset L and C relays, HiLoZ relay
#define REQ_AUTO_ON       58 // Request Automatic Tuning On
#define REQ_AUTO_OFF      59 // Request Automatic Tuning Off
#define REQ_FWD_PWR       60 // Request Current FWD Power
#define REQ_REV_PWR       61 // Request Current Reflected power
#define REQ_SWR           62 // Request Current SWR
#define REQ_UPDATE_ON     63 // Turn live updates on
#define REQ_UPDATE_OFF    64 // Turn live updates off
#define REQ_SET_IND       65 // Directly set Inductor (and HiLoZ) relay value
#define REQ_SET_CAP       66 // Directly set Capacitor relay value
#define REQ_SET_FREQ      67 // Request that tuner recalls tuning memory relay setting for the specified f#define REQuency
#define REQ_MEM_DUMP      68 // Request memory dump of all of EEPROM data

byte ind_val;
byte cap_val;
byte tuneerror;
byte swt_threshhold;

bool hilo_z;
bool antenna_selection;
bool tunepassed = false;
bool eeprom_erased = false;
bool is_standby = false;
bool live_update;
bool at_status;
bool is_active = false;
bool settings_stored = false;

float powerfwd;
float powerrev;
float swr_return;
float tx_frequency;
float ldg_version;


int rts = 21;


void parse_ldg(byte buff[3]){
  
  if(buff[0] == CMD_NOOP){ 
    Serial.println("AT Acordado ");
    
  }else if(buff[0] == CMD_INDVAL){
    Serial.print("Valor Indutor: ");
    ind_val = buff[2];
    Serial.println(ind_val,DEC);
    
  }else if(buff[0] == CMD_CAPVAL){
    Serial.print("Valor Capacitor: ");
    cap_val = buff[2];
    Serial.println(cap_val,DEC);
  
  }else if(buff[0] == CMD_HILOZ  ){
   Serial.print("HILOZ: ");
   hilo_z = buff[2];
   Serial.println( hilo_z == true ? "LOW": "HIGH" );    
  
  }else if(buff[0] == CMD_ANTENNA  ){
   Serial.print("Antenna: ");
   antenna_selection = buff[2];
   Serial.println( antenna_selection == true ? "2": "1" );    
    
  }else if(buff[0] == CMD_FWDPWR){ 
    Serial.print("Power: ");
    int power_buff =  (( buff[2] << 0) & 0xFFFFFF) + (( buff[1] << 8) & 0xFFFFFFFF);
    powerfwd = float(power_buff/100);
    Serial.println(powerfwd); 
    
  }else if(buff[0] == CMD_REVPWR ){
    Serial.print("Potência Refletida: ");
    int power_buff =  (( buff[2] << 0) & 0xFFFFFF) + (( buff[1] << 8) & 0xFFFFFFFF);
    powerrev = float(power_buff/100);
    Serial.println(powerrev); 
    
  }else if(buff[0] == CMD_SWR ){
     Serial.print("Potência Refletida: ");
     float swr = sqrt(buff[3] / 256.0);
     swr = (1.0 + swr) / (1.0 - swr);
     if(swr > 99.9) swr = 99.9;
     Serial.println(swr); 
     
  }else if(buff[0] == CMD_TXFREQ ){
     Serial.print("TX Frequency");
     
    float freq_buff = (( buff[2] << 0) & 0xFFFFFF) + (( buff[1] << 8) & 0xFFFFFFFF);
    tx_frequency = 20480000.0 / freq_buff;
     
     Serial.println(tx_frequency); 
     
  }else if(buff[0] == CMD_TUNEPASS ){
     Serial.println("Tune Passed!");
     tunepassed = true;
     
  }else if(buff[0] == CMD_TUNEFAIL ){
     Serial.print("Tune Failed: ");
     tunepassed = false;
     tuneerror = buff[2];
     if(tuneerror == 00 ) Serial.println("No RF was detected");
     else if(tuneerror == 01 ) Serial.println("RF Carrier was lost before the tune completed.");
     else if(tuneerror == 02 ) Serial.println("The tuner was unable to bring the SWR down below the SWR Threshold.");
  
  }else if(buff[0] == CMD_VERSION ){ // This function must be reviwed
    Serial.print("Version: ");
    ldg_version = map((buff[3])+127,0,255,0,15.15);
    Serial.println(ldg_version);
  
  }else if(buff[0] == CMD_CLEAR_DONE ){
    Serial.println("EEprom eresed!");
    eeprom_erased = true;
    
  }else if(buff[0] == CMD_INSTANDBY ){
    Serial.println("AT in standby now!");
    is_standby = true;
    
  }else if(buff[0] == CMD_ACTIVE ){ // complete
    Serial.println("AT retored the settings!");
    is_active = true;
    
  }else if(buff[0] == CMD_STORE_OK ){ // complete
    Serial.println("AT has saved the settings!");
    settings_stored = true;
    
  }else if(buff[0] == CMD_SWRTHRESH ){ // complete
    Serial.print("SWR threshhold: ");
    swt_threshhold = buff[2];
    
    if(swt_threshhold == 0 ) Serial.println("1.1:1");
    else if(swt_threshhold == 1 ) Serial.println("1.3:1");
    else if(swt_threshhold == 2 ) Serial.println("1.5:1");
    else if(swt_threshhold == 3 ) Serial.println("1.7:1");
    else if(swt_threshhold == 4 ) Serial.println("2.0:1");
    else if(swt_threshhold == 5 ) Serial.println("2.5:1");
    else if(swt_threshhold == 6 ) Serial.println("3.0:1");

    
  }else if(buff[0] == CMD_AUTO_STATUS ){ // complete
    Serial.print("Auto tuning is:");
    at_status = buff[2];
    Serial.println( at_status == true ? "Enabled": "Disabled" );    
    
  }else if(buff[0] == CMD_UPDATE_STATUS){ // complete
    Serial.print("Live update:");
    live_update = buff[2];
    Serial.println( live_update == true ? "Enabled": "Disabled" );    

  }

  
}

void ldg_write(char cmd){
  
   digitalWrite(rts,LOW);
   delay(3);
   digitalWrite(rts,HIGH);
   delay(1);
   Serial1.write(int(cmd));
   delay(5);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(rts,OUTPUT);
  digitalWrite(rts,LOW);
   
  


  
}

byte buff[3];

void loop() {
  // put your main code here, to run repeatedly:
    
  // Read data from LDG AT-200pc
   while(Serial1.available() > 0){
    byte c = Serial1.read();
    if(c == 165){
      Serial1.readBytes(buff,3);
      parse_ldg(buff); // decode the recived string;   
    }
   }


  ldg_write(40); // send command to LDG
  delay(5000);
     
}
