
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
// LiquidCrystal lcd(rs, enable, d4, d5, d6, d7) 
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);
#include "pitches.h"

int buzzerpin = 41;


const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {3,4,5,6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7,8,9}; //connect to the column pinouts of the keypad
 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
 
  int numberplayers;
  int amount;
  int amounttransf = 0;
  int totalmoney[7];
  int k = 0;
  String serial[9];
  char soldibuild[4];

  byte data_count = 0;

  /*#define pulsantemulti 8
  #define pulsantemeno 9
  #define pulsantepiu 10
  Vengono definiti PIN del RFID reader*/
  #define SDA_DIO 53  // 53 per Arduino Mega
  #define RESET_DIO 49
  MFRC522 RC522(SDA_DIO, RESET_DIO); 

void setup() {

  pinMode(buzzerpin, OUTPUT);
  Serial.begin(9600);
  /* Abilita SPI*/
  SPI.begin(); 
  /* Viene inizilizzato RFID reader */
  RC522.PCD_Init();
  RC522.PCD_SetAntennaGain(RC522.RxGain_max);

  // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.print(" Monopoly Bank ");     
  delay(3000);
  numberplayersfunction();
  initialmoneyfunction();
  assegnatag();
  preparation();  

}

void numberplayersfunction(){
  
  lcd.clear();
  lcd.print("Number of Players");  
  lcd.setCursor(0, 1);
  while (numberplayers == 0){
    char key = keypad.getKey();
    if (key != NO_KEY && key != '*' && key != '#'){
      if (key > '8') {
      lcd.print("Max 8 players"); 
      delay(1500);
      lcd.clear();
      lcd.print(key); 
      delay(1500);
      lcd.clear();
      lcd.print("number player"); 
      lcd.setCursor(0, 1);
      } 
      else if (key < '2') {
      lcd.print("Min 2 player"); 
      delay(1500);
      lcd.clear();
      lcd.print(key); 
      delay(1500);
      lcd.clear();
      lcd.print("number player"); 
      lcd.setCursor(0, 1);
      }
      else {
      lcd.print(key); 
      numberplayers=key - 48;
      lcd.clear();
      lcd.print(numberplayers);
      delay(1500);
      return;
      }
    }
  }
}

void initialmoneyfunction(){
  lcd.clear();
  lcd.print("Starting Money");   
  lcd.setCursor(0, 1);
  char soldibuild[3];
  char key;
  int j = 0;
  int i = 0;
  while (i <= (numberplayers - 1)){
    
    key = keypad.getKey();
    if (key != NO_KEY && key != '*' && key != '#'){
      soldibuild[k] = key;
      lcd.print(key);
      k++;
    }  

    if(k == 4) {

      for(i=0; i<=numberplayers - 1;i++){
        totalmoney[i]=atoi(soldibuild);
          

      }
      lcd.clear();
      clearData(); 
      k = 0;
      lcd.print("OK");  
      delay(1500);
    }
  }
}

void assegnatag() {
  
    lcd.clear();
    int i = 0;
    int j = 0;
    int y = 0;
  while (j <= (numberplayers - 1)){
    for(i=0; i<=numberplayers - 1;i++){
    lcd.print("Tag Giocatore "); 
    lcd.print(i + 1);
    while (y == i){ 


      if (RC522.PICC_IsNewCardPresent())  {
          if (RC522.PICC_ReadCardSerial()) {
            delay(300);
            String strID = "";
            for (byte i = 0; i < 4; i++) {
              strID +=
              (RC522.uid.uidByte[i] < 0x10 ? "0" : "") +
              String(RC522.uid.uidByte[i], HEX) +
              (i != 3 ? ":" : "");
            }
        
            if (strID != "00:00:00:00") {
          
              serial[i] = strID;
              lcd.setCursor(0, 1);
              lcd.print("  Acquired  ");  
              soundrecording();
              delay(1500);
              lcd.clear();
              y++;
            }
          }
      }
    }
    j++;
    }
  }
}

void preparation() {
  lcd.clear();
  lcd.print("amount");  
  lcd.setCursor(0, 1);}


void transfer() {
  lcd.clear();
  lcd.print("transfer");
  lcd.setCursor(0, 1);
  int creditoreok = 0;
  int preptrasf = 0;
  int checkamount = 0;
  int t = 0;
  amounttransf = 0;
  char moneytransf[4];
  while (preptrasf == 0) {   
    char key = keypad.getKey();
    if (key != NO_KEY && key != '*' && key != '#'){
      moneytransf[t] = key;
      lcd.print(key);
      t++;
    }    
    if (key == '*' ){
      
      amounttransf = atoi(moneytransf);
      lcd.clear();
      lcd.print("Trasferire ") ;
      lcd.print(amounttransf) ;
      while (checkamount == 0) {
        
    char keyconfirm = keypad.getKey();
      if (keyconfirm == '#' ){
        lcd.clear();
        lcd.print("Annullato") ;
        delay(1500);
        clearData();
        checkamount = 1;
        preparation();
        return;
      }

      if (keyconfirm == '*'){
        checkamount = 1; 
      }
      }
      paytransfer();
      receivetransfer(); 
      amounttransf = 0;
      return;
    }
  }
}

void paytransfer() {
  int debtorok = 0;
  int debtorleft= 0;
  lcd.clear();
  lcd.print("Debtor");
  while (debtorok == 0){
    if (RC522.PICC_IsNewCardPresent())  {
      if (RC522.PICC_ReadCardSerial()) {
        delay(300);
        String strID = "";
        for (byte i = 0; i < 4; i++) {
          strID +=
            (RC522.uid.uidByte[i] < 0x10 ? "0" : "") +
            String(RC522.uid.uidByte[i], HEX) +
            (i != 3 ? ":" : "");
        }
        String serial = strID;

        if (serial != "0"){
          for(int i=0; i<=numberplayers -1;i++){
            if (serial == serial[i]) {
                      
              debtorleft = totalmoney[i];
              totalmoney[i]=totalmoney[i] - amounttransf;
              if (totalmoney[i] < 1){
                lcd.clear();
                lcd.print("Bankrupt!"); 
                bankruptcysound();
                amounttransf = debtorleft;
                delay(1500);
                debtorok = 1;
              } else {
                lcd.clear();
                lcd.print("Paid!") ;
                losemoney();
                delay(1500);
                debtorok = 1;
              }
            }
          }
        }
      }
    }      
  }
}


void receivetransfer() {
  
  int creditoreok = 0;
  lcd.clear();
  lcd.print("Creditore");
    while (creditoreok == 0) {

      if (RC522.PICC_IsNewCardPresent())  {
                    if (RC522.PICC_ReadCardSerial()) {
                    delay(300);
                          String strID = "";
        for (byte i = 0; i < 4; i++) {
          strID +=
            (RC522.uid.uidByte[i] < 0x10 ? "0" : "") +
            String(RC522.uid.uidByte[i], HEX) +
            (i != 3 ? ":" : "");
        }
        String serial = strID;
        
          if (serial != "0"){
          for(int i=0; i<=numberplayers -1;i++){
            if (serial == serial[i]) {

                totalmoney[i]=totalmoney[i] + amounttransf;
                lcd.clear();
                lcd.print("Incassato!") ;
                rumoresoldi();
                delay(1500);
                lcd.clear();
                lcd.print("amount");  
                lcd.setCursor(0, 1);
                creditoreok = 1;
               clearData();
              /* k = 0; */
              
            }
          }
          }
        }
      }
      } 
}

void incassare() {
  
   int creditoreok = 0;
   int i = 0;
       char key = keypad.getKey();
         lcd.clear();
      lcd.print("Incassare "); 
      lcd.print(amount) ;
      
      while (creditoreok == 0) {
      if (key == '*' ){
        lcd.clear();
        lcd.print("Annullato") ;
        delay(1500);
        lcd.clear();
        lcd.print("amount");  
        lcd.setCursor(0, 1);
        return;
      }

        if (RC522.PICC_IsNewCardPresent())  {   
           if (RC522.PICC_ReadCardSerial()) {           
           delay(300);
          String strID = "";
        for (byte i = 0; i < 4; i++) {
          strID +=
            (RC522.uid.uidByte[i] < 0x10 ? "0" : "") +
            String(RC522.uid.uidByte[i], HEX) +
            (i != 3 ? ":" : "");
        }
        String serial = strID;
        
        lcd.print (strID); 

        if (serial != "0"){
          for(i=0; i<=numberplayers -1;i++){
            
            if (serial == serial[i]) {
              totalmoney[i]=totalmoney[i] + amount;
              lcd.clear();
              lcd.print("Incassato!") ;
              rumoresoldi();
              delay(1500);
              lcd.clear();
              lcd.print("amount");  
              lcd.setCursor(0, 1);
              creditoreok = 1;
              clearData();
             
            }
          }
        }
      }
    }
  }
  creditoreok = 0;
  amount = 0;
}

void pagare() {
  int debtorok = 0;
  int i = 0;
  char key = keypad.getKey();
  lcd.clear();
  lcd.print("Pagare "); 
  lcd.print(amount) ;

  while (debtorok == 0){  
    if (key == '*' ){
      lcd.clear();
      lcd.print("Annullato") ;
      delay(1500);
      lcd.clear();        
      lcd.print("amount");  
      lcd.setCursor(0, 1);
      return;
     }       


    if (RC522.PICC_IsNewCardPresent())  {
      if (RC522.PICC_ReadCardSerial()) {
        delay(300);
        String strID = "";
        for (i = 0; i < 4; i++) {
          strID +=
            (RC522.uid.uidByte[i] < 0x10 ? "0" : "") +
            String(RC522.uid.uidByte[i], HEX) +
            (i != 3 ? ":" : "");
        }
        String serial = strID;
        
        if (serial != "0") {
          for(int i=0; i<=numberplayers -1;i++){
            if (serial == serial[i]) {
              totalmoney[i]=totalmoney[i] - amount;
              if (totalmoney[i] < 1){
                lcd.clear();
                lcd.print("Bankrupt!"); 
                bankruptcysound();
                delay(1500);
                lcd.clear();
                lcd.print("amount");  
                lcd.setCursor(0, 1);
                debtorok = 1;
                clearData();
              } else {
                lcd.clear();
                lcd.print("Paid!") ;
                losemoney();
                delay(1500);
                lcd.clear();
                lcd.print("amount");  
                lcd.setCursor(0, 1);
                debtorok = 1;
                clearData();
              }
            }
          }
        }
      }
    }
  }
  debtorok = 0;
  amount = 0;
}

void clearData(){
  while(k !=0){
    soldibuild[k--] = 0; 
  }
  soldibuild[0] = 0;
  return; 
}

void balance(){

  for(int i=0; i<=numberplayers -1;i++){
    lcd.clear();
    lcd.print("Player ");
    lcd.print(i+1);
    lcd.setCursor(0, 1);
    if (totalmoney[i] > 0) {
      lcd.print(totalmoney[i]);
      delay(1500);
    } else {
      lcd.print("Bankrupt!");
      delay(1500);  
    }
    preparation();
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  int h = 0;
  amount = 0;
  char key = keypad.getKey();
  if (key != NO_KEY && key != '*' && key != '#'){
    soldibuild[k] = key;
    lcd.print(key);
    k++;
  }  

/* DA QUI DA MODIFICARE */

  if (key == '*' ){
    amount = atoi(soldibuild);
    if (amount == 0) {
      balance();
    } else {
      incassare();
    }
  }

  if (key == '#' ){
    amount = atoi(soldibuild);
    lcd.setCursor(0, 1); 
    delay(1000);
    if (amount == 0) {
      transfer();
    } else {
      pagare();
    }
  }


  if (key == '0' ){
    amount = atoi(soldibuild);
    if (amount == 0) {
      lanciodadi();
      k = 0;     
    } 
  }

  if (RC522.PICC_IsNewCardPresent())  {   
    if (RC522.PICC_ReadCardSerial()) {           
      delay(300);
      String strID = "";
      for (byte i = 0; i < 4; i++) {
        strID +=
        (RC522.uid.uidByte[i] < 0x10 ? "0" : "") +
        String(RC522.uid.uidByte[i], HEX) +
        (i != 3 ? ":" : "");
      }
      String serial = strID;  
      if (serial != "0"){

        for(h=0; h<=numberplayers -1;h++){
            
          if (serial == serial[h]) {
            lcd.clear();
            lcd.print("Saldo Gioc. ") ;
            lcd.print(h + 1) ;
            lcd.setCursor(0, 1);
            lcd.print(totalmoney[h]) ;
            delay(1500);
            lcd.clear();
            lcd.print("amount");  
            lcd.setCursor(0, 1);
          }
        }
      }
    }
  }   
}

void rumoresoldi(){
  tone(buzzerpin,NOTE_B5,100);
  delay(100);
  tone(buzzerpin,NOTE_E6,850);
  delay(800);
  noTone(buzzerpin);
}

void losemoney(){
  tone(buzzerpin,NOTE_G4);
  delay(250);
  tone(buzzerpin,NOTE_C4);
  delay(500);
  noTone(buzzerpin);
}

void bankruptcysound(){
  tone(buzzerpin,NOTE_E4);
  delay(250);
  tone(buzzerpin,NOTE_D4);  
  delay(250);
  tone(buzzerpin,NOTE_C4);  
  delay(250);
  tone(buzzerpin,NOTE_B4);  
  delay(250);
  tone(buzzerpin,NOTE_A4);
  delay(500);
  noTone(buzzerpin);
}

void soundrecording(){
  tone(buzzerpin,NOTE_A4);
  delay(250);
  tone(buzzerpin,NOTE_B4);  
  delay(250);
  tone(buzzerpin,NOTE_C4);  
  delay(250);
  tone(buzzerpin,NOTE_D4);  
  delay(250);
  tone(buzzerpin,NOTE_E4);
  delay(500);
  noTone(buzzerpin);
}

void lanciodadi(){
  lcd.clear();
  lcd.print("  Lancio  Dadi  ");
  int d = 0;
  int dado1 = 0;
  int dado2 = 0;
  while (d < 10) {
    dado1 = (random(1,7));
    dado2 = (random(1,7));
    lcd.setCursor (1, 1);
    lcd.print (dado1);
    lcd.setCursor (4, 1);
    lcd.print ("+");
    lcd.setCursor (6, 1);
    lcd.print (dado2);
    tone(buzzerpin,NOTE_A4,25);
    delay(100);
    noTone(buzzerpin);
    d++;
  }
  lcd.setCursor (9, 1);
  lcd.print ("=");
  lcd.setCursor (12, 1);
  lcd.print (dado1 + dado2);
  delay(1500);
  preparation();
}


