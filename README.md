# M5-8ChEncoder

A collection of templates showing the use of an M5 Stack Dial with An 8 Ch Encoder & Square Line Studio to Create UI's

![SLS 2 page menu](https://github.com/user-attachments/assets/78262991-b92f-4e33-8160-df45b31528e1)

## Contents
- [Section 2](#section-2)
- [Section 3](#5-8ChEncoderV1.2.1)
- [Version 1.0](#M5-8ChEncoderV1.0)
- [Version 1.1](#M5-8ChEncoderV1.1)
- [Version 1.2](#M5-8ChEncoderV1.2)
- - [Version 1.2.1](#M5-8ChEncoderV1.2.1)
- [Version 1.3](#M5-8ChEncoderV1.3)
- [Credits / Mentions](#Credits-/-Mentions)
--------------------------------------------------------
## Version 1.0

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
+ The UI Encoder Arc's displaying the Encoder Values are set from 0-100
+ The Encoders constantly rotate and are not clamped so the UI Arcs reach 100 or 0 and wrap around with positive and negative numbers
+ For Simplicity there is a separate void UpdateCH"n"() for each encoder
```
// Separate UpdateChannel function
void UpdateCH7()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(6); // Channel 7
  bool btn = sensor.getButtonStatus(6);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc7Arc, val);
    lv_label_set_text_fmt(ui_Enc7ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc7BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc7BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}
```

#### To-Do
- [ ] Correctly Map Encoders from 0-100 to match UI ARCS
- [ ] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [x] Add Active / Locked function with button press for each channel
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [x] Add toggle switch to screen 2
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add Option where toggle switch changes from showing encoder value to a text option mapped to the value
   - [ ] of the encoder from a predefined list (0-10 = send mqqt message A, 11-20 = send mqtt message B etc)
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## Version 1.1

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
### Enhancement
#### Create a single consolidated update channel function
+  There is a simpler single void UpdateChannel() which sets the encoder range from 0-100 and add some haptic feedback at 0 or 100 with a buzzer
```
// Consolidated UpdateChannel function
void UpdateChannel(uint8_t channel, lv_obj_t* arc, lv_obj_t* valueLabel, lv_obj_t* btnLabel) {
    static int current_val[8] = {0};
    static bool last_btn[8] = {false};
    static int last_val[8] = {0};

    int delta = sensor.getIncrementValue(channel);
    bool btn = sensor.getButtonStatus(channel);

    int new_val = constrain(current_val[channel] + delta, 0, 100);
    
    if (new_val != (current_val[channel] + delta)) {
        M5Dial.Speaker.tone(5000, 250);
    }

    current_val[channel] = new_val;

    if (current_val[channel] != last_val[channel]) {
        lv_arc_set_value(arc, current_val[channel]);
        lv_label_set_text_fmt(valueLabel, "%d", current_val[channel]);
        last_val[channel] = current_val[channel];
    }

    if (btn != last_btn[channel]) {
        lv_label_set_text(btnLabel, btn ? "OFF" : "ON");
        lv_obj_set_style_text_color(btnLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        last_btn[channel] = btn;
    }
}
```
#### Apply that to each channel update in the main loop
+ The void UpdateChannel which is applied to each void UpdateCH'n'() So that each channels encoder now works correctly in ranges 0-100 with haptic feedback
```
void UpdateCH1() { UpdateChannel(0, ui_Enc1Arc, ui_Enc1ValueLabel, ui_Enc1BtnValueLabel); }
void UpdateCH2() { UpdateChannel(1, ui_Enc2Arc, ui_Enc2ValueLabel, ui_Enc2BtnValueLabel); }
void UpdateCH3() { UpdateChannel(2, ui_Enc3Arc, ui_Enc3ValueLabel, ui_Enc3BtnValueLabel); }
void UpdateCH4() { UpdateChannel(3, ui_Enc4Arc, ui_Enc4ValueLabel, ui_Enc4BtnValueLabel); }
void UpdateCH5() { UpdateChannel(4, ui_Enc5Arc, ui_Enc5ValueLabel, ui_Enc5BtnValueLabel); }
void UpdateCH6() { UpdateChannel(5, ui_Enc6Arc, ui_Enc6ValueLabel, ui_Enc6BtnValueLabel); }
void UpdateCH7() { UpdateChannel(6, ui_Enc7Arc, ui_Enc7ValueLabel, ui_Enc7BtnValueLabel); }
void UpdateCH8() { UpdateChannel(7, ui_Enc8Arc, ui_Enc8ValueLabel, ui_Enc8BtnValueLabel); }
```

#### To-Do
- [x] Correctly Map Encoders from 0-100 to match UI ARCS
- [x] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [x] Add Active / Locked function with button press for each channel
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [x] Add toggle switch to screen 2
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add Option where toggle switch changes from showing encoder value to a text option mapped to the value
   - [ ] of the encoder from a predefined list (0-10 = send mqqt message A, 11-20 = send mqtt message B etc)
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## Version 1.2

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
### Enhancement
#### Create a single consolidated update channel function
   + Add Lock State Tracking: Introduce a locked array to track whether each encoder channel is locked
   + Modify Encoder Delta Handling: Only apply encoder changes when the channel is unlocked.
   + Button Toggle: Pressing the button toggles the lock state (on rising edge) and updates the label and provides buzzer feedback.
   + Update Button Label: Reflect the lock state instead of the button's physical state.
#### Modified UpdateChannel function:
```
void UpdateChannel(uint8_t channel, lv_obj_t* arc, lv_obj_t* valueLabel, lv_obj_t* btnLabel) {
    static int current_val[8] = {0};
    static bool last_btn[8] = {false};
    static int last_val[8] = {0};
    static bool locked[8] = {false}; // Tracks lock state for each channel

    int delta = sensor.getIncrementValue(channel);
    bool btn = sensor.getButtonStatus(channel);

    // Apply encoder delta only if the channel is unlocked
    if (!locked[channel]) {
        int new_val = current_val[channel] + delta;
        new_val = constrain(new_val, 0, 100);

        // Play sound if the value hits min/max
        if (new_val != (current_val[channel] + delta)) {
            M5Dial.Speaker.tone(5000, 250);
        }

        current_val[channel] = new_val;

        // Update UI if value changed
        if (current_val[channel] != last_val[channel]) {
            lv_arc_set_value(arc, current_val[channel]);
            lv_label_set_text_fmt(valueLabel, "%d", current_val[channel]);
            last_val[channel] = current_val[channel];
        }
    }

    // Handle button press to toggle lock state
    if (btn != last_btn[channel]) {
        if (btn) { // Rising edge (button pressed)
            locked[channel] = !locked[channel];
            M5Dial.Speaker.tone(3000, 100); // Audible feedback on toggle
        }
        last_btn[channel] = btn;

        // Update button label to reflect lock state
        lv_label_set_text(btnLabel, locked[channel] ? "LOCKED" : "ACTIVE");
        lv_obj_set_style_text_color(btnLabel, locked[channel] ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}
```
#### Usage:
+ When the button is pressed, the current value is locked, and the encoder stops affecting it.
+ Press the button again to unlock and resume adjustments.

#### To-Do
- [x] Correctly Map Encoders from 0-100 to match UI ARCS
- [x] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [x] Add Active / Locked function with button press for each channel
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [x] Add toggle switch to screen 2
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add Option where toggle switch changes from showing encoder value to a text option mapped to the value
   - [ ] of the encoder from a predefined list (0-10 = send mqqt message A, 11-20 = send mqtt message B etc)
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## Version 1.2.1

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8

### Enhancement
#### Create a global variables to be used elsewhere and add serial debugging
+ Uses global variables to track encoder states.
+ Add serial debugging of encoder values and states to check it matches the UI
   + Updates once per second using non-blocking millis()
#### Declare Global Variables:
```
#include "ui_helpers.h"
#include "UNIT_8ENCODER.h"

UNIT_8ENCODER sensor;
// Global encoder state tracking
int current_val[8] = {0};  // ✅ ADD THIS LINE for V1.2.1 
bool locked[8] = {false};  // ✅ ADD THIS LINE for V1.2.1:
```
#### Remove Static Variables:
```
void UpdateChannel(uint8_t channel, lv_obj_t* arc, lv_obj_t* valueLabel, lv_obj_t* btnLabel) {
    // Remove these lines to use global variables
    // static int current_val[8] = {0};  // ❌ DELETE THIS LINE for V1.2.1
    static bool last_btn[8] = {false};
    static int last_val[8] = {0};
    //static bool locked[8] = {false}; // ❌ DELETE THIS LINE for V1.2.1

    int delta = sensor.getIncrementValue(channel);  
    bool btn = sensor.getButtonStatus(channel);

    // The code will now use the global current_val array
    // ... (rest of the function remains unchanged)
}

```
#### Add Serial Output Code:
```
void loop() {
    // Existing code...
    M5Dial.update();

    // Print encoder states every second
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 1000) {
        lastPrint = millis();
        for (int i = 0; i < 8; i++) {
            Serial.printf("Encoder %d: Value=%-3d  State=%s\n",
                i+1,
                current_val[i],
                locked[i] ? "LOCKED" : "ACTIVE"
            );
        }
        Serial.println("---------------------");
    }
}
```
#### Example Output:

```
Encoder 1: Value=47   State=ACTIVE
Encoder 2: Value=89   State=LOCKED
Encoder 3: Value=12   State=ACTIVE
...
---------------------
```

#### To-Do
- [x] Correctly Map Encoders from 0-100 to match UI ARCS
- [x] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [x] Add Active / Locked function with button press for each channel
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [x] Add toggle switch to screen 2
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add Option where toggle switch changes from showing encoder value to a text option mapped to the value
   - [ ] of the encoder from a predefined list (0-10 = send mqqt message A, 11-20 = send mqtt message B etc)
- [ ] Add splash screen
- [ ] Tidy Up Code

--------------------------------------------------------
## Version 1.3

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8

### Enhancement
#### Add LED functionality on Encoder Unit
+ LED's Track the state change of each encoder (Red Locked - Green Active) mimicking the UI.

#### Initialize LEDs in void setup():
```
void setup() {
    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);
    M5Dial.Display.setBrightness(80);
    Serial.begin(115200);
    tft_lv_initialization();
    init_disp_driver();
    init_touch_driver();
    ui_init();
    WIRE.begin(13,15);
    sensor.begin(&Wire, ENCODER_ADDR, 13, 15, 100000UL);
    // Initialize LEDs 1-8 to Blue & Status LED to Yellow  
    delay(100);    //small hardware delay                 ✅// ADD for V1.3
    for (uint8_t i = 0; i < 8; i++) {                     ✅// ADD for V1.3
        sensor.setLEDColor(i, 0x0000ff);                  ✅// ADD for V1.3
    }                                                     ✅// ADD for V1.3
    sensor.setLEDColor(8, 0xFFDE21);                      ✅// ADD for V1.3
    delay(1000);                                          ✅// ADD for V1.3

}

```
#### Link to button state check in void UpdateChannel():
```
// Handle button press to toggle lock state
    if (btn != last_btn[channel]) {
        if (btn) { // Rising edge (button pressed)
            locked[channel] = !locked[channel];
            M5Dial.Speaker.tone(3000, 100); // Audible feedback on toggle
            // Update RGB LED (channels 1-8, not 0-7)
            sensor.setLEDColor(                                        ✅// ADD for V1.3    
                channel,                                               ✅// ADD for V1.3
                locked[channel] ? 0xFF0000 : 0x00FF00 // Red : Green   ✅// ADD for V1.3
            );                                                         ✅// ADD for V1.3
        }
        last_btn[channel] = btn;

        // Update button label to reflect lock state
        lv_label_set_text(btnLabel, locked[channel] ? "LOCKED" : "ACTIVE");
        lv_obj_set_style_text_color(btnLabel, locked[channel] ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

```


#### To-Do
- [x] Correctly Map Encoders from 0-100 to match UI ARCS
- [x] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [x] Add Active / Locked function with button press for each channel
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [x] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [x] Add toggle switch to screen 2
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add Option where toggle switch changes from showing encoder value to a text option mapped to the value
   - [ ] of the encoder from a predefined list (0-10 = send mqqt message A, 11-20 = send mqtt message B etc)
- [ ] Add splash screen
- [ ] Tidy Up Code
-------------------------------------------------------------------------------------
## Credits / Mentions
Thanks to the below for inspiration / help
#### Help
+ Miroslav / @dronecz :pray: <[SquareLine Studio Help Post](https://forum.squareline.io/t/how-to-use-button-to-activate-a-function/856/9)>
+ Deepseek :wink:
#### Inspration
+ VolosProjects :thumbsup: @VolosR
+ 

