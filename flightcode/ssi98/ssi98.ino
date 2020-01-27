/* Cycloon Modification of Team Fire Onboarding
 *  
 * Credit to Team Fire Onboarding for the original file, which was modified.
 * 
 * Updates
 * *CSV Format
 * *Log file
 * *Cleaned
 */

// ADAPTED FROM SSI87.INO
 
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

int minTransTime = 0*60; // Will try to send if greater than this and signal quality > 2
int maxTransTime = 0*60; // Will try to send if greater than this regardless of signal quality
int secondsSince = minTransTime;  // Seconds since last transmission; initialized to send 0 seconds after first initialization
int droptime = 0;
int venttime = 0;

static void smartdelay(unsigned long ms);
static void print_date(TinyGPS &gps);
static void initializeRockBlock();
static void printData(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt);
static void tryRB(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt, uint8_t *rxbuf);
static void drop(uint8_t seconds) {
  
  Serial.print("Dropping for seconds: ");
  Serial.println(seconds);
  
  Serial.println("Starting motor");
  digitalWrite(23, HIGH);
  delay(1000 * (int) seconds);
  
  Serial.println("Stopping motor"); 
  digitalWrite(23, LOW);
  
}

static void heaterOn(){
  digitalWrite(3, HIGH);
}

static void heaterOff(){
  digitalWrite(3, LOW);
}

static void resetGPS(){
  Serial.print("Resetting GPS");
  digitalWrite(5, LOW);
  delay(1000 * (int) 5);
  digitalWrite(5, HIGH);
  return;
}

static void vent(uint8_t seconds) {
  Serial.print("Venting for seconds: ");
  Serial.println(seconds);
  Serial.println("Starting motor OPEN");
  analogWrite(21, 255);//open vent
  delay(250);
  analogWrite(20, 255);//start braking
  delay(50);
  Serial.println("Stopping motor OPEN");
  analogWrite(21, 0);//depower
  analogWrite(20, 0);


  delay(1000 * (int) seconds);

  Serial.println("Starting motor CLOSE");
  analogWrite(20, 255);
  delay(250);
  Serial.println("Stopping motor CLOSE");
  analogWrite(20, 180);

  
  return;
}

static void cutdown(uint8_t seconds)
{
  Serial.print("cutdown started");
  digitalWrite(4, HIGH);
  delay(seconds*1000);
  digitalWrite(4, LOW);
}

void setup() {
  
  delay(1000);
  Serial.begin(38400);
  
  delay(1000);
  
  // while (!Serial);
  Serial.println("Starting setup");
    
  //Initialize GPS port
  Serial1.begin(9600);
   Serial.println("GPS maybe ready");
  
  // Initialize bmp
  if(bme.begin()) {
    Serial.println("BMP ready");
  } else {
    Serial.println("BMP not ready");
  }
  
  // Initialize RockBlock
  IridiumSerial.begin(19200);
  Serial.println("Rockblock starting");

  // Initialize RockBlock modem
  initializeRockBlock();
  Serial.println("Rockblock ready");

  // Initialize ballast control pin
  pinMode(23, OUTPUT);
  drop(5);
  Serial.println("Ballast pin ready");

  // Initialize vent control pin
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  Serial.println("Vent pin ready");

  //Heater init
  pinMode(3, OUTPUT);

  //GPS Reset pin
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); //turn on GPS

  //Cutdown pin
  pinMode(4, OUTPUT);

  //analogWrite(20,255);//start forcing vent closed


  resetGPS();
  delay(30000);
}

void ventSequence(int t1, int t2, int t3, int wait)
{
  vent(t1);
  delay(wait);
  vent(t2);
  delay(wait);
  vent(t3);
  delay(wait);
}
void firstVent()
{
  vent(30);
  delay(30000);
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

  printData(bmp_temp, pres, bmp_alt, flat, flon, gps_alt);  
  uint8_t rxbuf[4] = {0, 0, 0, 0};
  tryRB(bmp_temp, pres, bmp_alt, flat, flon, gps_alt, rxbuf);
  if (rxbuf[0] > 0) {
    drop(rxbuf[0]);
    secondsSince += rxbuf[0];
    droptime += rxbuf[0];
  }
  if (rxbuf[1] > 0) {
    minTransTime = rxbuf[1]*60;
    maxTransTime = rxbuf[1]*60;
    Serial.print("Setting min and max time to seconds: ");
    Serial.println(rxbuf[1]*60);
  }
  if (rxbuf[2] > 0) {
    if (rxbuf[2] == (uint8_t) 1) heaterOn();
    else if (rxbuf[2] == (uint8_t) 2) heaterOff();
    else if (rxbuf[2] == (uint8_t) 3) resetGPS();
  }
  if (rxbuf[3] > 0) {
    cutdown(rxbuf[3]);
    secondsSince += rxbuf[3];
  }
  
  //ventSequence(10,30,60,000);
  firstVent();
  //delay(29000); // plus 1000 from smart delay = 30 seconds

  secondsSince += 30;
}


static void printData(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt) {
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
  Serial.print("venttime = ");
  Serial.println(venttime);
  Serial.print("mintime = ");
  Serial.println(minTransTime);
  Serial.print("maxtime = ");
  Serial.println(maxTransTime);
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


static void tryRB(float bmp_temp, float pres, float bmp_alt, float flat, float flon, float gps_alt, uint8_t *rxbuf) {
  // Data to be sent: temp, pressure, BMP altitude, GPS latitude, GPS longitude, GPS altitude, droptime
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
  itoa(droptime, buff, 10);
  strcat(toSend, buff);
  strcat(toSend, ",");
  itoa(venttime, buff, 10);
  strcat(toSend, buff);
  Serial.print("String to send:");
  Serial.println(toSend);

  // Sending through RockBlock
  int signalQuality;

  size_t rxbufsize = 4;

  if (secondsSince >= minTransTime) {

    int err = modem.getSignalQuality(signalQuality);
    if (err == ISBD_SUCCESS) {
    
    
      // Only sends if at least minTransTime has passed since last transmission, and signal quality is 3 and above
      // or if at least maxTransTime has passed since last transmission and signal quality is not 0
      Serial.print("Signal quality is currently: ");
      Serial.println(signalQuality);
      Serial.print("Time since last transmission is currently: ");
      Serial.println(secondsSince);
    
      if (signalQuality > 2 || (signalQuality > 0 && secondsSince >= maxTransTime)) {
        Serial.println("Trying to send.");

        //err = modem.sendReceiveSBDBinary(sendArray, sizeof(sendArray), rxbuf, rxbufsize);
        err = modem.sendReceiveSBDText(toSend, rxbuf, rxbufsize);
        //err = modem.sendSBDText(toSend);
        if (err == ISBD_SUCCESS) {
          Serial.println("Hey, it worked!");
          Serial.println(toSend);
          Serial.print("Received rxbuf0: ");
          Serial.println(rxbuf[0]);
          Serial.print("Received rxbuf1: ");
          Serial.println(rxbuf[1]);
          Serial.print("Received rxbuf2: ");
          Serial.println(rxbuf[2]);
          Serial.print("Received rxbuf3: ");
          Serial.println(rxbuf[3]);
        
          secondsSince = 0;
        } else {
          Serial.println("Could not send");
          secondsSince += 30;
        }
      } else {
        Serial.println("Not trying to send.");
      }
    } else {
      Serial.println("Modem error");
    }    
  } else {
    Serial.print("Time since last transmission is currently: ");
    Serial.println(secondsSince);
    Serial.println("Not trying to send.");
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
    Serial.println("GPS NO SIGNAL");
  } else {
    char sz[32] = "";
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, (hour-7), minute, second);
    Serial.println(sz);
  }
  smartdelay(0);
}
