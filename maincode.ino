
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
 
  int number_of_players;
  int amount;
  int importotransf = 0;
  int totaledenaro[7];
  int k = 0;
  String seriali[9];
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
  lcd.print(" Banca Monopoly ");     
  delay(3000);
  numerogiocatorifunz();
  soldiinizialifunz();
  assegnatag();
  preparazione();  

}

void numerogiocatorifunz(){
  
  lcd.clear();
  lcd.print("Numero Giocatori");  
  lcd.setCursor(0, 1);
  while (number_of_players == 0){
    char key = keypad.getKey();
    if (key != NO_KEY && key != '*' && key != '#'){
      if (key > '8') {
      lcd.print("Max 8 giocatori"); 
      delay(1500);
      lcd.clear();
      lcd.print(key); 
      delay(1500);
      lcd.clear();
      lcd.print("Numero Giocatori"); 
      lcd.setCursor(0, 1);
      } 
      else if (key < '2') {
      lcd.print("Min 2 giocatori"); 
      delay(1500);
      lcd.clear();
      lcd.print(key); 
      delay(1500);
      lcd.clear();
      lcd.print("Numero Giocatori"); 
      lcd.setCursor(0, 1);
      }
      else {
      lcd.print(key); 
      number_of_players=key - 48;
      lcd.clear();
      lcd.print(number_of_players);
      delay(1500);
      return;
      }
    }
  }
}

void soldiinizialifunz(){
  lcd.clear();
  lcd.print("Soldi iniziali");   
  lcd.setCursor(0, 1);
  char soldibuild[3];
  char key;
  int j = 0;
  int i = 0;
  while (i <= (number_of_players - 1)){
    
    key = keypad.getKey();
    if (key != NO_KEY && key != '*' && key != '#'){
      soldibuild[k] = key;
      lcd.print(key);
      k++;
    }  

    if(k == 4) {

      for(i=0; i<=number_of_players - 1;i++){
        totaledenaro[i]=atoi(soldibuild);
          

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
  while (j <= (number_of_players - 1)){
    for(i=0; i<=number_of_players - 1;i++){
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
          
              seriali[i] = strID;
              lcd.setCursor(0, 1);
              lcd.print("  Acquisito  ");  
              suonoregtag();
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

void preparazione() {
  lcd.clear();
  lcd.print("Importo");  
  lcd.setCursor(0, 1);}


void trasferimento() {
  lcd.clear();
  lcd.print("Trasferimento");
  lcd.setCursor(0, 1);
  int creditoreok = 0;
  int preptrasf = 0;
  int checkimporto = 0;
  int t = 0;
  importotransf = 0;
  char solditransf[4];
  while (preptrasf == 0) {   
    char key = keypad.getKey();
    if (key != NO_KEY && key != '*' && key != '#'){
      solditransf[t] = key;
      lcd.print(key);
      t++;
    }    
    if (key == '*' ){
      
      importotransf = atoi(solditransf);
      lcd.clear();
      lcd.print("Trasferire ") ;
      lcd.print(importotransf) ;
      while (checkimporto == 0) {
        
    char keyconferma = keypad.getKey();
      if (keyconferma == '#' ){
        lcd.clear();
        lcd.print("Annullato") ;
        delay(1500);
        clearData();
        checkimporto = 1;
        preparazione();
        return;
      }

      if (keyconferma == '*'){
        checkimporto = 1; 
      }
      }
      pagatransf();
      ricevitransf(); 
      importotransf = 0;
      return;
    }
  }
}

void pagatransf() {
  int debitoreok = 0;
  int debitoreleft= 0;
  lcd.clear();
  lcd.print("Debitore");
  while (debitoreok == 0){
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
        String seriale = strID;

        if (seriale != "0"){
          for(int i=0; i<=number_of_players -1;i++){
            if (seriale == seriali[i]) {
                      
              debitoreleft = totaledenaro[i];
              totaledenaro[i]=totaledenaro[i] - importotransf;
              if (totaledenaro[i] < 1){
                lcd.clear();
                lcd.print("Bancarotta!"); 
                bancarottasound();
                importotransf = debitoreleft;
                delay(1500);
                debitoreok = 1;
              } else {
                lcd.clear();
                lcd.print("Pagato!") ;
                menosoldi();
                delay(1500);
                debitoreok = 1;
              }
            }
          }
        }
      }
    }      
  }
}


void ricevitransf() {
  
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
        String seriale = strID;
        
          if (seriale != "0"){
          for(int i=0; i<=number_of_players -1;i++){
            if (seriale == seriali[i]) {

                totaledenaro[i]=totaledenaro[i] + importotransf;
                lcd.clear();
                lcd.print("Incassato!") ;
                rumoresoldi();
                delay(1500);
                lcd.clear();
                lcd.print("Importo");  
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
      lcd.print(importo) ;
      
      while (creditoreok == 0) {
      if (key == '*' ){
        lcd.clear();
        lcd.print("Annullato") ;
        delay(1500);
        lcd.clear();
        lcd.print("Importo");  
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
        String seriale = strID;
        
        lcd.print (strID); 

        if (seriale != "0"){
          for(i=0; i<=number_of_players -1;i++){
            
            if (seriale == seriali[i]) {
              totaledenaro[i]=totaledenaro[i] + importo;
              lcd.clear();
              lcd.print("Incassato!") ;
              rumoresoldi();
              delay(1500);
              lcd.clear();
              lcd.print("Importo");  
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
  importo = 0;
}

void pagare() {
  int debitoreok = 0;
  int i = 0;
  char key = keypad.getKey();
  lcd.clear();
  lcd.print("Pagare "); 
  lcd.print(importo) ;

  while (debitoreok == 0){  
    if (key == '*' ){
      lcd.clear();
      lcd.print("Annullato") ;
      delay(1500);
      lcd.clear();        
      lcd.print("Importo");  
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
        String seriale = strID;
        
        if (seriale != "0") {
          for(int i=0; i<=number_of_players -1;i++){
            if (seriale == seriali[i]) {
              totaledenaro[i]=totaledenaro[i] - importo;
              if (totaledenaro[i] < 1){
                lcd.clear();
                lcd.print("Bancarotta!"); 
                bancarottasound();
                delay(1500);
                lcd.clear();
                lcd.print("Importo");  
                lcd.setCursor(0, 1);
                debitoreok = 1;
                clearData();
              } else {
                lcd.clear();
                lcd.print("Pagato!") ;
                menosoldi();
                delay(1500);
                lcd.clear();
                lcd.print("Importo");  
                lcd.setCursor(0, 1);
                debitoreok = 1;
                clearData();
              }
            }
          }
        }
      }
    }
  }
  debitoreok = 0;
  importo = 0;
}

void clearData(){
  while(k !=0){
    soldibuild[k--] = 0; 
  }
  soldibuild[0] = 0;
  return; 
}

void bilancio(){

  for(int i=0; i<=number_of_players -1;i++){
    lcd.clear();
    lcd.print("Giocatore ");
    lcd.print(i+1);
    lcd.setCursor(0, 1);
    if (totaledenaro[i] > 0) {
      lcd.print(totaledenaro[i]);
      delay(1500);
    } else {
      lcd.print("Bancarotta!");
      delay(1500);  
    }
    preparazione();
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  int h = 0;
  importo = 0;
  char key = keypad.getKey();
  if (key != NO_KEY && key != '*' && key != '#'){
    soldibuild[k] = key;
    lcd.print(key);
    k++;
  }  

/* DA QUI DA MODIFICARE */

  if (key == '*' ){
    importo = atoi(soldibuild);
    if (importo == 0) {
      bilancio();
    } else {
      incassare();
    }
  }

  if (key == '#' ){
    importo = atoi(soldibuild);
    lcd.setCursor(0, 1); 
    delay(1000);
    if (importo == 0) {
      trasferimento();
    } else {
      pagare();
    }
  }


  if (key == '0' ){
    importo = atoi(soldibuild);
    if (importo == 0) {
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
      String seriale = strID;  
      if (seriale != "0"){

        for(h=0; h<=number_of_players -1;h++){
            
          if (seriale == seriali[h]) {
            lcd.clear();
            lcd.print("Saldo Gioc. ") ;
            lcd.print(h + 1) ;
            lcd.setCursor(0, 1);
            lcd.print(totaledenaro[h]) ;
            delay(1500);
            lcd.clear();
            lcd.print("Importo");  
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

void menosoldi(){
  tone(buzzerpin,NOTE_G4);
  delay(250);
  tone(buzzerpin,NOTE_C4);
  delay(500);
  noTone(buzzerpin);
}

void bancarottasound(){
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

void suonoregtag(){
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
  preparazione();
}

