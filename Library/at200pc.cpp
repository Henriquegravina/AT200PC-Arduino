
#include "Arduino.h"
#include "at200pc.h"



At200pc::At200pc(HardwareSerial &SerialCom,int pin)
{
  pinMode(pin, OUTPUT);
  rts = pin;
  
  printer = &SerialCom; 
  printer->begin(9600);

}

void At200pc::write(char cmd){
  
   digitalWrite(rts,LOW);
   delay(3);
   digitalWrite(rts,HIGH);
   delay(1);
   printer->write(int(cmd));
   delay(5);
}

void At200pc::read(){
	while(printer->available() > 0){
		byte c = printer->read();
        if(c == 165){
      		printer->readBytes(buff,3);
      		parser(buff); // decode the recived string;   
    	}
   	}
}

int At200pc::antenna_selected(){
	return  (antenna_selection == true ? 2: 1);
	} 


void At200pc::parser(byte buff[3]){
  
  if(buff[0] == CMD_NOOP){ 
    
  }else if(buff[0] == CMD_INDVAL){
    ind_val = buff[1];
    
  }else if(buff[0] == CMD_CAPVAL){
    cap_val = buff[1];
  
  }else if(buff[0] == CMD_HILOZ  ){
   hilo_z = buff[1];
   //Serial.println( hilo_z == true ? "LOW": "HIGH" );    
  
  }else if(buff[0] == CMD_ANTENNA  ){
   antenna_selection = buff[1];
   //Serial.println( antenna_selection == true ? "2": "1" );    
    
  }else if(buff[0] == CMD_FWDPWR){ 
   // Serial.print("Power: ");
    int power_buff =  (( buff[2] << 0) & 0xFFFFFF) + (( buff[1] << 8) & 0xFFFFFFFF);
    powerfwd = float(power_buff/100);
   // Serial.println(powerfwd); 
    
  }else if(buff[0] == CMD_REVPWR ){
   // Serial.print("Potência Refletida: ");
    int power_buff =  (( buff[2] << 0) & 0xFFFFFF) + (( buff[1] << 8) & 0xFFFFFFFF);
    powerrev = float(power_buff/100);
   // Serial.println(powerrev); 
    
  }else if(buff[0] == CMD_SWR ){
   //  Serial.print("Relacao Refletida: ");
     float swr = sqrt(buff[1] / 256.0);
     swr = (1.0 + swr) / (1.0 - swr);
     if(swr > 99.9) swr = 99.9;
    // Serial.println(swr); 
     
  }else if(buff[0] == CMD_TXFREQ ){
   //  Serial.print("TX Frequency");
    float freq_buff = (( buff[2] << 0) & 0xFFFFFF) + (( buff[1] << 8) & 0xFFFFFFFF);
    tx_frequency = 20480000.0 / freq_buff;
    // Serial.println(tx_frequency); 
     
  }else if(buff[0] == CMD_TUNEPASS ){
	 // Serial.println("Tune Passed!");
     tunepassed = true;
     
  }else if(buff[0] == CMD_TUNEFAIL ){
     Serial.print("Tune Failed: ");
     tunepassed = false;
     tuneerror = buff[1];
     if(tuneerror == 00 ) Serial.println("No RF was detected");
     else if(tuneerror == 01 ) Serial.println("RF Carrier was lost before the tune completed.");
     else if(tuneerror == 02 ) Serial.println("The tuner was unable to bring the SWR down below the SWR Threshold.");
  
  }else if(buff[0] == CMD_VERSION ){ // This function must be reviwed
    Serial.print("Version: ");
    ldg_version = map((buff[2])+127,0,255,0,15.15);
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
    swt_threshhold = buff[1];
    
    if(swt_threshhold == 0 ) Serial.println("1.1:1");
    else if(swt_threshhold == 1 ) Serial.println("1.3:1");
    else if(swt_threshhold == 2 ) Serial.println("1.5:1");
    else if(swt_threshhold == 3 ) Serial.println("1.7:1");
    else if(swt_threshhold == 4 ) Serial.println("2.0:1");
    else if(swt_threshhold == 5 ) Serial.println("2.5:1");
    else if(swt_threshhold == 6 ) Serial.println("3.0:1");

    
  }else if(buff[0] == CMD_AUTO_STATUS ){ 
    Serial.print("Auto tuning is:");
    at_status = buff[1];
    Serial.println( at_status == true ? "Enabled": "Disabled" );    
    
  }else if(buff[0] == CMD_UPDATE_STATUS){ 
    Serial.print("Live update:");
    live_update = buff[1];
    Serial.println( live_update == true ? "Enabled": "Disabled" );    

  }

  
}

