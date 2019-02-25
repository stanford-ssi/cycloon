/* Cycloon Modification of Team Fire Onboarding
 * 
 * Updates
 * *CSV Format
 * *Log file
 * *Cleaned
 */
 
#include <Wire.h>
#include <SPI.h>
// #include <SdFat.h>
#include <TinyGPS.h>
#include <IridiumSBD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MAX31855.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// SD Card
/* SdFat sd;
SdFile file;
File myFile; */

// GPS
#define gpsPort Serial1
TinyGPS gps;

// Iridium SBD
#define IridiumSerial Serial3
IridiumSBD modem(IridiumSerial);

// BMP
Adafruit_BMP280 bme;

// BNO
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// Thermocouple
#define MAXDO   20
#define MAXCS   16
#define MAXCLK  17
// Initialize thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// const int SD_CS = 9; // Chip select for SD

/* const int eggPin = 22;  // Egg cutdown
const int egg_alt = 2250; // Altitude at which egg cutsdown
bool eggDropped; // true if egg has dropped */

/* const int mainPin = 23; // Main pin cutdown
const int main_alt = 22000; // Altitude at which main cutsdown
bool mainDropped; // true if main has dropped */

int seconds = 80;  // Seconds since last transmission; initialized to send 40 seconds after first initialization

/* void writeToFile(char filename[], float writeLine, int prec) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(writeLine, prec);
    myFile.close();
  }
} */

/* void writeToFileNoLine(char filename[], float writeLine, int prec) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(writeLine, prec);
    myFile.print(",");
    myFile.close();
  }
} */

/* void writeToFile(char filename[], double writeLine) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(writeLine);
    myFile.close();
  }
} */

/* void writeToFileNoLine(char filename[], double writeLine) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(writeLine);
    myFile.print(",");
    myFile.close();
  }
} */

/* void writeToFile(char filename[], char writeLine[]) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(writeLine);
    myFile.close();
  }
} */

/* void writeToFileNoLine(char filename[], char writeLine[]) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(writeLine);
    myFile.print(",");
    myFile.close();
  }
} */

/* void writeToFile(char filename[], int writeLine) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(writeLine);
    myFile.close();
  }
} */

/* void writeToFileNoLine(char filename[], int writeLine) {
  // open the file
  myFile = sd.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(writeLine);
    myFile.print(",");
    myFile.close();
  }
}

bool dropEgg(float bmp_alt, bool drop, char filename[]) {
  if(bmp_alt >= egg_alt && !drop) {
    digitalWrite(eggPin, HIGH);
    delay(70000);
    char tmp[] = "Egg dropped!";
    writeToFile(filename, tmp);
    drop = true;
  } else {
    digitalWrite(eggPin, LOW);
    drop = false;
  }
  return drop;
} */

/* bool dropMain(float bmp_alt, bool drop, char filename[]) {
  if(bmp_alt >= main_alt && !drop) {
    digitalWrite(mainPin, HIGH);
    delay(70000);
    char tmp[] = "Main dropped!";
    writeToFile(filename, tmp);
    drop = true;
  } else {
    digitalWrite(mainPin, LOW);
    drop = false;
  }
  return drop;
} */

static void smartdelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial1.available()) {
      gps.encode(Serial1.read());
    }
  } while (millis() - start < ms);
}

static void print_date(TinyGPS &gps, char filename[]) {
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE) {
    char tmp[] = "GPS NO SIGNAL";
    writeToFileNoLine(filename, tmp);
  } else {
    char sz[32] = "";
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, (hour-7), minute, second);
    writeToFileNoLine(filename, sz);
  }
  smartdelay(0);
}

void setup() {
/*  pinMode(eggPin, OUTPUT);
  digitalWrite(eggPin, LOW);
  pinMode(mainPin, OUTPUT);
  digitalWrite(mainPin, LOW); */
  
  char master[] = "data.csv"; // filename
  char logger[] = "log.txt";

  // Initialize GPS port
  Serial1.begin(9600);

 /* // Initialize SD port
  SPI.setMOSI(11);
  SPI.setMISO(12);
  SPI.setSCK(14);
  
  // Initialize SD card
  sd.begin(SD_CS, SPI_QUARTER_SPEED); */

  // Initialize bmp
  bme.begin();

  // Initialize bno
  bno.begin();
  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);
  
  // Initialize RockBlock
  IridiumSerial.begin(19200);

  // Initialize RockBlock modem
  int err;
  err = modem.begin();
  if (err != ISBD_SUCCESS) {
    char tmp[] = "Begin failed: error ";
    writeToFileNoLine(logger, tmp);
    writeToFile(logger, err);
    if (err == ISBD_NO_MODEM_DETECTED) {
      strcpy(tmp, "No modem detected: check wiring.");
      writeToFile(logger, tmp);
    }
  } else {
    char tmp[] = "Modem initialized.";
    writeToFile(logger, tmp);
  }

/*
  eggDropped = false;
  mainDropped = false; */

  char varNames[] = "timestamp,bmp_temp,pres,bmp_alt,flat,flon,altitude,x,y,z,int_temp,tc_temp";
  writeToFile(master, varNames);
}

void loop() {
  char master[] = "data.csv"; // Filename
  char logger[] = "log.txt";

  // Getting data from absolute orientation sensor
  sensors_event_t event;
  bno.getEvent(&event);
  float x = event.orientation.x;
  float y = event.orientation.y;
  float z = event.orientation.z;

  // Getting data from thermocouple
  double int_temp = thermocouple.readInternal();
  double tc_temp = thermocouple.readCelsius();
  
  // Getting data from BMP
  float bmp_temp = bme.readTemperature();
  float pres = bme.readPressure();
  float bmp_alt = bme.readAltitude();

  // Getting data from GPS
  float flat, flon;
  unsigned long age = 0;
  gps.f_get_position(&flat, &flon, &age);
  float gps_alt = gps.f_altitude();
  char stars[] = "****";
  writeToFile(logger, stars);
  print_date(gps, master);
  print_date(gps, logger);

  writeToFileNoLine(master, bmp_temp, 2); // BMP Temperature
  writeToFileNoLine(master, pres, 2);  // Pressure
  writeToFileNoLine(master, bmp_alt, 2); // BMP Altitude (approximated)
  
  writeToFileNoLine(master, flat, 10);  // GPS Latitude
  writeToFileNoLine(master, flon, 10);  // GPS Longitude
  writeToFileNoLine(master, gps.f_altitude(), 2); // GPS Altitude

  writeToFileNoLine(master, x, 2); // x-orientation
  writeToFileNoLine(master, y, 2); // y-orientation
  writeToFileNoLine(master, z, 2); // z-orientation

  writeToFileNoLine(master, int_temp); // Thermocouple int temp
  writeToFile(master, tc_temp); // Thermocouple temp

  // Vital method for GPS; do not remove!
  smartdelay(1000);

  // Egg drop
  // eggDropped = dropEgg(bmp_alt, eggDropped, master);

  // Data to be sent: temp, pressure, BMP altitude, GPS latitude, GPS longitude, GPS altitude
  char buff[20] = "";
  char toSend[60] = "";
  dtostrf(bmp_temp, 5, 1, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(tc_temp, 5, 1, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(pres, 6, 0, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(bmp_alt, 6, 0, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(flat, 10, 5, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(flon, 10, 5, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(gps_alt, 7, 1, buff);
  strcat(toSend, buff);

  // Sending through RockBlock
  int signalQuality;
  int err = modem.getSignalQuality(signalQuality);
  if (err == ISBD_SUCCESS) {
    // Only sends if at least 2 minutes has passed since last transmission, and signal quality is 3 and above
    // or if at least 4 minutes has passed since last transmission and signal quality is not 0
    if ((signalQuality > 2 || (signalQuality > 0 && seconds >= 240)) && seconds >= 120) {
      char tmp[] = "Signal quality is currently: ";
      writeToFileNoLine(logger, tmp);
      writeToFile(logger, signalQuality);
      strcpy(tmp, "Time since last transmission is currently: ");
      writeToFileNoLine(logger, tmp);
      writeToFile(logger, seconds);
      strcpy(tmp, "Trying to send.");
      writeToFile(logger, tmp);
      err = modem.sendSBDText(toSend);
      if (err == ISBD_SUCCESS) {
        strcpy(tmp, "Hey, it worked!");
        writeToFile(logger, tmp);
        seconds = 0;
      }
    } else {
      char tmp[] = "Signal quality is currently: ";
      writeToFileNoLine(logger, tmp);
      writeToFile(logger, signalQuality);
      strcpy(tmp, "Time since last transmission is currently: ");
      writeToFileNoLine(logger, tmp);
      writeToFile(logger, seconds);
      strcpy(tmp, "Not trying to send.");
      writeToFile(logger, tmp);
    }
  }
  
  delay(19000); // plus 1000 from smart delay = 20 seconds

  // Drop main
  mainDropped = dropMain(bmp_alt, mainDropped, master);
  
  seconds += 20;
}
