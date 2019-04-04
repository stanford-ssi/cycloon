/* Cycloon Modification of Team Fire Onboarding
 *  
 * Credit to Team Fire Onboarding for the original file, which was modified.
 * 
 * Updates
 * *CSV Format
 * *Log file
 * *Cleaned
 */
 
#include <Wire.h>
#include <SPI.h>
#include <TinyGPS.h>
#include <IridiumSBD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BNO055.h>

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


int seconds = 80;  // Seconds since last transmission; initialized to send 40 seconds after first initialization
int totalTime = 0;  // Total time in seconds since turning on
int minTransTime = 120; // Will try to send if greater than this and signal quality > 2
int maxTransTime = 240; // Will try to send if greater than this regardless of signal quality

static void smartdelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial1.available()) {
      gps.encode(Serial1.read());
    }
  } while (millis() - start < ms);
}

static void print_date(TinyGPS &gps) {
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE) {
    char tmp[] = "GPS NO SIGNAL";
    Serial.println(tmp);
  } else {
    char sz[32] = "";
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, (hour-7), minute, second);
    Serial.println(sz);
  }
  smartdelay(0);
}

void setup() {

  Serial.begin(38400);
  while (!Serial);

  //Initialize GPS port
  Serial1.begin(9600);
  
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
    Serial.println(tmp);
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED) {
      strcpy(tmp, "No modem detected: check wiring.");
      Serial.println(tmp);
    }
  } else {
    char tmp[] = "Modem initialized.";
    Serial.println(tmp);
  }

  char varNames[] = "timestamp,bmp_temp,pres,bmp_alt,flat,flon,altitude";
  Serial.println(varNames);


}

void loop() {
  
  delay(1000);

  // Getting data from BNO absolute orientation sensor
  sensors_event_t event;
  bno.getEvent(&event);
  float x = event.orientation.x;
  float y = event.orientation.y;
  float z = event.orientation.z;

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
  Serial.println(stars);
  print_date(gps);

  Serial.println(bmp_temp); // BMP Temperature
  Serial.println(pres);  // Pressure
  Serial.println(bmp_alt); // BMP Altitude (approximated)
  
  Serial.println(flat);  // GPS Latitude
  Serial.println(flon);  // GPS Longitude
  Serial.println(gps.f_altitude()); // GPS Altitude
  
  // Vital method for GPS; do not remove!
  smartdelay(1000);

  // Data to be sent: temp, pressure, BMP altitude, GPS latitude, GPS longitude, GPS altitude
  char buff[20] = "";
  char toSend[60] = "";
  dtostrf(bmp_temp, 5, 1, buff);
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
    if ((signalQuality > 2 || (signalQuality > 0 && seconds >= maxTransTime)) && seconds >= minTransTime) {
      char tmp[] = "Signal quality is currently: ";
      Serial.println(tmp);
      Serial.println(signalQuality);
      strcpy(tmp, "Time since last transmission is currently: ");
      Serial.println(tmp);
      Serial.println(seconds);
      strcpy(tmp, "Trying to send.");
      Serial.println(tmp);
      err = modem.sendSBDText(toSend);
      if (err == ISBD_SUCCESS) {
        strcpy(tmp, "Hey, it worked!");
        Serial.println(tmp);
        seconds = 0;
      }
    } else {
      char tmp[] = "Signal quality is currently: ";
      Serial.println(tmp);
      Serial.println(signalQuality);
      strcpy(tmp, "Time since last transmission is currently: ");
      Serial.println(tmp);
      Serial.println(seconds);
      strcpy(tmp, "Not trying to send.");
      Serial.println(tmp);
    }
  }
  
   delay(19000); // plus 1000 from smart delay = 20 seconds

  seconds += 20;
  totalTime += 20;

  if(totalTime >= 21600) {
    minTransTime = 3600;
    maxTransTime = 3600;
  }
}
