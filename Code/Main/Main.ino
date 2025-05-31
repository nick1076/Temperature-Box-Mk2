//Headers
#include <Wire.h> //Custom i2c pinout library
#include <Adafruit_GFX.h> //Graphics library for display
#include <Adafruit_SSD1306.h> //Display library
#include <DHT.h> //DHT22 (Temp/humidity sensor) library

//Vars
#define SCREEN_WIDTH 128 //Width of display used
#define SCREEN_HEIGHT 64 //Height of display used
#define DHT_TYPE DHT22 //Type of sensor (DHT11 or DHT22, 22 in this case)

#define DHT_PIN 9 //DHT22 data pin on uController
#define SDA_PIN 2 //SDA pin on uController
#define SCL_PIN 3 //SCL pin on uController
#define BUTTON_PIN 4 //Push Button pin on uController

bool initialized = false; //Tracks if screen initialized properly
bool tempMode = true; //When true, displays temperature, otherwise shows humidity

unsigned int tick = 0; //tick increments once per millisecond, used to queue actions in loop on intervals whilst checking inputs per tick
int buttonTick = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //Screen
DHT dht(DHT_PIN, DHT_TYPE); //Temperature Sensor

void setup() 
{
  //Device / pin setup
  Wire.begin(SDA_PIN, SCL_PIN); //Setup custom i2c line on these pins
  dht.begin(); //Start the temperature sensor
  pinMode(BUTTON_PIN, INPUT_PULLUP); //Initialize button as input

  //Serial setup
  Serial.begin(9600); //Start serial
  Serial.println("Initializing...");

  //Check if display has failed to begin
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }
  else{
    //Otherwise display has started correctly
    Serial.println("Initialized.");
    initialized = true;
    Display("...");
  }

  //Splashscreen delay
  delay(1000);    
}

void loop() 
{
  //Check if display never initialized, and return if so
  if (!initialized) { return; }

  if (tick >= 200)
  {
    tick=0;
  }
  
  delay(10);
  tick++;
  buttonTick--;

  if (tick == 1)
  {
    //Grab new temperature value and update screen
    UpdateGUI();
  }
  
  if (digitalRead(BUTTON_PIN) == LOW && buttonTick <= 0)
  {
    tempMode = !tempMode;
    buttonTick = 20;
    UpdateGUI();
  }
}

//Grabs new temperature/humidity value from DHT22 and updates screen with it
void UpdateGUI(){
  double humiRaw = dht.readHumidity();
  double tempRaw = dht.readTemperature();
  
  if (isnan(tempRaw) || isnan(humiRaw)) {
    Display("NaN");
    return;
  }

  tempRaw = tempRaw * 1.8 + 32; //C to F

  byte temp = (byte)tempRaw;
  byte humi = (byte)humiRaw;

  if (tempMode)
  {
    display.clearDisplay();

    //Temperature Value
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(40, 18);
    display.println(temp);

    //Degree Symbol
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(90, 14);
    display.println('o');
    
    display.display();
  }
  else
  {
    display.clearDisplay();

    //Humidity Value
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(30, 18);
    display.print(humi);
    display.println('%');
    
    display.display();
  }
}

//General display method that shows text in the middle
void Display(String text){
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(text);
  display.display(); 
}
