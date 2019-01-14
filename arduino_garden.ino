#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"


#define PIN_1_MOTOR_A 4
#define PIN_2_MOTOR_A 5
#define PIN_1_MOTOR_B 6
#define PIN_2_MOTOR_B 7

#define TEMP_PIN 2

#define MOISTURE_PIN_1 A0
#define MOISTURE_PIN_2 A1

#define check_time 18000000

//#define check_time 10000

#define DRY 550
#define WET 200

RTC_DS1307 rtc;
OneWire wires (TEMP_PIN);
DallasTemperature sensors (&wires);


struct garden_sensors {
  int current_moisture_value_sensor_1;
  int current_moisture_value_sensor_2;
  float current_temp;
  int previous_moisture_value_sensor_1;
  int previous_moisture_value_sensor_2;
  float previous_temp;
};

struct garden_sensors garden;

unsigned long sleep_value = check_time;


void setup(void)
{
  Serial.begin(9600);
  init_rtc();
  init_sensors();
  init_garden();
}
void loop(void)
{
  update_garden_values();
  check_water();
  print_current_values();
  delay(sleep_value);
}


void start_motor(int motor_pin_1, int motor_pin_2) {
  digitalWrite(motor_pin_1, HIGH);
  digitalWrite(motor_pin_2, LOW);
}

void stop_motor(int motor_pin_1, int motor_pin_2) {
  digitalWrite(motor_pin_1, HIGH);
  digitalWrite(motor_pin_2, HIGH);
}

bool is_motor_running(int motor_pin_1, int motor_pin_2) {
  int value1 = digitalRead(motor_pin_1);
  int value2 = digitalRead(motor_pin_2);
  if ((value1 == HIGH && value2 == LOW) || (value1 == LOW && value2 == HIGH)) {
    return true;
  }
  return false;
}

void init_sensors() {
  sensors.begin();
}

void init_garden() {
  garden.current_moisture_value_sensor_1 = 0;
  garden.current_moisture_value_sensor_2 = 0;
  garden.current_temp = 0;
  garden.previous_moisture_value_sensor_1 = 0;
  garden.previous_moisture_value_sensor_2 = 0;
  garden.previous_temp = 0;
}

float get_temperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

void init_rtc() {
  while (!rtc.begin());
}

void adjust() {
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

DateTime get_date_time() {
  return rtc.now();
}

int get_year() {
  return rtc.now().year();
}

int get_month() {
  return rtc.now().month();
}

int get_day() {
  return rtc.now().day();
}

int get_hour() {
  return rtc.now().hour();
}

int get_minute() {
  return rtc.now().minute();
}

int get_second() {
  return rtc.now().second();
}

int get_moisture_value(int sensor) {
  return analogRead(sensor);
}


void check_water() {
  int current_month = get_month();
  int current_hour = get_hour();
  int water_hour_min = 0;
  int water_hour_max = 99;
  if (current_month >= 4 && current_month <= 10) {
    water_hour_min = 6;
    water_hour_max = 22;
  } else {
    water_hour_min = 8;
    water_hour_max = 20;
  }
  if (current_hour >= water_hour_min && current_hour <= water_hour_max) {
    if ((get_moisture_value(MOISTURE_PIN_1))>300) { //soil saturation value depends on the plant
      start_motor(PIN_1_MOTOR_A, PIN_2_MOTOR_A);
      delay(8000);
      stop_motor(PIN_1_MOTOR_A, PIN_2_MOTOR_A);
    }
    if ((get_moisture_value(MOISTURE_PIN_2))>300) { //soil saturation value depends on the plant
      start_motor(PIN_1_MOTOR_B, PIN_2_MOTOR_B);
      delay(8000);
      stop_motor(PIN_1_MOTOR_B, PIN_2_MOTOR_B);
    }
  }
}

void update_garden_values() {
  garden.previous_moisture_value_sensor_1 = garden.current_moisture_value_sensor_1;
  garden.previous_moisture_value_sensor_2 = garden.current_moisture_value_sensor_2;
  garden.previous_temp = garden.current_temp;

  garden.current_temp = get_temperature();
  garden.current_moisture_value_sensor_1 = get_moisture_value(MOISTURE_PIN_1);
  garden.current_moisture_value_sensor_2 = get_moisture_value(MOISTURE_PIN_2);
}

void print_current_values() {
  Serial.print("Moisture A0 Sensor: ");
  Serial.println(garden.current_moisture_value_sensor_1);
  Serial.print("Moisture A1 Sensor: ");
  Serial.println(garden.current_moisture_value_sensor_2);
  Serial.print("Temperature: ");
  Serial.println(garden.current_temp);
  DateTime now = get_date_time();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}
