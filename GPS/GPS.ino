#include <LGPS.h>
#include <LWiFi.h>

#define GPS_ON 0
#define WIFI_SCAN 1

gpsSentenceInfoStruct info;
char buff[256];
 int i;
 
static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}


void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  double latitude;
  double longitude;
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(30000);  
  pinMode(13, OUTPUT);
  while (i < 10)
  {
  digitalWrite(13, HIGH); // LED off  
  delay(1000);
  digitalWrite(13, LOW); // LED off
  delay(1000);  
  i++;
  }
  delay(10000);  
#if GPS_ON    
  Serial.println("LGPS Power on, and waiting ..."); 
  LGPS.powerOn();
  delay(10000); 
#endif  
#if WIFI_SCAN
  Serial.println("Begin Wifi ..."); 
    LWiFi.begin();
#endif  
  i=0;    
}

void loop() {
#if GPS_ON  
  if(i >= 20)
  {
      LGPS.powerOn();
      i=0;
  }
  i++;   
  // put your main code here, to run repeatedly:
  //Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  //Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);
  if( i == 10)
  {
      LGPS.powerOff();  
  }
#endif

#if WIFI_SCAN
  if(i >= 20)
  {
    LWiFi.begin();
    i=0;
  }
  i++; 
  int nearbyAccessPointCount = LWiFi.scanNetworks();
  sprintf(buff, "Access Point List %d", nearbyAccessPointCount); 
  Serial.println(buff); 
  if( i == 10)
  {
     LWiFi.end();  
  }
#endif
  delay(60000);  
}
