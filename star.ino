/*
    ___         _         _
   |_ _|_ _  __| |_  _ __| |___ ___
    | || ' \/ _| | || / _` / -_|_-<
   |___|_||_\__|_|\_,_\__,_\___/__/
*/


#include "application.h"
#include "neopixel.h"


/*
  ___       __ _
 |   \ ___ / _(_)_ _  ___ ___
 | |) / -_)  _| | ' \/ -_|_-<
 |___/\___|_| |_|_||_\___/__/
*/

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE

#define PIXEL_PIN D6
#define PIXEL_TYPE WS2812
#define STICK_COUNT 5
#define STICK_LENGTH 8
#define PIXEL_COUNT (STICK_LENGTH * STICK_COUNT)
#define MAX_BRIGHTNESS 10

/*
    __         _       _
  | _ \_ _ ___| |_ ___| |_ _  _ _ __  ___ ___
  |  _/ '_/ _ \  _/ _ \  _| || | '_ \/ -_|_-<
  |_| |_| \___/\__\___/\__|\_, | .__/\___/__/
                          |__/|_|
*/

void colorAll(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

void scanPixels(uint8_t count, bool dir, bool cycle, uint32_t c, uint8_t wait);
void flashAdjacent(uint32_t c, uint8_t wait);
void fillStick (uint32_t c, uint8_t wait);
void fadeInAllPixels (uint32_t c, uint8_t wait);
void fadeOutAllPixels (uint32_t c, uint8_t wait);
void spinStar(uint32_t c, bool dir, uint8_t wait);
void randomPixels(uint32_t c, uint8_t wait);
void rotatematrix(int m, int n, int mat[5][8]);




/*
   ___ _     _          _
  / __| |___| |__  __ _| |___
 | (_ | / _ \ '_ \/ _` | (_-<
  \___|_\___/_.__/\__,_|_/__/
*/

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
int setTestInt(String testValue);
uint8_t myTestValue = 0;
byte testColour = 0;
uint8_t mode = 1;

int LEDmatrix[STICK_COUNT][STICK_LENGTH] = {
      {1,0,0,0,0,0,0,0},
      {1,0,0,0,0,0,0,0},
      {1,0,0,0,0,0,0,0},
      {1,0,0,0,0,0,0,0},
      {1,0,0,0,0,0,0,0}
};


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

/**************************/
/* Particle setup function*/
/**************************/
void setup() {

  Serial.begin();

  //initialise stripe
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //set starting brightnes
  strip.setBrightness(MAX_BRIGHTNESS);

  Particle.function("test_int", setTestInt);
  Particle.function("colour", setColour);
  Particle.function("mode", setMode);
  Particle.function("next_mode", incMode);

}

/***********************************************/
/* Particle function to pass in a test integer */
/***********************************************/
int setTestInt(String testValue){

  myTestValue = testValue.toInt();

  //return success
  return 1;
}

/***********************************************/
/* Particle function to pass in new mode       */
/***********************************************/
int setMode(String modeString){
  mode = modeString.toInt();
  Serial.printf("Mode set to %d \n",mode);
  //return success
  return 1;
}


/**********************************************/
/* Particle function to pass in a test colour */
/**********************************************/

int setColour(String colourVal){

  testColour = (byte)colourVal.toInt();

  return 1;
}


/**********************************************/
/* Particle function to increment the mode    */
/**********************************************/
int incMode(String ignore){
  if (mode <9){
    mode++;
  }
  else{
    mode = 1;
  }

  return 1;
}

/*************/
/* Main Loop */
/*************/

void loop() {
  // Some example procedures showing how to display to the pixels:
  // Do not run more than 15 seconds of these, or the b/g tasks
  // will be blocked.
  //--------------------------------------------------------------


  //strip.setPixelColor(0, strip.Color(255, 0, 255));
  //strip.show();

  switch (mode){

    case 1:
      strip.setBrightness(MAX_BRIGHTNESS);
      colorWipe(strip.Color(255, 0, 0), 50); // Red
      colorWipe(strip.Color(0, 255, 0), 50);
      colorWipe(strip.Color(0, 0, 255), 50);
      break;

    case 2:
      rainbow(10);
      break;

    case 3:
      scanPixels(2,0,1,Wheel(testColour),100);
      break;

    case 4:
      flashAdjacent(Wheel(testColour), 100);
      break;

    case 5:
      fillStick(Wheel(testColour), 100);
      break;

    case 6:
      spinStar(Wheel(testColour),1,100);
      break;

    case 7:
      rainbowCycle(10);
      break;

    case 8:
      pulsePixels(strip.Color(255, 255, 255), 50); // Cyan
      break;

    case 9:
      strip.setBrightness(MAX_BRIGHTNESS);
      randomPixels(strip.Color(255, 255, 255), 50);
      break;

    case 10:
        //rotatematrix(STICK_COUNT,STICK_LENGTH,LEDmatrix);
        showMatrix(strip.Color(255, 255, 255), 250);
        break;

    default:
      colorWipe(strip.Color(255, 255, 255), 50);
      break;
    }
}

/*************************************************/
/* Display the LEDmatrix                         */
/*************************************************/
void showMatrix(uint32_t c, uint8_t wait){

  strip.clear();
  //Transpose the 2d LED matrix to the strip..
  for(uint16_t i=0; i<STICK_COUNT; i++) {
    for(uint16_t j=0; j<STICK_LENGTH; j++){
      if (LEDmatrix[i][j] == TRUE){
        strip.setPixelColor(j+(i*STICK_LENGTH),c);
        Serial.printf("Pixel %d, colour %d \n",j+(i*STICK_LENGTH),c);
      }
    }

  }
  strip.show();
  delay(wait);



}


/*************************************************/
/* Random illumination                           */
/*************************************************/
void randomPixels(uint32_t c, uint8_t wait){

  uint8_t randomNumber1 = random(0,255);
  uint8_t randomNumber2 = random(0,255);
  uint8_t randomNumber3 = random(0,255);

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i,strip.Color(randomNumber1, randomNumber2, randomNumber3));
    randomNumber1 = random(0,255);
    randomNumber2 = random(0,255);
    randomNumber3 = random(0,255);
  }
  strip.show();
  delay(wait);
}

/*************************************************/
/* Spin the star - illuminate sticks sequentially*/
/*************************************************/
void spinStar(uint32_t c, bool dir, uint8_t wait){

  for (uint16_t i=0; i<STICK_COUNT; i++){
    //iterate over sticks
    strip.clear();
    for (uint16_t j=0; j<STICK_LENGTH; j++){
      //iterate over pixels in each sticks
      strip.setPixelColor(j+(i*STICK_LENGTH),c);
    }
    strip.show();
    delay(wait);
  }

}

/*************************************************/
/* Pulse from dim to bright - not working!       */
/*************************************************/
void pulsePixels (uint32_t c, uint8_t wait){
  fadeInAllPixels(c, wait);
  fadeOutAllPixels(c,wait);
  //delay(500);
}


/*************************************************/
/* Fade in a single colour at all pixels         */
/*************************************************/
void fadeInAllPixels (uint32_t c, uint8_t wait){

  //TODO - improve this to fade to a max global brightness setting over a period defined in mS

  strip.clear();
  //strip.setBrightness(MAX_BRIGHTNESS);

  //Turn on everything at the requested colour
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i,c);
  }

  for (uint16_t j=1; j<=MAX_BRIGHTNESS; j++){
    //Turn on everything at the requested colour
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i,c);
    }
    strip.setBrightness(j);
    Serial.printf("Set brightness to %d \n",j );
    strip.show();
    delay(wait);
  }
}


/*************************************************/
/* Fade out a single colour at all pixels */
/*************************************************/
void fadeOutAllPixels (uint32_t c, uint8_t wait){

  strip.clear();
  //strip.setBrightness(254);

  //Turn on everything at the requested colour
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i,c);
  }

  for (uint16_t j=0; j<=MAX_BRIGHTNESS-1; j++){
    strip.setBrightness(MAX_BRIGHTNESS-j);
    Serial.printf("Set brightness to %d \n",MAX_BRIGHTNESS-j);
    strip.show();
    delay(wait);

  }
}


/**********************************/
/* Fill each stick simultaneously */
/**********************************/
void fillStick (uint32_t c, uint8_t wait){

  strip.clear();

  for(uint16_t i=0; i<STICK_LENGTH; i++) {
    for (uint16_t j=0; j<STICK_COUNT; j++){
      strip.setPixelColor(i+(j*STICK_LENGTH), c);
    }
    strip.show();
    delay(wait);
  }

}

/*************************/
/* Flash adjacent pixels */
/*************************/
void flashAdjacent(uint32_t c, uint8_t wait){
  uint8_t i=0;
  static uint8_t offset = 0;

  strip.clear();

  for(i=offset; i<strip.numPixels(); i+=2){
    strip.setPixelColor(i,c);
  }

  if (offset == 0){
    offset = 1;
  }
  else{
    offset = 0;
  }

  strip.show();
  delay(wait);

}

/***************************************************************************/
/* Moving pixels                                                           */
/* count is number of pixels in the moving group                           */
/* dir is direction (true = clockwise, false = anticlockwise)              */
/* cycle will automate a colour cycle when true, ficxed colour c when false*/
/***************************************************************************/
void scanPixels(uint8_t count, bool dir, bool cycle, uint32_t c, uint8_t wait){
  static uint16_t first=0, last=strip.numPixels(), j;
  static byte cycleColour = 0;
  uint8_t pixelCount = count;

  //clear the strip
  strip.clear();

  //Automatically cycle through colours if cycle is true
  if (cycle == true){
    //set the colour
    c = Wheel(cycleColour);
    if (cycleColour < 255){
      cycleColour++;
    }
    else{
      cycleColour = 0;
    }
  }

  //set the next pixels in the sequence.
  //Clockwise
  if (dir == true){

    strip.setPixelColor(first,c);
    //now add the correct number of pixels behind the first pixels
    if (first>0 && pixelCount >0){
      for(j=1; j<count; j++){
        strip.setPixelColor(first-j,c);
      }
    }

    if (first<strip.numPixels()+count-1){
      first++;
    }
    else {
      first= 0;
    }
  }
  //anticlockwise
  else{
    for(j=0; j<count; j++){
      strip.setPixelColor(last-j,c);
    }

    if (last>0){
      last--;
    }
    else {
      last= strip.numPixels();
    }
  }
  strip.show();
  delay(wait);
}

// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint32_t c, uint8_t wait) {
  uint16_t i;

  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

// Fill the dots one after the other with a color, wait (ms) after each one
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

/* Rainbow glow effect */
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// A function to rotate a matrix mat[][] of size R x C.
// Initially, m = R and n = C
void rotatematrix(int m, int n, int mat[5][8])
{
    int row = 0, col = 0;
    int prev, curr;

    /*
       row - Staring row index
       m - ending row index
       col - starting column index
       n - ending column index
       i - iterator
    */
    while (row < m && col < n)
    {

        if (row + 1 == m || col + 1 == n)
            break;

        // Store the first element of next row, this
        // element will replace first element of current
        // row
        prev = mat[row + 1][col];

         /* Move elements of first row from the remaining rows */
        for (int i = col; i < n; i++)
        {
            curr = mat[row][i];
            mat[row][i] = prev;
            prev = curr;
        }
        row++;

        /* Move elements of last column from the remaining columns */
        for (int i = row; i < m; i++)
        {
            curr = mat[i][n-1];
            mat[i][n-1] = prev;
            prev = curr;
        }
        n--;

         /* Move elements of last row from the remaining rows */
        if (row < m)
        {
            for (int i = n-1; i >= col; i--)
            {
                curr = mat[m-1][i];
                mat[m-1][i] = prev;
                prev = curr;
            }
        }
        m--;

        /* Move elements of first column from the remaining rows */
        if (col < n)
        {
            for (int i = m-1; i >= row; i--)
            {
                curr = mat[i][col];
                mat[i][col] = prev;
                prev = curr;
            }
        }
        col++;
    }

}
