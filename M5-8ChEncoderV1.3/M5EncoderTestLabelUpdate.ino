#include "M5Dial.h"
#include <Arduino.h>
#include <lvgl.h>
#include <SPI.h>
#include "ui.h"
#include "ui_helpers.h"
#include "UNIT_8ENCODER.h"

UNIT_8ENCODER sensor;
// Global encoder state tracking
int current_val[8] = {0};  // added for version 1.2.1 
bool locked[8] = {false};  // added for version 1.2.1 

// LVGL Display Variables
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

// Display Configuration
#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 240
#define LV_VER_RES_MAX 240
#define LV_HOR_RES_MAX 240
M5GFX *tft;
#define WIRE Wire

// Function prototypes
void UpdateChannel(uint8_t channel, lv_obj_t* arc, lv_obj_t* valueLabel, lv_obj_t* btnLabel);
void UpdateCH1();
void UpdateCH2();
void UpdateCH3();
void UpdateCH4();
void UpdateCH5();
void UpdateCH6();
void UpdateCH7();
void UpdateCH8();
void UpdateToggleSwitch();

void tft_lv_initialization() {
    lv_init();
    static lv_color_t buf1[(LV_HOR_RES_MAX * LV_VER_RES_MAX) / 4];
    static lv_color_t buf2[(LV_HOR_RES_MAX * LV_VER_RES_MAX) / 4];
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (LV_HOR_RES_MAX * LV_VER_RES_MAX) / 4);
    tft = &M5Dial.Lcd;
}

void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    tft->startWrite();
    tft->setAddrWindow(area->x1, area->y1, w, h);
    tft->pushColors((uint16_t*)&color_p->full, w * h, true);
    tft->endWrite();
    lv_disp_flush_ready(disp);
}

void init_disp_driver() {
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    lv_disp_drv_register(&disp_drv);
    lv_disp_set_bg_color(NULL, lv_color_hex3(0x000));
}

void my_touchpad_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    M5.Touch.update(millis());
    if (M5.Touch.getCount() > 0) {
        auto pos = M5.Touch.getDetail();
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = pos.x;
        data->point.y = pos.y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void init_touch_driver() {
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

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
    delay(100);    //small hardware delay                 // ADD for V1.3
    for (uint8_t i = 0; i < 8; i++) {                     // ADD for V1.3
        sensor.setLEDColor(i, 0x0000ff);                  // ADD for V1.3
    }                                                     // ADD for V1.3
    sensor.setLEDColor(8, 0xFFDE21);                      // ADD for V1.3
    delay(1000);                                          // ADD for V1.3

}
/*
// Original V1.1 Consolidated UpdateChannel function
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
*/

// modified V1.2 UpdateChannel function: //
void UpdateChannel(uint8_t channel, lv_obj_t* arc, lv_obj_t* valueLabel, lv_obj_t* btnLabel) {
    //static int current_val[8] = {0}; // Removed for V1.2.1
    static bool last_btn[8] = {false};
    static int last_val[8] = {0};    
    //static bool locked[8] = {false}; // Removed for V1.2.1 

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
            // Update RGB LED (channels 1-8, not 0-7)
            sensor.setLEDColor(                                        // ADD for V1.3    
                channel,                                               // ADD for V1.3
                locked[channel] ? 0xFF0000 : 0x00FF00 // Red : Green   // ADD for V1.3
            );                                                         // ADD for V1.3
        }
        last_btn[channel] = btn;

        // Update button label to reflect lock state
        lv_label_set_text(btnLabel, locked[channel] ? "LOCKED" : "ACTIVE");
        lv_obj_set_style_text_color(btnLabel, locked[channel] ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}




// Simplified channel update functions
void UpdateCH1() { UpdateChannel(0, ui_Enc1Arc, ui_Enc1ValueLabel, ui_Enc1BtnValueLabel); }
void UpdateCH2() { UpdateChannel(1, ui_Enc2Arc, ui_Enc2ValueLabel, ui_Enc2BtnValueLabel); }
void UpdateCH3() { UpdateChannel(2, ui_Enc3Arc, ui_Enc3ValueLabel, ui_Enc3BtnValueLabel); }
void UpdateCH4() { UpdateChannel(3, ui_Enc4Arc, ui_Enc4ValueLabel, ui_Enc4BtnValueLabel); }
void UpdateCH5() { UpdateChannel(4, ui_Enc5Arc, ui_Enc5ValueLabel, ui_Enc5BtnValueLabel); }
void UpdateCH6() { UpdateChannel(5, ui_Enc6Arc, ui_Enc6ValueLabel, ui_Enc6BtnValueLabel); }
void UpdateCH7() { UpdateChannel(6, ui_Enc7Arc, ui_Enc7ValueLabel, ui_Enc7BtnValueLabel); }
void UpdateCH8() { UpdateChannel(7, ui_Enc8Arc, ui_Enc8ValueLabel, ui_Enc8BtnValueLabel); }

void UpdateToggleSwitch() {
    static bool last_toggle = false;
    bool toggle = sensor.getSwitchStatus();
    if (toggle != last_toggle) {
        lv_label_set_text(ui_ToggleSwitchStatelabel1, toggle ? "OFF" : "ON");
        lv_label_set_text(ui_ToggleSwitchStatelabel2, toggle ? "OFF" : "ON");
        lv_obj_set_style_text_color(ui_ToggleSwitchStatelabel1, toggle ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ToggleSwitchStatelabel2, toggle ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        toggle ? lv_obj_clear_state(ui_Switch1, LV_STATE_CHECKED) : lv_obj_add_state(ui_Switch1, LV_STATE_CHECKED);
        last_toggle = toggle;
    }
}

void loop() {
    uint32_t wait_ms = lv_timer_handler();
    M5.delay(wait_ms);
    UpdateCH1();
    UpdateCH2();
    UpdateCH3();
    UpdateCH4();
    UpdateCH5();
    UpdateCH6();
    UpdateCH7();
    UpdateCH8();
    UpdateToggleSwitch();
    M5Dial.update();
        // Print encoder states every second // added for version 1.2.1
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