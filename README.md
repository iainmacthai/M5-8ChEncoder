# M5-8ChEncoder

A collection of templates showing the use of an M5 Stack Dial with An 8 Ch Encoder & Square Line Studio to Create UI's

![SLS 2 page menu](https://github.com/user-attachments/assets/78262991-b92f-4e33-8160-df45b31528e1)

--------------------------------------------------------
## M5-8ChEncoderV1.0

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8


+ There is a separate void UpdateCH"n"() for each encoder
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
+ The UI Encoder Arc's displaying the Encoder Values are set from 0-100
+ The encoders constantly rotate and are not clamped so the UI Arcs reach 100 or 0 and wrap around with positive and negative numbers
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
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## M5-8ChEncoderV1.1

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
+ Both screen show the toggle switch button

+ (CHANGE) There is a single void UpdateChannel() which sets the encoder range from 0-100 and add some haptic feedback at 0 or 100 with a buzzer
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
+ (CHANGE)The void UpdateChannel which is applied to each void UpdateCH'n'() So that each channels encoder now works correctly in ranges 0-100 with haptic feedback
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


+ The UI Encoder Arc's displaying the Encoder Values are set from 0-100
+ The encoders constantly rotate and are not clamped so the UI Arcs reach 100 or 0 and wrap around with positive and negative numbers

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
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## M5-8ChEncoderV1.2

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
+ (CHANGE) modified UpdateChannel function: Changes the functionality of the button on each encoder value to make the encoder "Active" or Locked" and stores the value
   + Add Lock State Tracking: Introduce a locked array to track whether each encoder channel is locked
   + Modify Encoder Delta Handling: Only apply encoder changes when the channel is unlocked.
   + Toggle Lock State on Button Press: Update the lock state when the button is pressed and provide feedback.
   + Update Button Label: Reflect the lock state instead of the button's physical state.
modified UpdateChannel function:
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
#### Key Changes:
+ Lock State: The locked array tracks whether adjustments are allowed for each encoder.
+ Delta Handling: Encoder values only update when the channel is unlocked.
+ Button Toggle: Pressing the button toggles the lock state (on rising edge) and updates the label.
+ Visual/Audible Feedback: The button label shows "LOCKED" (red) or "ACTIVE" (green), and a beep confirms the toggle.
#### Usage:
+ When the button is pressed, the current value is locked, and the encoder stops affecting it.
+ Press the button again to unlock and resume adjustments.
+ The locked value (current_val) can be used elsewhere in your code as needed.
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
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## M5-8ChEncoderV1.2.1

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
#### Declare Global Variables:
```
#include "ui_helpers.h"
#include "UNIT_8ENCODER.h"

UNIT_8ENCODER sensor;
// Global encoder state tracking
int current_val[8] = {0};  //  ADD THIS LINE for V1.2.1 
bool locked[8] = {false};  // ADD THIS LINE for V1.2.1:
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


#### Features:
+ Updates exactly once per second using non-blocking millis()
+ Uses global variables to track encoder states.

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
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code

--------------------------------------------------------
## Mentions / Credit
Thanks to the below for inspiration / help
#### Help
+ Miroslav / @dronecz :pray: <[SquareLine Studio Help Post](https://forum.squareline.io/t/how-to-use-button-to-activate-a-function/856/9)>
+ Deepseek :wink:
#### Inspration
+ VolosProjects :thumbsup: @VolosR
+ 

