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

// GPS
#define gpsPort Serial1
TinyGPS gps;

// Iridium SBD
#define IridiumSerial Serial3
IridiumSBD modem(IridiumSerial);

// BMP
 Adafruit_BMP280 bme;

int minTransTime = 0; // Will try to send if greater than this and signal quality > 2
int maxTransTime = 0; // Will try to send if greater than this regardless of signal quality
int secondsSince = minTransTime;  // Seconds since last transmission; initialized to send 0 seconds after first initialization
int droptime = 0;

static void smartdelay(unsigned long ms);
static void print_date(TinyGPS &gps);
static void initializeRockBlock();
static void printData(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt, int droptime);
static void tryRB(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt, int droptime, uint8_t *rxbuf);
static void drop(uint8_t seconds) {return;}

void setup() {
  
  Serial.begin(38400);
  while (!Serial);
  Serial.println("Starting setup");
    
  //Initialize GPS port
  Serial1.begin(9600);
  Serial.println("GPS ready");
  
  // Initialize bmp
  bme.begin();
  Serial.println("BMP ready");
  
  // Initialize RockBlock
  IridiumSerial.begin(19200);
  Serial.println("Rockblock starting");

  // Initialize RockBlock modem
  initializeRockBlock();
  Serial.println("Rockblock ready");

}

// Every 30 seconds
void loop() {

  // Getting data from BMP
  float bmp_temp = bme.readTemperature();
  float pres = bme.readPressure();
  float bmp_alt = bme.readAltitude();

  // Getting data from GPS
  float flat, flon;
  unsigned long age = 0;
  gps.f_get_position(&flat, &flon, &age);
  float gps_alt = gps.f_altitude();
  
  // Vital method for GPS; do not remove!
  smartdelay(1000);
  char stars[] = "****";
  Serial.println(stars);
  print_date(gps);

  printData(bmp_temp, pres, bmp_alt, flat, flon, gps_alt, droptime);  
  uint8_t rxbuf[1] = {0};
  tryRB(bmp_temp, pres, bmp_alt, flat, flon, gps_alt, droptime, rxbuf);
  if (rxbuf[0] > 0) {
    drop(rxbuf[0]);
    secondsSince += rxbuf[0];
    droptime += rxbuf[0];
  }
  
  delay(29000); // plus 1000 from smart delay = 30 seconds

  secondsSince += 30;
}


static void printData(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt, int droptime) {
  Serial.print("bmp temperature = ");
  Serial.println(bmp_temp); // BMP Temperature
  Serial.print("bmp pressure = ");
  Serial.println(pres);  // Pressure
  Serial.print("bmp altitude = ");
  Serial.println(bmp_alt); // BMP Altitude (approximated)

  Serial.print("gps lat = ");
  Serial.println(flat);  // GPS Latitude
  Serial.print("gps lon = ");
  Serial.println(flon);  // GPS Longitude
  Serial.print("gps alt = ");
  Serial.println(gps_alt); // GPS Altitude
  Serial.print("droptime = ");
  Serial.println(droptime);
}

static void initializeRockBlock() {
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
}


static void tryRB(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt, int droptime, uint8_t *rxbuf) {
  // Data to be sent: temp, pressure, BMP altitude, GPS latitude, GPS longitude, GPS altitude
  char buff[20] = "";
  char toSend[49] = "";
  toSend[48] = '\0';
  dtostrf(bmp_temp, 4, 1, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(pres, 6, 0, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(bmp_alt, 5 , 0, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(flat, 8, 3, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(flon, 8, 3, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf(gps_alt, 5, 0, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  dtostrf( (float) droptime, 4, 0, buff);
  strcat(toSend, buff);
  strcat(toSend, ",");
  Serial.print("String to send:");
  Serial.println(toSend);
  // Sending through RockBlock
  int signalQuality;

  size_t rxbufsize=1;
  int err = modem.getSignalQuality(signalQuality);
  if (err == ISBD_SUCCESS) {
    // Only sends if at least minTransTime has passed since last transmission, and signal quality is 3 and above
    // or if at least maxTransTime has passed since last transmission and signal quality is not 0
    char tmp[] = "Signal quality is currently:                      ";
    Serial.print(tmp);
    Serial.println(signalQuality);
    strcpy(tmp, "Time since last transmission is currently: ");
    Serial.print(tmp);
    Serial.println(secondsSince);
    
    if (secondsSince >= minTransTime && (signalQuality > 2 || (signalQuality > 0 && secondsSince >= maxTransTime))) {
      strcpy(tmp, "Trying to send.");
      Serial.println(tmp);

      err = modem.sendReceiveSBDText(toSend,  rxbuf, rxbufsize);
      //err = modem.sendSBDText(toSend);
      if (err == ISBD_SUCCESS) {
        strcpy(tmp, "Hey, it worked!");
        Serial.println(tmp);
        Serial.println(toSend);
        Serial.print("Received value: ");
        Serial.println(rxbuf[0]);
        secondsSince = 0;
      } else {
        Serial.println("Could not send");
      }
    } else {
      strcpy(tmp, "Not trying to send.");
      Serial.println(tmp);
    }
  } else {
    Serial.println("Modem error");
  }
}
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
