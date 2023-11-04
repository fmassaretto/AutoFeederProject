#include <Arduino.h>
#include <ArduinoJson.h>
#include <.env/WifiCredentials.cpp>
#include <LittleFS.h>
#include <../lib/mellodyMaker/MellodyMaker2.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <SPI.h>
#include <RTClib.h>
#include <Wire.h>
#include <Ultrasonic.h>
#include <LiquidCrystal.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

const char *mqtt_server = "192.168.0.194";
const char *ssid = STASSID;
const char *password = STAPSK;

#define SDA_PIN D2
#define SCL_PIN D1
#define TRIG_PIN D6
#define ECHO_PIN D7
#define BUZZER_PIN D3
#define SERVO_PIN D8
#define IR_PIN D5

int fisrtTimeScheduleHour = -1;
int fisrtTimeScheduleMinute = -1;
int fisrtTimeScheduleSecond = -1;

int secondTimeScheduleHour = -1;
int secondTimeScheduleMinute = -1;
int secondTimeScheduleSecond = -1;

int thirdTimeScheduleHour = -1;
int thirdTimeScheduleMinute = -1;
int thirdTimeScheduleSecond = -1;

RTC_DS1307 rtc;
// RtcDS1307<TwoWire> Rtc(Wire);
Servo myservo;
LiquidCrystal lcd(22, 21, 4, 18, 23, 19);
Ultrasonic ultra(TRIG_PIN, ECHO_PIN);

ESP8266WebServer server(80);

bool adjustOneTime = false;
bool isOffline = false;
int irDetection = HIGH; // no obstacle

enum QUIZ_RESULT
{
  WON,
  LOSE
};

enum SCHEDULE_TIME_PRESET
{
  FIRST,
  SECOND,
  THIRD
};

void lcdDisplay(int col, int row, String text)
{
  lcd.setCursor(col, row);
  lcd.print(text);
}

int tankLevelDistance()
{
  const int distance = ultra.read();

  Serial.print("Distance in CM: ");
  Serial.println(distance);

  delay(50);
  return distance;
}

String tankLevel()
{
  String levelDescription = "";

  if (tankLevelDistance() <= 6)
  {
    levelDescription = "Cheio";
  }
  else if (tankLevelDistance() > 6 && tankLevelDistance() <= 10)
  {
    levelDescription = "Pela metade";
  }
  else
  {
    levelDescription = "Vazio";
  }

  lcdDisplay(0, 0, levelDescription);

  return levelDescription;
}

bool isBowlEmpty()
{
  irDetection = digitalRead(IR_PIN);
  if (irDetection == LOW)
  {
    Serial.print("Pote está cheio!\n");
    delay(50);
    return false;
  }

  Serial.print("Pote VAZIO!\n");
  delay(50);
  return true;
}

void openDoor()
{
  Serial.println("Opening door...");
  myservo.write(90);
  delay(100);
  myservo.write(150);
}

void playSong(QUIZ_RESULT result)
{
  if (result == QUIZ_RESULT::WON)
  {
    playWonMellody(BUZZER_PIN);
  }
  else if (result == QUIZ_RESULT::LOSE)
  {
    playLoseMellody(BUZZER_PIN);
  }
}

void quizWon()
{
  playSong(QUIZ_RESULT::WON);
  openDoor();
}

void quizLose()
{
  playSong(QUIZ_RESULT::LOSE);
}

void setFirstScheduleTime(String key, int value)
{
  if (key == "hour0")
  {
    fisrtTimeScheduleHour = value;
  }
  if (key == "minute0")
  {
    fisrtTimeScheduleMinute = value;
  }
  if (key == "second0")
  {
    fisrtTimeScheduleSecond = value;
  }
}

void setSecondScheduleTime(String key, int value)
{
  if (key == "hour1")
  {
    secondTimeScheduleHour = value;
  }
  if (key == "minute1")
  {
    secondTimeScheduleMinute = value;
  }
  if (key == "second1")
  {
    secondTimeScheduleSecond = value;
  }
}

void setThirdScheduleTime(String key, int value)
{
  if (key == "hour2")
  {
    thirdTimeScheduleHour = value;
  }
  if (key == "minute2")
  {
    thirdTimeScheduleMinute = value;
  }
  if (key == "second2")
  {
    thirdTimeScheduleSecond = value;
  }
}

void setScheduleTimeByPreset(SCHEDULE_TIME_PRESET preset, String key, int value)
{
  Serial.print("SCHEDULE_TIME_PRESET:");
  Serial.println(preset);
  Serial.print("key:");
  Serial.println(key);
  Serial.print("value:");
  Serial.println(value);

  if (preset == SCHEDULE_TIME_PRESET::FIRST)
  {
    setFirstScheduleTime(key, value);
  }
  else if (preset == SCHEDULE_TIME_PRESET::SECOND)
  {
    setSecondScheduleTime(key, value);
  }
  else if (preset == SCHEDULE_TIME_PRESET::FIRST)
  {
    setThirdScheduleTime(key, value);
  }
}

void schedulerDeserialize(String input)
{
  StaticJsonDocument<768> doc;
  Serial.println("input: ");
  Serial.println(input);

  DeserializationError error = deserializeJson(doc, input);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  for (JsonObject item : doc.as<JsonArray>())
  {
    const String key = item["key"];  // "hour0", "minute0", "second0", "hour1", "minute1", "second1", "hour2", ...
    const int value = item["value"]; // "24", "59", "59", "24", "59", "59", "24", "59", "59"

    if (key.endsWith("0"))
    {
      setScheduleTimeByPreset(SCHEDULE_TIME_PRESET::FIRST, key, value);
    }
    else if (key.endsWith("1"))
    {
      setScheduleTimeByPreset(SCHEDULE_TIME_PRESET::SECOND, key, value);
    }
    else if (key.endsWith("2"))
    {
      setScheduleTimeByPreset(SCHEDULE_TIME_PRESET::THIRD, key, value);
    }
  }
}

void schedulerLogic()
{
  if (isOffline)
  {
    // Display that it is offline
    // run code to open the door each 5min
    // Display the crrent time
    // display next time that will open the doorç
    // display if the bowl is empty
    // VErify if bowl is empty so can open the door and release food
  }
  else
  {
    // Display that it is connected and show the IP
    // VErify if the bowl is empty and if the 3 scheduled time
  }
}

void handleTankStatus()
{
  server.send(200, "text/plain", tankLevel());
}

void handleBowlStatus()
{
  server.send(200, "text/plain", (isBowlEmpty() ? "VAZIO" : "CHEIO"));
}

void handleScheduler()
{
  if (server.hasArg("plain") == false)
  { // Check if body received
    server.send(204, "application/json", "{\"Error:\": \"Body not received\"}");
    return;
  }

  String message = server.arg("plain");

  schedulerDeserialize(message);

  Serial.println(message);
  server.send(200, "application/json", message);
}

void handleQuiz()
{
  String result = server.pathArg(0);
  Serial.println("Quiz " + result);
  if (result == "won")
  {
    quizWon();
  }
  else if (result == "lose")
  {
    quizLose();
  }
  server.send(200);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void setupRTC()
{
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    int tries = 0;
    while (tries != 150)
    {
      delay(10);
      tries++;
    }
  }

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void setupServer()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int wifiConnectionTries = 30;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

    if (wifiConnectionTries == 0)
    {
      isOffline = true;
      break;
    }
    wifiConnectionTries--;
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/bowl-status/", HTTP_GET, handleBowlStatus);

  server.on("/tank-status/", HTTP_GET, handleTankStatus);

  server.on(UriBraces("/quiz/result/{}"), HTTP_POST, handleQuiz);

  server.on("/schedule/", HTTP_POST, handleScheduler);

  server.serveStatic("/", LittleFS, "/");

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void setup(void)
{
  Serial.begin(115200);
  delay(3000);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);

#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(SERVO_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(16, 2);
  myservo.attach(SERVO_PIN);
  // myservo.write(90);
  Serial.print("Servo is attached: ");
  Serial.println(myservo.attached() == true ? "YES" : "NO");

  setupRTC();
  setupServer();

  LittleFS.begin();
}

DateTime a()
{
  DateTime dt = DateTime(F(__DATE__), F(__TIME__));
  ulong lastMillis = millis();
  uint32_t lastUnix = dt.unixtime();

  uint32_t elapsedSeconds = (millis() - lastMillis) / 1000;
  lastMillis += elapsedSeconds * 1000;
  lastUnix += elapsedSeconds;
  return lastUnix;
}

void loop(void)
{
  server.handleClient();
  schedulerLogic();
  isBowlEmpty();

  // DateTime now = rtc.now(); // Faz a leitura da data e hora atual, guarda na função now 
  // int rtcHour = now.hour();
  // int rtcMinute = now.minute();
  // int rtcSecond = now.second();

  // Serial.println(String(now.hour()));
  // Serial.printf("%d/%d/%d - %i:%i:%i", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  // Serial.println();

  // DateTime now = rtc.now();

  // Serial.print(now.year(), DEC);
  // Serial.print('/');
  // Serial.print(now.month(), DEC);
  // Serial.print('/');
  // Serial.print(now.day(), DEC);
  // Serial.print(" (");
  // Serial.print(") ");
  // Serial.print(now.hour(), DEC);
  // Serial.print(':');
  // Serial.print(now.minute(), DEC);
  // Serial.print(':');
  // Serial.print(now.second(), DEC);
  // Serial.println();

  // Serial.print(" since midnight 1/1/1970 = ");
  // Serial.print(now.unixtime());
  // Serial.print("s = ");
  // Serial.print(now.unixtime() / 86400L);
  // Serial.println("d");

  // // calculate a date which is 7 days, 12 hours, 30 minutes, and 6 seconds into the future
  // DateTime future(now + TimeSpan(7, 12, 30, 6));

  // Serial.print(" now + 7d + 12h + 30m + 6s: ");
  // Serial.print(future.year(), DEC);
  // Serial.print('/');
  // Serial.print(future.month(), DEC);
  // Serial.print('/');
  // Serial.print(future.day(), DEC);
  // Serial.print(' ');
  // Serial.print(future.hour(), DEC);
  // Serial.print(':');
  // Serial.print(future.minute(), DEC);
  // Serial.print(':');
  // Serial.print(future.second(), DEC);
  // Serial.println();

  // Serial.println();
  // delay(3000);
  // void distanceCalc();
  // void levelCheck();

  // DateTime now = rtc.now();

  // if (pinSensor == LOW)
  // {
  //   if (now.hour() == fisrtTimeScheduleHour && now.minute() == fisrtTimeScheduleMinute || now.hour() == secondTimeScheduleHour && now.minute() == secondTimeScheduleMinute || now.hour() == thirdTimeScheduleHour && now.minute() == thirdTimeScheduleMinute)
  //   {
  //     servo.write(90);
  //     delay(500);
  //   }
  // }
  // else
  // {
  //   servo.write(0);
  // }
}
