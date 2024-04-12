#include <MsTimer2.h>
#include "Keyboard.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 19

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 33

char serialInput = 0;
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
//Row Column pin numbers
#define NUM_ROWS 5
#define NUM_COLS 7
//number of iterations of identical keyscan values before we trigger a keypress
#define DEBOUNCE_ITER 5 
//milliseconds between each scan. SCAN_PERIOD * DEBOUNCE_ITER = minimum response time
#define SCAN_PERIOD 3

#define KEY_RELEASED 1
#define KEY_PRESSED 0

#define KEY_FUNCTION -1
#define NOK 0
#define KC_NONUS_BACKSLASH        ( 100  | 0xF000 )
#define KC_NONUS_HASH             (  50  | 0xF000 )

byte rowPins[NUM_ROWS] = {26,28,30,32,37}; // the pins where the rows are connected in order
byte colPins[NUM_COLS] = {1,2,7,22,21,10,12}; // the pins where the columns are connected in order
byte keyIterCount[NUM_ROWS][NUM_COLS];
byte keyState[NUM_ROWS][NUM_COLS];   


//new keymap 
int keyMap[NUM_ROWS][NUM_COLS] = {
        {NOK, NOK, 0,1,2,3,'A'},
        {NOK, NOK, 4,5, 6, 7,'B'},
        {NOK, NOK, 8, 9, 10, 11,'C'},
        {NOK, NOK, 12, 13, 14, 15,'D'},
        {NOK, NOK, 'F', 'G', 'H', 'I', 'E'}                                 
};

// what does this do? nothing.It's like a shift but does not work anyway 
int funcKeyMap[NUM_ROWS][NUM_COLS] = {
         {NOK, NOK, 0,1,2,3,'A'},
        {NOK, NOK, 4,5, 6, 7,'B'},
        {NOK, NOK, 8, 9, 10, 11,'C'},
        {NOK, NOK, 12, 13, 14, 15,'D'},
        {NOK, NOK, 'F', 'G', 'H', 'I', 'E'}                                   
};

// how are the leds numbered 
int LedMAP[NUM_ROWS][NUM_COLS] = {
        {NOK, NOK, 24,23,22,21,20},
        {NOK, NOK, 15,16,17,18,19},
        {NOK, NOK, 14,13,12,11,10},
        {NOK, NOK, 5,6,7,8,9},
        {NOK, NOK, 4,3,2,1,0}                                   
};

boolean funcMode = false;

void setup() 
{
  Serial.begin(9600);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  for (int row = 0; row < NUM_ROWS; row++) 
  {
    pinMode(rowPins[row], INPUT);
  } 
  
  for (int col = 0; col < NUM_COLS; col++) 
  {
    pinMode(colPins[col], INPUT_PULLUP);
  }

  // Set the initial values on the iter count and state arrays.
  for (int row = 0; row < NUM_ROWS; row++) 
  {
    for (int col = 0; col < NUM_COLS; col++) 
    {
      // Initial iter value is debounce + 1 so that a key transition isn't immediately detected on startup.
      keyIterCount[row][col] = DEBOUNCE_ITER + 1;
      keyState[row][col] = KEY_RELEASED;
    }
  }

  LEDBOX();
  //Keyboard.begin();
  
  MsTimer2::set(SCAN_PERIOD, keyScan);
  MsTimer2::start();
}


//Pressing the FN key could potentially shift the scan code between the key being pressed 
//and being released. If the FN key is hit then any pressed keys have to be reset to be in the 
//'released' state and their iter counts set to DEBOUNCE_ITER+1. 
//Quick improvement: Only do this if the scan codes are different in the two maps. This means that 
//any keys that are the same between the layers like the modifiers will remain pressed.

void resetKeyStates(bool funcMode) 
{
  // Set the initial values on the iter count and state arrays.
  for (int row = 0; row < NUM_ROWS; row++) 
  {
    for (int col = 0; col < NUM_COLS; col++) 
    {
      // Only reset if they're different on the two layers.
      if (keyMap[row][col] != funcKeyMap[row][col]) 
      {
        keyIterCount[row][col] = DEBOUNCE_ITER + 1;

        // If it's PRESSED, then send information over the serial port
        if (keyState[row][col] == KEY_PRESSED) 
        {
          // Indicate that it's a key release on the previous map code
          Serial.print("R");
          Serial.print(funcMode ? funcKeyMap[row][col] : keyMap[row][col]);
          Serial.println();

          // Indicate that it's a key press on the new map code
          Serial.print("P");
          Serial.print(funcMode ? keyMap[row][col] : funcKeyMap[row][col]);
          Serial.println();
        }
      }
    }
  }  
}

//we have a debounced key transition event, either pressed or released.
void transitionHandler(int state, boolean fnMode, int row, int col) 
{
  // Pick which keyMap we're using based on whether we're in func mode
  int scanCode = fnMode ? funcKeyMap[row][col] : keyMap[row][col];

  if (state == KEY_PRESSED) 
  {
    // Send the scanCode over the serial port when a key is pressed
    // Serial.print("P"); // Indicate that it's a key press
    Serial.print(scanCode);
    Serial.println();
  }
 // else if (state == KEY_RELEASED) 
  //{
    // Send the scanCode over the serial port when a key is released
    // Serial.print("R"); // Indicate that it's a key release
    //Serial.print(scanCode);
    //Serial.println();
 // }
}

void colorWipe(uint32_t color, int wait)
{
  for(int i=0; i<strip.numPixels(); i++) 
  { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// this is for changing the color when pressing the button
void setLEDRoutine(int state, int row, int col)
{
    //strip.clear();
    strip.setPixelColor(LedMAP[row][col], strip.Color(0,   255,   0));
    strip.show();
}

void LEDBOX (void)
{
  int LEDNumber = 16;
  int BOX[] = {
          24, 23, 22, 21,/* 20,*/
          15,16, 17,18,/* 19,*/
         14, 13, 12,  11,/* 10,*/
           5, 6,  7 , 8, /*  9,*/
          /*4,  3,  2,  1,   0*/
          };
  strip.clear();

int BOX2[] = {
          /*24, 23, 22, 21, 20,*/
          /*15,*/16, 17,/*18, 19,*/
         /*14,*/ 13, 12, /* 11, 10,*/
          /* 5, 6,  7 , 8,   9,*/
          /*4,  3,  2,  1,   0*/
          };
  // changing the color of the selected LEDs on the keyboard 
  for (int Counter = 0; Counter < LEDNumber; Counter++)
  {
    strip.setPixelColor(BOX[Counter], strip.Color(255,255,255)); // BOX[Counter]= the selected LED, strip.Color(R, G, B)
    strip.show();
  }

  strip.setPixelColor(BOX2[0], strip.Color(128,0,128)); // BOX[Counter]= the selected LED, strip.Color(R, G, B)
  strip.setPixelColor(BOX2[1], strip.Color(128,0,128));
  strip.setPixelColor(BOX2[2], strip.Color(128,0,128));
  strip.setPixelColor(BOX2[3], strip.Color(128,0,128));
  strip.show();
  
}
//Scan handler, runs in interrupt context, triggered by msTimer

//bookmark - I examined the code up to here
void keyScan()
{
  //set the scanFuncMode to false so we can check for any of the FN keys being pressed
  bool scanFuncMode = false;

  //First loop runs through each of the rows,
  for (int row=0; row < NUM_ROWS; row++)
  {
        //for each row pin, set to output LOW 
        pinMode(rowPins[row], OUTPUT);
        digitalWrite(rowPins[row], LOW);
        
        //now iterate through each of the columns, set to input_pullup, 
        //the Row is output and low, and we have input pullup on the column pins,
        //so a '1' is an un pressed switch, and a '0' is a pressed switch.
        for (int col=0; col < NUM_COLS; col++) 
        {
            byte value = digitalRead(colPins[col]);
            //if the value is different from the stored value, then reset the count and set the stored value.
            if(value == KEY_PRESSED && keyState[row][col] == KEY_RELEASED) 
            {
              keyState[row][col] = KEY_PRESSED;
              keyIterCount[row][col] = 0;
              LEDBOX();
              setLEDRoutine(LedMAP[row][col], row, col);

            } 
            else 
              if (value == KEY_RELEASED && keyState[row][col] == KEY_PRESSED) 
              {
               keyState[row][col] = KEY_RELEASED;
               keyIterCount[row][col] = 0;
              } 
              else 
              {
              //Stored value is the same as the current value, this is where our debounce magic happens.
              //if the keyIterCount < debounce iter then increment the keyIterCount and move on
              //if it's == debounce iter then trigger the key & increment it so the trigger doesn't happen again.
              //if it's > debounce iter then we do nothing, except check for the FN key being pressed.
                if(keyIterCount[row][col] < DEBOUNCE_ITER) 
                {
                   keyIterCount[row][col] ++;
                } 
                else
                   if (keyIterCount[row][col] == DEBOUNCE_ITER) 
                  {
                     keyIterCount[row][col] ++;
                    transitionHandler(keyState[row][col], funcMode, row, col);
                  } 
                  else 
                  {
                //We'll check for the func activation here, we just want to check if
                //any of them are pressed. If any are then set the scan func var to 
                //true. It starts as false so if there aren't any func keys pressed
                //it'll remain as false. At the end of the scan we just set the global
                //func bool to whatever the scan local one is.

                //we'll just check the base layer to avoid in sticky situations
                    if(keyMap[row][col] == KEY_FUNCTION && keyState[row][col] == KEY_PRESSED)
                    {
                        scanFuncMode = true;
                    }
                  }
            }
        }
        //now just reset the pin mode (effectively disabling it)
        pinMode(rowPins[row], INPUT);
  }
  //lastly if the scanFuncMode is different from the global func mode, then do the keystate reset, 
  //and set the global func mode to the scan func mode.
  if(funcMode != scanFuncMode) {
    resetKeyStates(funcMode);
    funcMode = scanFuncMode;
  }
}  

// added from mississipi, to be able to write the data in pyhton console
void loop() {

  if (Serial.available())
  {
    char c = Serial.read();
    Serial.print(c);
  }  
}