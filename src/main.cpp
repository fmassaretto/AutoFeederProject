#include <Arduino.h>
#include <ArduinoJson.h>
#include <.env/WifiCredentials.cpp>
#include <LittleFS.h>
#include <../lib/mellodyMaker/MellodyMaker2.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <RTClib.h>
#include <Servo.h>
#include <Ultrasonic.h>
#include <LiquidCrystal.h>

const char *ssid = STASSID;
const char *password = STAPSK;

int LED = 2;
const int pinSensor = 17;
const int echoPin = 26;
const int trigPin = 25;
#define BUZZER_PIN D3
#define SERVO_PIN D4


int fisrtTimeScheduleHour = 0;
int fisrtTimeScheduleMinute = 0;
int fisrtTimeScheduleSecond = 0;

int secondTimeScheduleHour = 0;
int secondTimeScheduleMinute = 0;
int secondTimeScheduleSecond = 0;

int thirdTimeScheduleHour = 0;
int thirdTimeScheduleMinute = 0;
int thirdTimeScheduleSecond = 0;

long distancia;
long duracao;

Servo servo;
RTC_DS1307 rtc;
LiquidCrystal lcd (22, 21, 5, 18, 23, 19);
Ultrasonic ultra(trigPin, echoPin);


ESP8266WebServer server(80);

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

void openDoor()
{
  Serial.println("Opening door...");
  digitalWrite(SERVO_PIN, HIGH);
  delay(2000);
  digitalWrite(SERVO_PIN, LOW);
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
  digitalWrite(LED, 1);
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
  digitalWrite(LED, 0);
}

void distanceCalc() 
{
  digitalWrite(trigPin, LOW); //Pulsando o sensor ultrassônico 
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duracao = pulseIn(echoPin, HIGH);
  distancia = duracao / 58; //Calculando distancia 

void levelCheck()
{
  if (distancia >= 50 && distancia <= 60) 
  { 
    lcd.setCursor(0, 0);
    lcd.print("Cheio");
    delay(500);
  } 
  else if (distancia < 50 && distancia >= 30) 
  {
    lcd.setCursor(0, 0);
    lcd.print("Pela metade");
    delay(500);
  } 
  else if (distancia < 30 && distancia >= 10) 
  {
    lcd.setCursor(0, 0);
    lcd.print("Quase vazio");
    delay(500);
  } 
  else if (distancia < 10) 
  {
    lcd.setCursor(0, 0);
    lcd.print("Vazio");
    delay(500);
  }
}

void setup(void)
{
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  lcd.begin(16, 2);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(SERVO_PIN, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pinSensor, INPUT);
  Wire.begin(15, 2);
  if (! rtc.begin()) {
    Serial.println("RTC nao encontrado");
    while (1);
  }
  if (! rtc.isrunning()) 
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
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

  server.on(UriBraces("/quiz/result/{}"), HTTP_POST, handleQuiz);

  server.on("/schedule/", HTTP_POST, handleScheduler);

  server.on(UriBraces("/users/{}"), []()
            {
    String user = server.pathArg(0);
    server.send(200, "text/plain", "User: '" + user + "'"); });

  server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), []()
            {
    String user = server.pathArg(0);
    String device = server.pathArg(1);
    server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'"); });

  server.serveStatic("/", LittleFS, "/");

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  LittleFS.begin();
}

void loop(void)
{
  server.handleClient();
  void distanceCalc();
  void infraRed();
  void levelCheck();

  lcd.clear();
  DateTime now = rtc.now();

  if(pinSensor == LOW)
  {
    if(now.hour() == fisrtTimeScheduleHour && now.minute() == firstTimeScheduleMinute || now.hour() == secondTimeScheduleHour && now.minute() == secondTimeScheduleMinute || now.hour() == thirdTimeScheduleHour  && now.minute() == thirdTimeScheduleMinute)
    {
      servo.write(90);   
      delay(500);
    } 
  } 
  else
  {
      servo.write(0);
  }
}


