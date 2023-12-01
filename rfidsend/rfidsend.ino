/*
 * Connect RC522 as follows:
 * RC522        Arduino
 * RST          9   
 * SDA(SS)      10 
 * MOSI         11 
 * MISO         12 
 * SCK          13 

 * Connect the RN2xx3 as follows:
 * RN2xx3 -- Arduino
 * Uart TX -- 5
 * Uart RX -- 6
 * Reset -- 7
 * Vcc -- 3.3V
 * Gnd -- Gnd



 */
// For RC522 (RFID)
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// For RN2xx3
#include <rn2xx3.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 6); // RX, TX

//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);

void setup() {
  //output LED pin
  pinMode(LED_BUILTIN, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(9600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");
  initialize_radio();
  //transmit a startup message
  myLora.tx("Helium Mapper on Helium Enschede node");

  delay(1000);

  // setup for rfid
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(100);				// Optional delay. Some board do need more time after init to be ready, see Readme
	//mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }
  //print out the HWEUI so that we can register it via ttnctl
  //Serial.println("When using OTAA, register this DevEUI: ");
  //Serial.println(myLora.hweui());
  //Serial.println("RN2xx3 firmware version:");
  //Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join Helium");
  bool join_result = false;

  const char *appEui = "6081F940C2D730D6";
  const char *appKey = "3B69E96EA8E84A05C6B5C15A091799F4";
  delay(500);
  join_result = myLora.initOTAA(appEui, appKey);

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have Helium coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined Helium");
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}
	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
  //Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
  //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //Serial.print(mfrc522.uid.uidByte[i], HEX);
  content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
  content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

	Serial.println(content);
  myLora.tx(content); //one byte, blocking function
  delay(1000);
}
