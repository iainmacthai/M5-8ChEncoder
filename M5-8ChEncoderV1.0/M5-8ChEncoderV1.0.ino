#include "M5Dial.h"
#include <Arduino.h>
#include <lvgl.h>
//#include <Wire.h>  // need to confirm this later
#include <SPI.h>
#include "ui.h"
#include "ui_helpers.h"
#include "UNIT_8ENCODER.h"

UNIT_8ENCODER sensor;
/**
 * UNIT 8Encoder Reference

    int32_t getEncoderValue(uint8_t index);
    void setEncoderValue(uint8_t index, int32_t value);
    int32_t getIncrementValue(uint8_t index);
    bool getButtonStatus(uint8_t index);
    bool getSwitchStatus(void);
    void setLEDColor(uint8_t index, uint32_t color);
    void setAllLEDColor(uint32_t color);
    uint8_t setI2CAddress(uint8_t addr);
    uint8_t getI2CAddress(void);
    uint8_t getFirmwareVersion(void);
    void resetCounter(uint8_t index);
 */



// LVGL Display Variables
static lv_disp_draw_buf_t draw_buf;      // Display buffer
static lv_disp_drv_t disp_drv;           // Display driver descriptor
static lv_indev_drv_t indev_drv;         // Touch driver descriptor


// Display Configuration
#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 240
#define LV_VER_RES_MAX 240
#define LV_HOR_RES_MAX 240
M5GFX *tft;
#define WIRE Wire


/**
 * Initialize LVGL and display buffers
 */
void tft_lv_initialization() {
    lv_init();
    
    // Declare buffers for 1/10 of screen size (double buffering)
    static lv_color_t buf1[(LV_HOR_RES_MAX * LV_VER_RES_MAX) / 4];
    static lv_color_t buf2[(LV_HOR_RES_MAX * LV_VER_RES_MAX) / 4];
    
    // Initialize display buffer with the buffers
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (LV_HOR_RES_MAX * LV_VER_RES_MAX) / 4);
    
    // Assign M5Dial LCD to our tft pointer
    tft = &M5Dial.Lcd;
}

/**
 * Display flushing callback for LVGL
 * @param disp Pointer to display driver
 * @param area Area to flush
 * @param color_p Pointer to color data
 */
void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    tft->startWrite();
    tft->setAddrWindow(area->x1, area->y1, w, h);
    tft->pushColors((uint16_t*)&color_p->full, w * h, true);
    tft->endWrite();
    
    lv_disp_flush_ready(disp);  // Inform LVGL flushing is done
}

/**
 * Initialize LVGL display driver
 */
void init_disp_driver() {
    lv_disp_drv_init(&disp_drv);          // Basic initialization
    disp_drv.flush_cb = my_disp_flush;    // Set flush callback
    disp_drv.draw_buf = &draw_buf;        // Assign display buffer
    disp_drv.hor_res = LV_HOR_RES_MAX;    // Set horizontal resolution
    disp_drv.ver_res = LV_VER_RES_MAX;    // Set vertical resolution
    
    lv_disp_drv_register(&disp_drv);                     // Register the driver
    lv_disp_set_bg_color(NULL, lv_color_hex3(0x000));    // Set black background
}

/**
 * Touchpad read callback for LVGL
 * @param drv Pointer to input device driver
 * @param data Pointer to data to be filled
 */
void my_touchpad_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    uint32_t currentTime = millis();
    M5.Touch.update(currentTime);

    if (M5.Touch.getCount() > 0) {
        auto pos = M5.Touch.getDetail();
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = pos.x;
        data->point.y = pos.y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

/**
 * Initialize LVGL touch driver
 */
void init_touch_driver() {
    lv_disp_drv_register(&disp_drv);
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_t* my_indev = lv_indev_drv_register(&indev_drv);  // Register driver
}

/**
 * Arduino setup function - runs once at startup
 */
void setup() {
    // Initialize M5Dial hardware
    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);
    M5Dial.Display.setBrightness(80);
    // Initialize serial communication
    Serial.begin(115200);
    // Initialize display and touch interfaces
    tft_lv_initialization();
    init_disp_driver();
    init_touch_driver();
    // Initialize UI (from ui.h)
    ui_init();
    // Initialize Custom I2C
    WIRE.begin(13,15);
    sensor.begin(&Wire, ENCODER_ADDR, 13, 15, 100000UL);    

}

void UpdateCH1()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(0); // Channel 1
  bool btn = sensor.getButtonStatus(0);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc1Arc, val);
    lv_label_set_text_fmt(ui_Enc1ValueLabel, "%d", val);
    last_encoder = encoder;
  }
if(btn != last_btn) {
    lv_label_set_text(ui_Enc1BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc1BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}

void UpdateCH2()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(1); // Channel 2
  bool btn = sensor.getButtonStatus(1);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc2Arc, val);
    lv_label_set_text_fmt(ui_Enc2ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc2BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc2BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}

void UpdateCH3()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(2); // Channel 3
  bool btn = sensor.getButtonStatus(2);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc3Arc, val);
    lv_label_set_text_fmt(ui_Enc3ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc3BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc3BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}

void UpdateCH4()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(3); // Channel 4
  bool btn = sensor.getButtonStatus(3);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc4Arc, val);
    lv_label_set_text_fmt(ui_Enc4ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc4BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc4BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}

void UpdateCH5()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(4); // Channel 5
  bool btn = sensor.getButtonStatus(4);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc5Arc, val);
    lv_label_set_text_fmt(ui_Enc5ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc5BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc5BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}

void UpdateCH6()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(5); // Channel 6
  bool btn = sensor.getButtonStatus(5);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc6Arc, val);
    lv_label_set_text_fmt(ui_Enc6ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc6BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc6BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
}
}

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

void UpdateCH8()
{
static int last_encoder = 0;
static bool last_btn = false;
   // Read encoder
  int encoder = sensor.getEncoderValue(7); // Channel 8
  bool btn = sensor.getButtonStatus(7);
  // Update arc if changed
  if(encoder != last_encoder) {
    int val = encoder % 100;
    lv_arc_set_value(ui_Enc8Arc, val);
    lv_label_set_text_fmt(ui_Enc8ValueLabel, "%d", val);
    last_encoder = encoder;
  }
  // Update button if changed
  if(btn != last_btn) {
    lv_label_set_text(ui_Enc8BtnValueLabel, btn ? "OFF" : "ON");
    // Change colour based on state: RED OFF (0xFF0000) GREEN ON (0x00FF00) 
    lv_obj_set_style_text_color(ui_Enc8BtnValueLabel, btn ? lv_color_hex(0xFF0000) : lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);    
    last_btn = btn;
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
M5Dial.update();

}

