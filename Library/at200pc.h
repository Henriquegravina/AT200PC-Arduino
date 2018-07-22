/*

	LDG Eletronics AT200pc control library
	

*/


#ifndef At200pc_h
#define At200pc_h

#include "Arduino.h"

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
#define REQ_MEMTUNE        5 // Request Memory Tune. Attempt to tune via a built-in stored tune value for this frequency. Will fall through to a full tune if the memory tune fails.
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




class At200pc
{
  public:
    At200pc(HardwareSerial &SerialCom,int pin);
    void parser(byte buff[3]);
    void write(char cmd);
    void read();
    int antenna_selected();
    
  private:
    HardwareSerial* printer;
    
    int rts; // pint to wake up at200pc
    
    char buff[3];
    
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
    
};

#endif