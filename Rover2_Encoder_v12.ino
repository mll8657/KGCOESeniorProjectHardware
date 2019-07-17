/* 
Demonstrates use of rotary encoder for motor
direction and distance.
*/

#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

uint32_t syncTime = 0; // time of last sync()
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card

File logfile;

//#define Lcha 9
//#define Rcha 8
#define Lcha 7    // arduino motor shield
#define Rcha 6  // arduino motor shield

int prev_Lmeas=0;
int prev_Rmeas=0;
int Lmeas=0;
int Rmeas=0;
volatile int Lcount = 0; // universal count
volatile int Rcount = 0; // universal count
float Rhz;
float Rtime=0;
float prev_Rtime=0;
long desired_dt=500000;    // [mus] can be used to fix the sample time (w/o interupts)
long previousTime=0;
float currentTime;

void setup() { 
  pinMode(Lcha, INPUT);
  pinMode(Rcha, INPUT);
  
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  Serial.print("Time");
  Serial.print("\t");                   
  Serial.print("Left Encoder");
  Serial.print("\t");                   
  Serial.println("Right Encoder");
  logfile.println("test_device,pizza_time");
  logfile.println("time,left_rotations,right_rotations,right_rotation_rate");
  logfile.println("milliseconds,rotations,rotations,hertz");
  }

void loop() {

  static unsigned long timer = 0;                //print manager timer

  // make a string for assembling the data to log:
  String dataString = "";

/*  digitalWrite(4, LOW); //Establishes forward direction of Channel A - Right Motor
  analogWrite(5, 100);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(7, HIGH); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(6, 100);   //Spins the motor on Channel B at full speed  - Left Motor
*/

if (millis()<2000){  
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A - Right Motor
  analogWrite(3, 100);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(13, LOW); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(11, 100);   //Spins the motor on Channel B at full speed  - Left Motor
}

if(millis()>2000 && millis()<5000){
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A - Right Motor
  analogWrite(3, 75);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(13, LOW); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(11, 175);   //Spins the motor on Channel B at full speed  - Left Motor
}
if(millis()>5000 && millis()<7000){
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A - Right Motor
  analogWrite(3, 100);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(13, LOW); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(11, 100);   //Spins the motor on Channel B at full speed  - Left Motor
}
if(millis()>7000 && millis()<10000){
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A - Right Motor
  analogWrite(3, 175);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(13, LOW); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(11, 75);   //Spins the motor on Channel B at full speed  - Left Motor
}
if(millis()>10000 && millis()<12000){
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A - Right Motor
  analogWrite(3, 100);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(13, LOW); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(11, 100);   //Spins the motor on Channel B at full speed  - Left Motor
}

if(millis()>12000){
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A - Right Motor
  analogWrite(3, 0);   //Spins the motor on Channel A at full speed  - Right Motor
  digitalWrite(13, LOW); //Establishes forward direction of Channel A  - Left Motor
  analogWrite(11, 0);   //Spins the motor on Channel B at full speed  - Left Motor
}
 Lmeas=digitalRead(Lcha);
  Rmeas=digitalRead(Rcha);
  
  if(Lmeas-prev_Lmeas > 0.5){
    Lcount++;
  }
  if(Rmeas-prev_Rmeas > 0.5){
    Rtime=millis();
    Rhz=10.0*(Rtime-prev_Rtime)/1000.0;
    Rcount++;
    prev_Rtime=Rtime;
  }
  
  if(millis() - timer > 100){
    Serial.print(millis()/1000.0,6);
    Serial.print("\t");                   
    Serial.print(Lcount);
    Serial.print("\t");                   
    Serial.print(Rcount);
    Serial.print("\t");                   
    Serial.println(Rhz,4);
    timer = millis();

    dataString += timer;
    dataString += ",";
    dataString += Lcount/10.0;
    dataString += ",";
    dataString += Rcount/10.0;
    dataString += ",";
    dataString += Rhz;

    // if the file is available, write to it:
    if (logfile) {
      logfile.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening LOGGERXX.csv");
    }
  }
  
  prev_Lmeas=Lmeas;  
  prev_Rmeas=Rmeas;

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  logfile.flush();
    
 } // end loop
