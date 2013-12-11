/*
* Authors: Amanda Clark and Austin Tew
* Title: Alternative Aquatic Battery
* Last Modified: December 10, 2013
*/

#include "LCD4884.h"

// Initial starting points for the display
#define MENU_X	5		// 0-83
#define MENU_Y	1		// 0-5

// constants used to define pin numbers
// Analog pins:
const int LED_VOLTAGE_PIN = 1;
const int ALT_LED_POWER_PIN = 5;
const int PROBE_READ_PIN = 2;
const int OUTPUT_VOLTAGE_PIN = 3;
const int LIM_SWITCH_READ_PIN = 4;
// Digital pins:
const int LIM_SWITCH_PROBE_PIN = 10;
const int POWER_SOURCE_PIN = 8;
const int DRIVE_PUMP_PIN = 9;
const int VALVE_PIN = 12;
const int PROBE_PIN = 13;

// Values used during simulation
int LED_voltage;
int output_voltage;
int probe_voltage;
int probe_lim_switch_val;
char num[10];
bool valve_closed;

// The setup routine runs once when you press reset:
void setup() 
{
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  // Initialize the digital pins as outputs, and initialize 
  // their values.
  pinMode(POWER_SOURCE_PIN, OUTPUT);
  digitalWrite(POWER_SOURCE_PIN, LOW);
  pinMode(DRIVE_PUMP_PIN, OUTPUT);
  digitalWrite(DRIVE_PUMP_PIN, HIGH);
  pinMode(VALVE_PIN, OUTPUT);
  digitalWrite(VALVE_PIN, HIGH);
  pinMode(LIM_SWITCH_PROBE_PIN, OUTPUT);
  digitalWrite(LIM_SWITCH_PROBE_PIN, LOW);
  
  valve_closed = true;
  
  // LCD display initialization
  lcd.LCD_init();
  lcd.LCD_clear();
  
  // Menu initialization
  init_MENU();
}

void init_MENU(void)
{
  byte i;
  lcd.LCD_clear();
  // Display the voltage:
  lcd.LCD_write_string(MENU_X + 11, MENU_Y, "V: ", MENU_NORMAL);
  // Initialize the source
  lcd.LCD_write_string(MENU_X + 25, MENU_Y, "None      ", MENU_NORMAL);
  // Initialize the labes for the switch, pump, and valve.
  // These are only displayed once to save processor time.
  lcd.LCD_write_string(MENU_X, MENU_Y + 1, "Switch:", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Pump: ", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, MENU_Y + 3, "Valve: ", MENU_NORMAL);
}

// The loop routine runs over and over again forever:
void loop() 
{
  // read the input on analog pin 0:
  LED_voltage = analogRead(LED_VOLTAGE_PIN);//LED_VOLTAGE_PIN);  
  
  // Not enough voltage is being supplied by the hand crank to
  // power the LED.
  if (LED_voltage < 200)
  {
    
    // Pump off.
    digitalWrite(DRIVE_PUMP_PIN, HIGH);
    
    // Set the power to come from the hydroelectric generator.
    digitalWrite(POWER_SOURCE_PIN, LOW);
    
    // Determine whether the switch is on or off.
    pinMode(PROBE_PIN, OUTPUT);
    digitalWrite(PROBE_PIN, HIGH);
    delay(10);
    probe_voltage = analogRead(PROBE_READ_PIN);
    // Set the pin to an input so that it's no longer driving
    // the pin to a certain value.
    pinMode(PROBE_PIN, INPUT);
    
    // The switch is on.
    if (probe_voltage > 10)
    {
      // Valve open
      valve_closed = false;
      digitalWrite(DRIVE_PUMP_PIN, HIGH);
      digitalWrite(VALVE_PIN, LOW);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 3, "OPEN  ", MENU_NORMAL);
      // LED is on.
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 1, "ON ", MENU_NORMAL);
      // Power source: hydroelectric generator
      lcd.LCD_write_string(MENU_X + 25, MENU_Y, "HydroGen", MENU_NORMAL);
    }
    // The switch is off.
    else
    {
      // Valve closed
      valve_closed = true;
      digitalWrite(VALVE_PIN, HIGH);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 3, "CLOSED", MENU_NORMAL);
      // LED is off.
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 1, "OFF", MENU_NORMAL);
      // Power source: none
      lcd.LCD_write_string(MENU_X + 25, MENU_Y, "None    ", MENU_NORMAL);
    }
    // Delay for switch probe.
    delay(500);
  }
  // The hand crank is doing well.
  else
  {
    valve_closed = true;
    // Power comes from hand crank.
    digitalWrite(POWER_SOURCE_PIN, HIGH);
    delay(10);
    
    // Valve is closed.
    digitalWrite(VALVE_PIN, HIGH);
    lcd.LCD_write_string(MENU_X + 42, MENU_Y + 3, "CLOSED", MENU_NORMAL);
    // Power source: hand crank
    lcd.LCD_write_string(MENU_X + 25, MENU_Y, "HandCrnk", MENU_NORMAL);
  }
  
  if (valve_closed)
  {
    output_voltage = analogRead(OUTPUT_VOLTAGE_PIN);
    output_voltage = output_voltage / 100;
  }
  else
  {
    output_voltage = analogRead(ALT_LED_POWER_PIN);
    output_voltage = output_voltage / 200;
  }
  
  delay(10);
  
  // Display the input voltage to the LED:
  lcd.LCD_write_string(MENU_X, MENU_Y, itoa(output_voltage, num, 10), MENU_NORMAL);
  lcd.LCD_write_string(MENU_X + 6, MENU_Y, " ", MENU_NORMAL);
  
  // The hand crank is generating enough power for the pump 
  // and LED.
  if(LED_voltage > 500)
  {
    // Don't run the pump if the reservoir is full, determined
    // by the limit switch.
    digitalWrite(LIM_SWITCH_PROBE_PIN, HIGH);
    delay(10);
    probe_lim_switch_val = analogRead(LIM_SWITCH_READ_PIN);
    digitalWrite(LIM_SWITCH_PROBE_PIN, LOW);
   
    // If limit switch isn't pressed (reservoir isn't full):
    if (probe_lim_switch_val < 20)
    {
      // Pump on.
      Serial.println("Bah!");
      digitalWrite(DRIVE_PUMP_PIN, LOW);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 2, "ON ", MENU_NORMAL);
    }
    // Limit switch is pressed.
    else
    {
      // Pump off.
      digitalWrite(DRIVE_PUMP_PIN, HIGH);
      lcd.LCD_write_string(MENU_X + 42, MENU_Y + 2, "OFF", MENU_NORMAL);
    }
  }
  // There's not enough power from the hand crank to power the
  // pump and LED.
  else
  {
    // Pump off.
    digitalWrite(DRIVE_PUMP_PIN, LOW);
    lcd.LCD_write_string(MENU_X + 42, MENU_Y + 2, "OFF", MENU_NORMAL);
  }
}
