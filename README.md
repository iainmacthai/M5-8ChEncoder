# M5-8ChEncoder

A collection of templates showing the use of an M5 Stack Dial with An 8 Ch Encoder & Square Line Studio to Create UI's
--------------------------------------------------------
### M5-8ChEncoderV1.0

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8
+ Both screen show the toggle switch button

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
### To-Do
- [ ] Correctly Map Encoders from 0-100 to match UI ARCS
- [ ] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
### M5-8ChEncoderV1.1

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

### To-Do
- [x] Correctly Map Encoders from 0-100 to match UI ARCS
- [x] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
### M5-8ChEncoderV1.2

It Consists of a 2 screen UI on the M5Dial
+ Screen 1 Shows Encoder Channels 1-4
+ Screen 2 Shows Encoder Channels 5-8

### To-Do
- [x] Correctly Map Encoders from 0-100 to match UI ARCS
- [x] Provide Haptic Feedback with Buzzer
- [ ] Add encoder reset button
- [ ] Store encoder/switch/button values in register on Encoder unit for power down
- [ ] Display correct values on power on
- [ ] Utilise the Built-in LED's on the unit
- [ ] Incorporate MQTT/WiFI
- [ ] Create Settings Interface / Extra Screens
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] TBC
- [ ] Add splash screen
- [ ] Tidy Up Code
--------------------------------------------------------
## Mentions / Credit
Thanks to the below for inspiration / help
### Help
+ Miroslav / @dronecz :pray: <[SquareLine Studio Help Post](https://forum.squareline.io/t/how-to-use-button-to-activate-a-function/856/9)>
+ Deepseek :wink:
### Inspration
+ VolosProjects :thumbsup: @VolosR
+ 
