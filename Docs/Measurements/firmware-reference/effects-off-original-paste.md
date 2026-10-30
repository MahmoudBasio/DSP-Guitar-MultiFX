\#include \<Arduino.h>

\#include \<U8g2lib.h>

\#include \<Encoder.h>

\#include \<Audio.h>

\#include \<Wire.h>

\#include \<SPI.h>

\#include \<math.h>



// =====================================================

// 1. HARDWARE & DISPLAY SETUP

// =====================================================

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /\* clock=\*/ 24, /\* data=\*/ 25, /\* reset=\*/ U8X8_PIN_NONE);



Encoder myEnc(31, 32);&#x20;

const int BTN_ENC   = 30;&#x20;

const int FS_DELAY  = 34;  

const int FS_REVERB = 33;&#x20;

const int FS_CHORUS = 35;&#x20;



// =====================================================

// 2. DSP & EFFECT STATES

// =====================================================

volatile bool DELAY_ON  = true;

volatile bool REVERB_ON = true;

volatile bool CHORUS_ON = true;



// DSP Constants

float CHORUS_DRY_GAIN         = 0.6f;

float REVERB_FROM_DELAY_GAIN  = 0.6f;

float REVERB_FROM_CHORUS_GAIN = 0.5f;

float FINAL_DRY_GAIN          = 0.65f;

float FINAL_WET_GAIN          = 0.75f;

float PRE_FILTER_HZ           = 3200.0f;

float REPEAT_FILTER_HZ        = 1800.0f;

float OUTPUT_VOLUME           = 0.7f;

int   LINEIN_LEVEL            = 9;



// Delay Modulation (Handled in loop)

float DELAY_MOD_DEPTH_MS = 3.0f;

float DELAY_MOD_RATE_HZ  = 0.18f;

unsigned long lastModUpdate = 0;



// Dynamic DSP Variables Updated by UI

int   current_delay_time_ms = 430;

float current_delay_fb      = 0.58f;

float current_delay_wet     = 0.55f;

float current_reverb_room   = 0.9f;

float current_reverb_damp   = 0.3f;

float current_reverb_wet    = 0.6f;

float current_chorus_rate   = 0.3f;

float current_chorus_depth  = 20.0f;

float current_chorus_base   = 20.0f;

float current_chorus_wet    = 0.8f;



// =====================================================

// 3. UI STATE MANAGEMENT & PRESETS

// =====================================================

enum Screen { MAIN_MENU, SUB_MENU, EDIT_VALUE };

volatile Screen current_screen = MAIN_MENU;

volatile int main_index = 0;&#x20;

int sub_index = 0;  



volatile bool ui_needs_update = true;



// Preset initial values mapping to your required defaults (0-100 scale)

// DELAY:  Time \~42% (430ms), FB 61% (0.58), Mix 55% (0.55)

// REVERB: Size 90% (0.9), Damp 30% (0.3), Wet 60% (0.6)

// CHORUS: Rate \~4% (0.3Hz), Depth 66% (20ms), Base 50% (20ms)

const int preset_values[3][3] = {

  {42, 61, 55}, // Delay

  {90, 30, 60}, // Reverb

  {4,  66, 50}  // Chorus

};



int values[3][3]; // Current active values



const char\* main_menu[] = {"DELAY", "REVERB", "CHORUS", "RESET ALL"};

const char\* delay_params[]  = {"Time (ms)", "Feedback", "Mix"};

const char\* reverb_params[] = {"Room Size", "Damping", "Wet"};

const char\* chorus_params[] = {"Rate (Hz)", "Depth", "Base (ms)"};



// Timing Variables

unsigned long lastInterruptTime = 0;

const unsigned long debounceDelay = 250;&#x20;

const unsigned long doubleClickGap = 400;&#x20;

unsigned long lastClickTime = 0;



// Forward declarations

void applySettings();

void drawUI();



// =====================================================

// 4. CUSTOM AUDIO CHORUS CLASS

// =====================================================

class AudioEffectCustomChorus : public AudioStream {

public:

  AudioEffectCustomChorus() : AudioStream(1, inputQueueArray) {}

  void setEnabled(bool en) { enabled = en; }

  void setParams(float baseDelayMs, float depthMs, float rateHz, float dryGain, float wetGain) {

    baseDelay = baseDelayMs;

    depth     = depthMs;

    rate      = rateHz;

    dryMix    = dryGain;

    wetMix    = wetGain;

  }

  void update(void) override {

    audio_block_t \*block = receiveWritable(0);

    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

      float input = block->data[i] / 32768.0f;

      if (!enabled) {

        block->data[i] = (int16_t)(input \* 32767.0f);

        continue;

      }

      delayBuffer[writeIndex] = input;

      float lfo = sinf(lfoPhase);

      lfoPhase += 2.0f \* PI \* rate / SAMPLE_RATE;

      if (lfoPhase > 2.0f \* PI) lfoPhase -= 2.0f \* PI;

      float delayMs = baseDelay + depth \* lfo;

      if (delayMs < 1.0f) delayMs = 1.0f;

      float delaySamples = delayMs \* SAMPLE_RATE / 1000.0f;

      float readPos = writeIndex - delaySamples;

      while (readPos < 0) readPos += BUFFER_SIZE;

      int index1 = (int)readPos;

      int index2 = (index1 + 1) % BUFFER_SIZE;

      float frac = readPos - index1;

      float delayed = delayBuffer[index1] \* (1.0f - frac) + delayBuffer[index2] \* frac;

      float output = input \* dryMix + delayed \* wetMix;

      if (output > 1.0f) output = 1.0f;

      if (output < -1.0f) output = -1.0f;

      block->data[i] = (int16_t)(output \* 32767.0f);

      writeIndex++;

      if (writeIndex >= BUFFER_SIZE) writeIndex = 0;

    }

    transmit(block);

    release(block);

  }

private:

  audio_block_t \*inputQueueArray[1];

  static const int BUFFER_SIZE = 2048;

  float delayBuffer[BUFFER_SIZE] = {0};

  int writeIndex = 0;

  float lfoPhase = 0.0f;

  bool enabled   = true;

  float baseDelay = 20.0f;

  float depth     = 10.0f;

  float rate      = 0.2f;

  float dryMix    = 0.6f;

  float wetMix    = 0.8f;

  const float SAMPLE_RATE = 44100.0f;

};



// =====================================================

// 5. AUDIO OBJECTS & CONNECTIONS

// =====================================================

AudioInputI2S            in;

AudioEffectCustomChorus  chorus;

AudioFilterBiquad        preFilter;

AudioMixer4              delayInputMixer;

AudioEffectDelay         delay1;

AudioFilterBiquad        repeatFilter;

AudioMixer4              reverbInputMixer;

AudioEffectFreeverb      reverb1;

AudioMixer4              wetMixer;

AudioMixer4              finalMixerL;

AudioMixer4              finalMixerR;

AudioOutputI2S           out;

AudioControlSGTL5000     audioShield;



AudioConnection patchCord1(in, 0, preFilter, 0);

AudioConnection patchCord2(preFilter, 0, chorus, 0);

AudioConnection patchCord3(chorus, 0, finalMixerL, 0);

AudioConnection patchCord4(chorus, 0, finalMixerR, 0);

AudioConnection patchCord5(chorus, 0, delayInputMixer, 0);

AudioConnection patchCord6(delayInputMixer, 0, delay1, 0);

AudioConnection patchCord7(delay1, 0, repeatFilter, 0);

AudioConnection patchCord8(repeatFilter, 0, delayInputMixer, 1);

AudioConnection patchCord9(repeatFilter, 0, wetMixer, 0);

AudioConnection patchCord10(repeatFilter, 0, reverbInputMixer, 0);&#x20;

AudioConnection patchCord11(chorus, 0, reverbInputMixer, 1);&#x20;

AudioConnection patchCord12(reverbInputMixer, 0, reverb1, 0);

AudioConnection patchCord13(reverb1, 0, wetMixer, 1);

AudioConnection patchCord14(wetMixer, 0, finalMixerL, 1);

AudioConnection patchCord15(wetMixer, 0, finalMixerR, 1);

AudioConnection patchCord16(finalMixerL, 0, out, 0);

AudioConnection patchCord17(finalMixerR, 0, out, 1);



// =====================================================

// 6. HELPER LOGIC

// =====================================================



// Maps 0-100 UI values to actual DSP float ranges

float mapFloat(int x, float in_min, float in_max, float out_min, float out_max) {

  return (float)(x - in_min) \* (out_max - out_min) / (float)(in_max - in_min) + out_min;

}



// Push 0-100 arrays to true DSP Engine values

void applySettings() {

  // Update actual DSP variables based on the 0-100 UI arrays

  current_delay_time_ms = (int)mapFloat(values[0][0], 0, 100, 10.0, 1000.0);

  current_delay_fb      = mapFloat(values[0][1], 0, 100, 0.0, 0.95);

  current_delay_wet     = mapFloat(values[0][2], 0, 100, 0.0, 1.0);



  current_reverb_room   = mapFloat(values[1][0], 0, 100, 0.0, 1.0);

  current_reverb_damp   = mapFloat(values[1][1], 0, 100, 0.0, 1.0);

  current_reverb_wet    = mapFloat(values[1][2], 0, 100, 0.0, 1.0);



  current_chorus_rate   = mapFloat(values[2][0], 0, 100, 0.1, 5.0);

  current_chorus_depth  = mapFloat(values[2][1], 0, 100, 0.0, 30.0);

  current_chorus_base   = mapFloat(values[2][2], 0, 100, 10.0, 30.0);



  // Apply to hardware/objects

  audioShield.volume(OUTPUT_VOLUME);

  audioShield.lineInLevel(LINEIN_LEVEL);



  preFilter.setLowpass(0, PRE_FILTER_HZ, 0.707f);

  repeatFilter.setLowpass(0, REPEAT_FILTER_HZ, 0.707f);



  // Chorus update

  chorus.setEnabled(CHORUS_ON);

  chorus.setParams(current_chorus_base, current_chorus_depth, current_chorus_rate, CHORUS_DRY_GAIN, current_chorus_wet);



  // Delay update (Base delay applied, modulation adds to it in loop)

  delay1.delay(0, current_delay_time_ms);

  delayInputMixer.gain(0, DELAY_ON ? 1.0f : 0.0f);                &#x20;

  delayInputMixer.gain(1, DELAY_ON ? current_delay_fb : 0.0f);  



  // Reverb update

  reverb1.roomsize(current_reverb_room);

  reverb1.damping(current_reverb_damp);

  reverbInputMixer.gain(0, (DELAY_ON && REVERB_ON) ? REVERB_FROM_DELAY_GAIN : 0.0f);

  reverbInputMixer.gain(1, REVERB_ON ? REVERB_FROM_CHORUS_GAIN : 0.0f);



  // Mixer Updates

  wetMixer.gain(0, DELAY_ON ? current_delay_wet : 0.0f);  &#x20;

  wetMixer.gain(1, REVERB_ON ? current_reverb_wet : 0.0f);&#x20;



  finalMixerL.gain(0, FINAL_DRY_GAIN);

  finalMixerR.gain(0, FINAL_DRY_GAIN);

  finalMixerL.gain(1, FINAL_WET_GAIN);

  finalMixerR.gain(1, FINAL_WET_GAIN);

}



// Reset System function

void resetToDefaults() {

  for(int i=0; i<3; i++) {

    for(int j=0; j<3; j++) {

      values[i][j] = preset_values[i][j];

    }

  }

  applySettings();

  ui_needs_update = true;

}



// ISRs for footswitches (Toggles Effect On/Off)

void isrToggleReverb() {&#x20;

  if (millis() - lastInterruptTime > debounceDelay) {&#x20;

    REVERB_ON = !REVERB_ON;&#x20;

    applySettings(); ui_needs_update = true;&#x20;

    lastInterruptTime = millis();&#x20;

  }&#x20;

}

void isrToggleDelay() {&#x20;

  if (millis() - lastInterruptTime > debounceDelay) {&#x20;

    DELAY_ON = !DELAY_ON;&#x20;

    applySettings(); ui_needs_update = true;&#x20;

    lastInterruptTime = millis();&#x20;

  }&#x20;

}

void isrToggleChorus() {&#x20;

  if (millis() - lastInterruptTime > debounceDelay) {&#x20;

    CHORUS_ON = !CHORUS_ON;&#x20;

    applySettings(); ui_needs_update = true;&#x20;

    lastInterruptTime = millis();&#x20;

  }&#x20;

}



// Encoder Button Logic

void handleButtonLogic() {

  if (current_screen == MAIN_MENU) {

    if (main_index == 3) {

      resetToDefaults();

    } else {

      current_screen = SUB_MENU;

      sub_index = 0;

    }

  }&#x20;

  else if (current_screen == SUB_MENU) {

    current_screen = EDIT_VALUE;

  }&#x20;

  else if (current_screen == EDIT_VALUE) {

    current_screen = SUB_MENU;

  }

}



// Encoder Rotation Logic

void handleRotation(int dir) {

  if (current_screen == MAIN_MENU) {

    main_index = constrain(main_index + dir, 0, 3);

  } else if (current_screen == SUB_MENU) {

    sub_index = constrain(sub_index + dir, 0, 2);

  } else if (current_screen == EDIT_VALUE) {

    values[main_index][sub_index] = constrain(values[main_index][sub_index] + dir, 0, 100);

    applySettings(); // Instantly apply parameter changes

  }

}



// =====================================================

// 7. MAIN SETUP

// =====================================================

void setup() {

  Serial.begin(115200);

  AudioMemory(300);

  audioShield.enable();

  audioShield.inputSelect(AUDIO_INPUT_LINEIN);



  u8g2.begin();



  // Splash Screen

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_7x14_tr);

  u8g2.drawStr(15, 38, "DSP CONTROLLER");&#x20;

  u8g2.sendBuffer();

  delay(1500);&#x20;



  pinMode(BTN_ENC, INPUT_PULLUP);

  pinMode(FS_DELAY, INPUT_PULLUP);

  pinMode(FS_REVERB, INPUT_PULLUP);

  pinMode(FS_CHORUS, INPUT_PULLUP);



  // Hardware interrupts map Switch numbers to effects

  attachInterrupt(digitalPinToInterrupt(FS_REVERB), isrToggleReverb, FALLING); // SW1

  attachInterrupt(digitalPinToInterrupt(FS_DELAY), isrToggleDelay, FALLING);   // SW2

  attachInterrupt(digitalPinToInterrupt(FS_CHORUS), isrToggleChorus, FALLING); // SW3



  // Set default matrices & start Audio engine

  resetToDefaults();

  drawUI();&#x20;

}



// =====================================================

// 8. MAIN LOOP

// =====================================================

void loop() {

  // 1. Encoder Handling

  static long oldPos = -999;

  long newPos = myEnc.read() / 4;

  if (newPos != oldPos) {

    handleRotation((newPos > oldPos) ? 1 : -1);

    oldPos = newPos;

    ui_needs_update = true;

  }



  // 2. Encoder Click Handling

  static int lastBtn = HIGH;

  int currentBtn = digitalRead(BTN_ENC);

  if (currentBtn == LOW && lastBtn == HIGH) {

    unsigned long currentTime = millis();

    if (currentTime - lastClickTime < doubleClickGap) {

      current_screen = MAIN_MENU;&#x20;

    } else {

      handleButtonLogic();&#x20;

    }

    lastClickTime = currentTime;

    ui_needs_update = true;

    delay(150);

  }

  lastBtn = currentBtn;



  // 3. Audio Modulation for Delay

  if (DELAY_ON && (millis() - lastModUpdate >= 20)) {

    lastModUpdate = millis();

    float t = millis() / 1000.0f;

    float mod = DELAY_MOD_DEPTH_MS \* sinf(2.0f \* PI \* DELAY_MOD_RATE_HZ \* t);

    int dynamicDelay = (int)(current_delay_time_ms + mod);

    if (dynamicDelay < 1) dynamicDelay = 1;

    delay1.delay(0, dynamicDelay);

  }



  // 4. UI Redraw Trigger

  if (ui_needs_update) {

    drawUI();

    ui_needs_update = false;

  }

}



// =====================================================

// 9. UI DRAW LOGIC

// =====================================================

void drawUI() {

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_6x12_tr);



  if (current_screen == MAIN_MENU) {

    u8g2.drawStr(15, 10, "- MAIN MENU -");

    u8g2.drawHLine(0, 13, 128);

    for (int i = 0; i < 4; i++) {

      int y = 28 + (i \* 11);



      // Setup status tag for effect items

      char itemText[30];

      if (i == 0) sprintf(itemText, "DELAY  [%s]", DELAY_ON ? "ON" : "OFF");

      else if (i == 1) sprintf(itemText, "REVERB [%s]", REVERB_ON ? "ON" : "OFF");

      else if (i == 2) sprintf(itemText, "CHORUS [%s]", CHORUS_ON ? "ON" : "OFF");

      else strcpy(itemText, main_menu[i]); // Reset All



      if (i == main_index) {&#x20;

        u8g2.drawStr(5, y, ">");&#x20;

        u8g2.drawStr(20, y, itemText);&#x20;

      } else {&#x20;

        u8g2.drawStr(15, y, itemText);&#x20;

      }

    }

  }&#x20;

  else {

    // SUB-MENU Screen

    if (current_screen == SUB_MENU) {

      char title[30]; sprintf(title, "%s SETTINGS", main_menu[main_index]);

      u8g2.drawStr(10, 10, title); u8g2.drawHLine(0, 13, 128);

      for (int i = 0; i < 3; i++) {

        int y = 30 + (i \* 11);

        const char\* pName = (main_index == 0) ? delay_params[i] : (main_index == 1) ? reverb_params[i] : chorus_params[i];



        char lineBuf[30];

        sprintf(lineBuf, "%s: %d%%", pName, values[main_index][i]);



        if (i == sub_index) {&#x20;

          u8g2.drawStr(5, y, ">");&#x20;

          u8g2.drawStr(15, y, lineBuf);&#x20;

        } else {&#x20;

          u8g2.drawStr(15, y, lineBuf);&#x20;

        }

      }

    }&#x20;

    // EDIT Screen

    else if (current_screen == EDIT_VALUE) {

      u8g2.drawStr(5, 10, "ADJUSTING...");

      const char\* pName = (main_index == 0) ? delay_params[sub_index] : (main_index == 1) ? reverb_params[sub_index] : chorus_params[sub_index];

      u8g2.setFont(u8g2_font_9x15_tr); u8g2.drawStr(10, 30, pName);



      // Progress Bar

      u8g2.drawFrame(10, 38, 108, 10);&#x20;

      u8g2.drawBox(12, 40, values[main_index][sub_index], 6);



      char s[20]; sprintf(s, "LEVEL: %d%%", values[main_index][sub_index]);

      u8g2.setFont(u8g2_font_6x12_tr); u8g2.drawStr(15, 60, s);&#x20;

    }



    // Help Text Navigation Guide

    u8g2.setFont(u8g2_font_u8glib_4_tf);

    u8g2.drawStr(60, 62, "2x CLICK: BACK");

  }

  u8g2.sendBuffer();

}