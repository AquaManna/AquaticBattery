#include "LCD4884.h"

#define MENU_X	5		// 0-83
#define MENU_Y	1		// 0-5

const int LED_VOLTAGE_PIN = 1; // Old: 97
const int PROBE_READ_PIN = 2; // Old: 98
const int OUTPUT_VOLTAGE_PIN = 3;
const int LIM_SWITCH_PROBE_PIN = 10;
const int LIM_SWITCH_READ_PIN = 4;
const int POWER_SOURCE_PIN = 8; // Old: 50
const int DRIVE_PUMP_PIN = 9; // Old: 51
const int VALVE_PIN = 12; // Old: 52
const int PROBE_PIN = 13; // Old: 49


int LED_voltage;
int output_voltage;
int probe_voltage;
int probe_lim_switch_val;
char num[10];

// the setup routine runs once when you press reset:
void setup() 
{
  //pinMode(PUMP_TRANSISTOR_PIN, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(POWER_SOURCE_PIN, OUTPUT);
  digitalWrite(POWER_SOURCE_PIN, LOW);
  pinMode(DRIVE_PUMP_PIN, OUTPUT);
  digitalWrite(DRIVE_PUMP_PIN, LOW);
  pinMode(VALVE_PIN, OUTPUT);
  digitalWrite(VALVE_PIN, HIGH);
  pinMode(LIM_SWITCH_PROBE_PIN, OUTPUT);
  digitalWrite(LIM_SWITCH_PROBE_PIN, LOW);
  
  // Display initialization
  lcd.LCD_init();
  lcd.LCD_clear();
  //menu initialization
  init_MENU();
}

void init_MENU(void)
{
  byte i;
  lcd.LCD_clear();
  lcd.LCD_write_string(MENU_X + 11, MENU_Y, "V: ", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X + 25, MENU_Y, "None      ", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, MENU_Y + 1, "Switch:", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Pump: ", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, MENU_Y + 3, "Valve: ", MENU_NORMAL);
}

// the loop routine runs over and over again forever:
void loop() 
{
  //LED_voltage = 0;
  // read the input on analog pin 0:
  LED_voltage = analogRead(LED_VOLTAGE_PIN);//LED_VOLTAGE_PIN);
  output_voltage = analogRead(OUTPUT_VOLTAGE_PIN);
  output_voltage = output_voltage / 100;
  
  delay(10);
  lcd.LCD_write_string(MENU_X, MENU_Y, itoa(output_voltage, num, 10), MENU_NORMAL);
  lcd.LCD_write_string(MENU_X + 6, MENU_Y, " ", MENU_NORMAL);  
  
  if (LED_voltage < 200)
  {
    digitalWrite(POWER_SOURCE_PIN, LOW);
    pinMode(PROBE_PIN, OUTPUT);
    digitalWrite(PROBE_PIN, HIGH);
    delay(10);
    probe_voltage = analogRead(PROBE_READ_PIN);
    pinMode(PROBE_PIN, INPUT);
    
    if (probe_voltage > 10)
    {
      // Valve open
      digitalWrite(VALVE_PIN, LOW);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 3, "OPEN  ", MENU_NORMAL);
      // LED is on.
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 1, "ON ", MENU_NORMAL);
      // Power source: hydroelectric generator
      lcd.LCD_write_string(MENU_X + 25, MENU_Y, "HydroGen", MENU_NORMAL);
    }
    else
    {
      // Valve closed
      digitalWrite(VALVE_PIN, HIGH);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 3, "CLOSED", MENU_NORMAL);
      // LED is off.
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 1, "OFF", MENU_NORMAL);
      // Power source: none
      Serial.println("Hi!");
      lcd.LCD_write_string(MENU_X + 25, MENU_Y, "None    ", MENU_NORMAL);
    }
    delay(500);
  }
  else
  {
    digitalWrite(POWER_SOURCE_PIN, HIGH);
    delay(10);
    digitalWrite(VALVE_PIN, HIGH);
    // Valve is closed.
    lcd.LCD_write_string(MENU_X + 42, MENU_Y + 3, "CLOSED", MENU_NORMAL);
    // Power source: hand crank
    lcd.LCD_write_string(MENU_X + 25, MENU_Y, "HandCrnk", MENU_NORMAL);
  }
  
  if(LED_voltage > 500)
  {
    digitalWrite(LIM_SWITCH_PROBE_PIN, HIGH);
    delay(10);
    probe_lim_switch_val = analogRead(LIM_SWITCH_READ_PIN);
    digitalWrite(LIM_SWITCH_PROBE_PIN, LOW);
    
    Serial.print("Limit switch voltage: ");
    Serial.println(probe_lim_switch_val);
    // If limit switch isn't pressed:
    if (probe_lim_switch_val < 20)
    {
      // Pump on.
      digitalWrite(DRIVE_PUMP_PIN, HIGH);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 2, "ON ", MENU_NORMAL);
    }
    else
    {
      // Pump off.
      digitalWrite(DRIVE_PUMP_PIN, LOW);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 2, "OFF", MENU_NORMAL);
    }
  }
  else
  {
    // Pump off.
    digitalWrite(DRIVE_PUMP_PIN, LOW);
    lcd.LCD_write_string(MENU_X + 42, MENU_Y + 2, "OFF", MENU_NORMAL);
  }
}
