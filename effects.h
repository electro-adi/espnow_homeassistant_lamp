uint8_t gHue = 0;






#define MAX_FLASHES 8  // minimum is 4
#define FREQUENCY 6 // delay between strikes (increase for longer time)
uint8_t dimmer, percent;
uint8_t flashStart, flashEnd, flashNum;
CRGB bg_color = CHSV(175, 180, 16);







CRGB currentColor(130,25,25);  // starting color
CRGB targetColor(30,40,130);   // starting target color
const uint8_t fadeRate = 42;   // larger number is a slower fade

boolean fadeToColor = 1;  // turns on/off the fading toward target




CHSV leds_vu[NUM_LEDS];  // FastLED array using HSV.

uint8_t target[NUM_LEDS];  // Array to store target (brightness) values.
//uint8_t delta;  // Amount to increase/decrease (brightness) value by.
uint8_t pixel;  // Which pixel to operate on.
uint8_t temp;  // Used for testing.
uint8_t highCutoff = 230;  // High value where hue ramp ends.
uint8_t lowCutoff = 20;  // Low value cutoff before flooring to zero.
uint8_t ccc = 0;  // color combination choice (ccc)

// Initial color combinations choice (ccc = 0)
uint8_t hue_high = 0;  // red for high values with,
uint8_t hue_low = 96;  // green for low values.





static float pulseSpeed = 0.5;  // Larger value gives faster pulse.

uint8_t hueA = 15;  // Start hue at valueMin.
uint8_t satA = 230;  // Start saturation at valueMin.
float valueMin = 120.0;  // Pulse minimum value (Should be less then valueMax).

uint8_t hueB = 95;  // End hue at valueMax.
uint8_t satB = 255;  // End saturation at valueMax.
float valueMax = 255.0;  // Pulse maximum value (Should be larger then valueMin).

uint8_t hue = hueA;  // Do Not Edit
uint8_t sat = satA;  // Do Not Edit
float val = valueMin;  // Do Not Edit
uint8_t hueDelta = hueA - hueB;  // Do Not Edit
static float delta = (valueMax - valueMin) / 2.35040238;  // Do Not Edit


uint8_t blockSize = 1;  //number of pixels per section




CRGB temp_1[NUM_LEDS];

uint8_t pos;  // stores a position for color being blended in
uint8_t posR, posG, posB;  // positions of moving R,G,B dots
uint8_t hue1;  // used to cycle through the rainbow
uint8_t hue2;  // used to change the moving color
boolean moving = 1;  // switch for moving/not moving



DEFINE_GRADIENT_PALETTE( browngreen_gp ) { 
    0,    6,  255,    0,     //green
   71,    0,  255,  153,     //bluegreen
  122,  200,  200,  200,     //gray
  181,  110,   61,    6,     //brown
  255,    6,  255,    0      //green
};
CRGBPalette16 orangePink = browngreen_gp;



uint8_t colorIndex[NUM_LEDS];

DEFINE_GRADIENT_PALETTE( greenblue_gp ) { 
  0,   0,  255, 245,
  46,  0,  21,  255,
  179, 12, 250, 0,
  255, 0,  255, 245
};

CRGBPalette16 greenblue = greenblue_gp;


void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
  FastLED.delay(1000 / 120);
  EVERY_N_MILLISECONDS(20) 
  {
    gHue++;
  }
}

void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
  FastLED.show();
  FastLED.delay(1000 / 120);
  EVERY_N_MILLISECONDS(20) 
  {
    gHue++;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  FastLED.show();
  FastLED.delay(1000 / 120);
  EVERY_N_MILLISECONDS(20) 
  {
    gHue++;
  }
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
  FastLED.show();
  FastLED.delay(1000 / 120);
  EVERY_N_MILLISECONDS(20) 
  {
    gHue++;
  }
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  FastLED.show();
  FastLED.delay(1000 / 120);
  EVERY_N_MILLISECONDS(20) 
  {
    gHue++;
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  FastLED.show();
  FastLED.delay(1000 / 120);
  EVERY_N_MILLISECONDS(20) 
  {
    gHue++;
  }
}

void lightning_flashes() {

  uint8_t numFlashes = random8(3,MAX_FLASHES);
  for (uint8_t flashCounter = 0; flashCounter < numFlashes; flashCounter++) {
   
    if (flashCounter == 0) {
      dimmer = 5;  // the brightness of the leader is scaled down by a factor of 5
      flashStart = random8(0,NUM_LEDS/3);
      flashEnd = random8(NUM_LEDS/3, NUM_LEDS);
      flashNum = flashEnd - flashStart;
    } else if (flashCounter == 1) {
      dimmer = random8(2,4);  // return strokes are brighter than the leader
      flashStart = random8(0,NUM_LEDS/2);
      flashEnd = random8(NUM_LEDS - (NUM_LEDS/3), NUM_LEDS);
      flashNum = flashEnd - flashStart;
    } else {
      dimmer = random8(1,3);  // return strokes are brighter yet
      flashStart = random8(0,NUM_LEDS/3);
      flashEnd = random8(NUM_LEDS - (NUM_LEDS/4), NUM_LEDS);
      flashNum = flashEnd - flashStart;
    }

    fill_solid(leds, NUM_LEDS, bg_color);  // solid bg color
    fill_solid(leds+flashStart, flashNum - 1, CHSV(0,0,255/dimmer));
    FastLED.show();
    delay(random8(4,10));  // each flash only lasts 4-10 milliseconds
    FastLED.showColor(bg_color);  // "clear" flash before continuing

    // break up the last two flashs a bit
    if (flashCounter == numFlashes - 2) {
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        percent = random8(100);
        if (percent < 45) {
          leds[i] = bg_color;
        }
      }
    }
    if (flashCounter == numFlashes - 1) {
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        percent = random8(100);
        if (percent < 25) {
          leds[i] = bg_color;
        }
      }
    }
    FastLED.show();

    if (flashCounter == 0) { delay(random8(70,120)); }  // longer delay until next flash after the leader
    delay(40+random8(80));  // shorter delay between strikes  
  }

  FastLED.showColor(bg_color);  // "clear" last flash

  delay(random16(1000 + (FREQUENCY * 1000)));  // delay between strikes
}

void nblendU8TowardU8(uint8_t& current, const uint8_t target)
{
  if( current == target) {
    return;
  }

  if( current < target ) {
    uint8_t delta = target - current;
    delta = scale8_video( delta, 1);
    current += delta;
  } else {
    uint8_t delta = current - target;
    delta = scale8_video( delta, 1);
    current -= delta;
  }
}

void newTargetColor() {
  random16_add_entropy( random() );
  targetColor = CHSV( random8(), 255, 255);
}

void fade_toward_solid() {

  EVERY_N_SECONDS(15) {
    newTargetColor();  // set the new target color
    fadeToColor = 1;  // start fading toward target
  }

  EVERY_N_MILLISECONDS(fadeRate) {
    if (currentColor == targetColor) {
      fadeToColor = 0;  // stop fading
    }
    if (fadeToColor) {
      nblendU8TowardU8( currentColor.r, targetColor.r);
      nblendU8TowardU8( currentColor.g, targetColor.g);
      nblendU8TowardU8( currentColor.b, targetColor.b);
      currentColor = CRGB(currentColor.r,currentColor.g,currentColor.b);
    }

    fill_solid(leds, NUM_LEDS, currentColor);

    // For Testing --- always display target on first pixel
    leds[0] = targetColor;

    FastLED.show();
  }
}


void chase_target() { 
  // Assign new random target values after a bit.
  EVERY_N_MILLISECONDS(2000) {
    for (int i=0; i < ( NUM_LEDS / 2 ); i++){  // Only do about half the pixels.
      pixel = random8(NUM_LEDS);  // Pick random pixel.
      target[pixel] = random8();  // Assign random value.
      while (target[pixel] > 90 && target[pixel] < 160) {  // Avoid some middle values, pick again.
        target[pixel] = random8();  // Assign another random value.
      }
      if (target[pixel] < lowCutoff){
        target[pixel] = 0;  // Force low values to clamp to zero.
      }
    }
  }//end assign new target values


  EVERY_N_MILLISECONDS(50) {
    for (uint8_t i=0; i < NUM_LEDS; i++){
      // Check current values against target values.
      if (leds_vu[i].value < target[i]){  // less then the target, so fade up.
        delta = ((target[i] - leds_vu[i].value) / 8) + 1;
        if (leds_vu[i].value + delta >= target[i]){
          leds_vu[i].value = target[i];  // limit to target.
        }
        else {
          leds_vu[i].value = leds_vu[i].value + delta;
        }
      }
      else {  // greater then the target, so fade down.
        delta = ((leds_vu[i].value - target[i]) / 12) + 1;  // +1 so delta is always at least 1.
        if (leds_vu[i].value - delta <= target[i]){
          leds_vu[i].value = target[i];  // limit to target.
        }
        else {
          leds_vu[i].value = leds_vu[i].value - delta;
        }
      }
      if (i == 0){ temp = delta; }  // Save first pixel's delta to print below.
  
      // Tweak hue color based on brightness.
      int c_hue = constrain(leds_vu[i].value,lowCutoff,highCutoff);
      leds_vu[i].hue = map(c_hue, lowCutoff, highCutoff, hue_low, hue_high);
                    // map(valueIn, fromLow,   fromHigh,   toLow,   toHigh)

  
      // Set saturation.
      leds_vu[i].saturation = 255;
  
      // Copy the HSV leds_vu[] data to the leds[] array.
      leds[i] = leds_vu[i];
        // FastLED will automatically convert HSV to RGB data.  Converting from HSV
        // to RGB is very fast and also accurate.  It is also possible to convert from
        // RGB to HSV, but it is not automatic, not as acurate, and not as fast.
  
    }//end of looping over all the pixels checking targets.
  }//end EVERY_N*

  // Continuosly fade target to zero.
  EVERY_N_MILLISECONDS(70) {
    for (int j=0; j < NUM_LEDS; j++){
      if (target[j] > lowCutoff){
        target[j] -= 1;  // Fade by this amount.
      }
      else {
        target[j] = 0;  // If target less then lowCutoff, clamp to zero.
      }
    }
  }//end of continuously fading target down.


  FastLED.show();  // Display the leds[] array.

  EVERY_N_SECONDS(20) {  // Pick a new color combination choice (ccc).
    ccc =  random(4);
    if (ccc == 0) {
      hue_high = 0;  // red for high values
      hue_low = 96;  // green for low values
    }
    if (ccc == 1) {
      hue_high = 96;  // green for high values
      hue_low = 192;  // purple for low values
    }
    if (ccc == 2) {
      hue_high = 105;  // green for high values
      hue_low = 15;  // red for low values
    }
    if (ccc == 3) {
      hue_high = 82;  // yellow-green for high values
      hue_low = 140;  // aqua for low values
    }
  }//end EVERY_N* ccc
}


void breath_effect(){
  float dV = ((exp(sin(pulseSpeed * millis()/2000.0*PI)) -0.36787944) * delta);
  val = valueMin + dV;
  hue = map(val, valueMin, valueMax, hueA, hueB);  // Map hue based on current val
  sat = map(val, valueMin, valueMax, satA, satB);  // Map sat based on current val

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, sat, val);

    // You can experiment with commenting out these dim8_video lines
    // to get a different sort of look.
    leds[i].r = dim8_video(leds[i].r);
    leds[i].g = dim8_video(leds[i].g);
    leds[i].b = dim8_video(leds[i].b);
  }

  FastLED.show();
   
}


void blocks(){

  EVERY_N_SECONDS(2) {  //change the block size every few seconds
    blockSize = random8(2,9);
  }
  
  for (uint8_t n=0; n<(NUM_LEDS/(blockSize*2))+1; n++) {
    uint8_t pos;
    for (uint8_t i=0; i<blockSize; i++) {
      pos = (n*blockSize*2) + i;
      if ( pos >= NUM_LEDS ) { break; }  //don't go past last pixel
      leds[pos] = CRGB(255,0,0);
    }
    for (uint8_t i=0; i<blockSize; i++) {
      pos = (n*blockSize*2) + i + blockSize;
      if ( pos >= NUM_LEDS ) { break; }  //don't go past last pixel
      leds[pos] = CRGB(0,255,0);
    }
  }

  FastLED.show();
  
}



void blend_rainbow() {
  EVERY_N_MILLISECONDS(60) {
    hue1++;
  }

  fill_rainbow(leds, NUM_LEDS, hue1, 255/NUM_LEDS/4);  // draw part of the rainbow into the strip
  fadeToBlackBy(leds, NUM_LEDS, 128);  // fade the whole rainbow down some

  EVERY_N_MILLISECONDS(150) {
    if (moving) {
      pos++;
      if (pos == NUM_LEDS) { pos = 0; }  // reset to begining
    }
  }

  EVERY_N_MILLISECONDS(5) {
    if (!moving) {
      hue2 = hue2 - 1;
    }
  }

  CRGB blendThisIn  = CHSV(hue2, 140, 255);  // colors to blend into background
  CRGB blendThisIn2 = CHSV(hue2, 170, 255);
  CRGB blendThisIn3 = CHSV(hue2, 210, 255);
  CRGB blendThisIn4 = CHSV(hue2, 255, 255);

  static uint8_t blendAmount = 128;  // amount of blend  [range: 0-255]

  nblend(leds[pos],                   blendThisIn4, blendAmount/3);
  nblend(leds[mod8(pos+1, NUM_LEDS)], blendThisIn3, blendAmount/2);
  nblend(leds[mod8(pos+2, NUM_LEDS)], blendThisIn2, blendAmount);
  nblend(leds[mod8(pos+3, NUM_LEDS)], blendThisIn,  blendAmount);
  nblend(leds[mod8(pos+4, NUM_LEDS)], blendThisIn2, blendAmount);
  nblend(leds[mod8(pos+5, NUM_LEDS)], blendThisIn3, blendAmount/2);
  nblend(leds[mod8(pos+6, NUM_LEDS)], blendThisIn4, blendAmount/3);

  posR = beatsin8(5, 0, (NUM_LEDS-1));
  posG = beatsin8(7, 0, (NUM_LEDS-1));
  posB = beatsin8(9, 0, (NUM_LEDS-1));

  CRGB tempColor = CRGB::Red;
  temp_1[posR] = tempColor;
  tempColor = CRGB::Green;
  nblend(temp_1[posG], tempColor, 128);
  tempColor = CRGB::Blue;
  nblend(temp_1[posB], tempColor, 128);

  EVERY_N_MILLISECONDS(2) {
    fadeToBlackBy(temp_1, NUM_LEDS, 1);  // create fading tail effect
  }

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    nblend(leds[i], temp_1[i], 128);
  }

  nblend(leds[mod8(posR, NUM_LEDS)], CRGB::Red, 90);
  nblend(leds[mod8(posR+1, NUM_LEDS)], CRGB::Red, 128);
  nblend(leds[mod8(posR+2, NUM_LEDS)], CRGB::Red, 90);

  nblend(leds[mod8(posG, NUM_LEDS)], CRGB::Green, 90);
  nblend(leds[mod8(posG+1, NUM_LEDS)], CRGB::Green, 128);
  nblend(leds[mod8(posG+2, NUM_LEDS)], CRGB::Green, 90);

  nblend(leds[mod8(posB, NUM_LEDS)], CRGB::Blue, 90);
  nblend(leds[mod8(posB+1, NUM_LEDS)], CRGB::Blue, 128);
  nblend(leds[mod8(posB+2, NUM_LEDS)], CRGB::Blue, 90);

  FastLED.show();  // update the display

  EVERY_N_SECONDS(random8(4, 16)) {
    moving = !moving;  // randomly start/stop part of the animation for fun
  }
}


void phase_beat() {

  uint8_t sinBeat   = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
  uint8_t sinBeat2  = beatsin8(30, 0, NUM_LEDS - 1, 0, 85);
  uint8_t sinBeat3  = beatsin8(30, 0, NUM_LEDS - 1, 0, 170);

  // If you notice that your pattern is missing out certain LEDs, you
  // will need to use the higher resolution beatsin16 instead. In this
  // case remove the 3 lines above and replace them with the following:
  // uint16_t sinBeat   = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);
  // uint16_t sinBeat2  = beatsin16(30, 0, NUM_LEDS - 1, 0, 21845);
  // uint16_t sinBeat3  = beatsin16(30, 0, NUM_LEDS - 1, 0, 43690);

  leds[sinBeat]   = CRGB::Blue;
  leds[sinBeat2]  = CRGB::Red;
  leds[sinBeat3]  = CRGB::White;
  
  fadeToBlackBy(leds, NUM_LEDS, 10);

  FastLED.show();
}


void moving_dot() {

  uint16_t sinBeat = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);

  leds[sinBeat] = CRGB::Blue;
  
  fadeToBlackBy(leds, NUM_LEDS, 10);

  FastLED.show();
}


void gradient_beat() {

  uint16_t beatA = beatsin16(30, 0, 255);
  uint16_t beatB = beatsin16(20, 0, 255);
  fill_palette(leds, NUM_LEDS, (beatA + beatB) / 2, 10, orangePink, 255, LINEARBLEND);

  FastLED.show();
}

void brightness_wave() {

  //Creat a sin wave with period of 2 seconds (30bpm) to change the brightness of the strip
  uint8_t sinBeat = beatsin8(30, 50, 255, 0, 0);
  
  // Color each pixel from the palette using the index from colorIndex[]
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(greenblue, colorIndex[i], sinBeat);
  }
  
  EVERY_N_MILLISECONDS(5){
    for (int i = 0; i < NUM_LEDS; i++) {
      colorIndex[i]++;
    }
  }
  FastLED.show();
}


void adding_waves() {

  // Waves for LED position
  uint8_t posBeat  = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
  uint8_t posBeat2 = beatsin8(60, 0, NUM_LEDS - 1, 0, 0);
  uint8_t posBeat3 = beatsin16(30, 0, NUM_LEDS - 1, 0, 127);
  uint8_t posBeat4 = beatsin16(60, 0, NUM_LEDS - 1, 0, 127);


  // In the video I use beatsin8 for the positions. For longer strips,
  // the resolution isn't high enough for position and can lead to some
  // LEDs not lighting. If this is the case, use the 16 bit versions below
  // uint16_t posBeat  = beatsin16(30, 0, NUM_LEDS - 1, 0, 0);
  // uint16_t posBeat2 = beatsin16(60, 0, NUM_LEDS - 1, 0, 0);
  // uint16_t posBeat3 = beatsin16(30, 0, NUM_LEDS - 1, 0, 32767);
  // uint16_t posBeat4 = beatsin16(60, 0, NUM_LEDS - 1, 0, 32767);

  // Wave for LED color
  uint8_t colBeat  = beatsin8(45, 0, 255, 0, 0);

  leds[(posBeat + posBeat2) / 2]  = CHSV(colBeat, 255, 255);
  leds[(posBeat3 + posBeat4) / 2]  = CHSV(colBeat, 255, 255);

  fadeToBlackBy(leds, NUM_LEDS, 10);

  FastLED.show();
}