/*
* Basic Button with Multiple Functions.
* Must have lennarthennigs/Button2@^2.3.2 for library in platformio.ini.
*/
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "Button2.h"    // Button2 Header


/*** PINS FOR BUTTONS ***/
#define LEFT_BUTTON_PIN  33   //A0
#define RIGHT_BUTTON_PIN  14  //A3
#define UP_BUTTON_PIN  15     //A1
#define DOWN_BUTTON_PIN  32   //A2


/*** PINS FOR OLED ***/
#define OLED_SCL 22
#define OLED_SDA 23  // 21

// OLED DIMENSION
#define OLED_WIDTH u8g2.getDisplayWidth()       // 128  Pixels
#define OLED_HEIGHT u8g2.getDisplayHeight()     // 64   Pixels

#define START_DELAY 3		// Time for Start Logo Delay (in seconds).
#define IDLE_TIME 5         // Defualt Idle Countdown is 30 Seconds.            // TODO: Make this link to the settings and change it to it.

Button2 left_button, right_button, up_button, down_button;         // Name for each button.

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/ 23);   // ESP32 Thing, HW I2C with pin remapping

// Define your menu items
// Define the structure for a menu item
struct MenuItem {
    const char* name;
    int values[3]; // Assuming each menu item can have up to 3 values
};
const int MENU_ITEMS_COUNT = 4;
MenuItem menuItems[MENU_ITEMS_COUNT] = {
    {"CO2 Levels",{0 /*Low*/, 0 /*High*/}},
    {"Fan Speed",{0 /*Auto*/}},
    {"Idle Time",{0 /*Default Mode 0: 30 Seconds*/}},
    {"Time",{0 /*Hour*/, 0 /*Minutes*/, 0 /*AM/PM*/}}
};
int currentMenuItem = 0;  // Current menu item index
int selectedMenuItem = 0; // 0 Means Not Selected To lock on the Menu must have + 1


int idle_flag = 1; // 0=idle; 1=active_menu
hw_timer_t *timer = NULL;

/*** FUNCTIONS ***/
void idle_on(void){
    Serial.print("\n Idling... \n");
    idle_flag = 0;
    return;
}
void longClickDetected(Button2& btn) {
    Serial.println("long click detected");
    Serial.print("on button "); 
    Serial.print(btn.getPin());
    Serial.println(); 
}
void button_handler(Button2& btn) {
    // Reset Timer Interrupt for idle.
    idle_flag = 1;
    timerAlarmWrite(timer, IDLE_TIME * 1000000, true); // Set alarm value to seconds, disable auto-reload
    timerAlarmEnable(timer); // Enable timer alarm
    

    // Switch was for testing, Not really needed.
    switch (btn.getType()) {
        case single_click:
            Serial.print("click ");
            Serial.print("on button "); 
            Serial.print(btn.getPin());
            Serial.println(); 
            break;
        default:
            break;
    }
    
    /* UPDATES MENU NAVIGATION */
    if (selectedMenuItem == 0) {
        if (btn.getPin() == LEFT_BUTTON_PIN) {
                Serial.println("Left");
                Serial.println(); 
                //delay(400);   // Adjust the delay as needed
                currentMenuItem = (currentMenuItem - 1 + sizeof(menuItems) / sizeof(menuItems[0])) % (sizeof(menuItems) / sizeof(menuItems[0]));
            }
        if (btn.getPin() == RIGHT_BUTTON_PIN) {
            Serial.println("Right");
            Serial.println(); 
            //delay(400);   // Adjust the delay as needed
            currentMenuItem = (currentMenuItem + 1) % (sizeof(menuItems) / sizeof(menuItems[0]));
        }
    }
    
}
void init_buttons(){
    // LEFT BUTTON
    left_button.begin(LEFT_BUTTON_PIN);
    left_button.setReleasedHandler(button_handler);
    left_button.setLongClickDetectedHandler(longClickDetected); 
    /*     
    left_button.setLongClickTime(1000);
    left_button.setDoubleClickTime(400);
    */

    // RIGHT BUTTON
    right_button.begin(RIGHT_BUTTON_PIN);
    right_button.setReleasedHandler(button_handler); 
    right_button.setLongClickDetectedHandler(longClickDetected); 
    /*     
    right_button.setLongClickTime(1000);
    right_button.setDoubleClickTime(400);
    */

    // DOWN BUTTON
    down_button.begin(DOWN_BUTTON_PIN);
    down_button.setReleasedHandler(button_handler);
    down_button.setLongClickDetectedHandler(longClickDetected); 
    /*     
    down_button.setLongClickTime(1000);
    down_button.setDoubleClickTime(400);
    */


    // UP BUTTON
    up_button.begin(UP_BUTTON_PIN);
    up_button.setReleasedHandler(button_handler); 
    up_button.setLongClickDetectedHandler(longClickDetected); 
    /*     
    up_button.setLongClickTime(1000);
    up_button.setDoubleClickTime(400);
    */
}
void button_loops() {
    left_button.loop();
    right_button.loop();
    up_button.loop();
    down_button.loop();
}

/* DISPLAY OPTIONS */
void idle_display(void){    // display_main_screen (from oled.h)
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
    u8g2.drawStr(0,10,"Displays Temperatures");	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
    delay(1000); 
    return; 
}


void start_display(void){
    u8g2.clearBuffer();					    // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);	    // choose a suitable font
    u8g2.drawStr(OLED_WIDTH/2,OLED_HEIGHT/2,"Auto Pilot");	// write something to the internal memory
    u8g2.sendBuffer();					    // transfer internal memory to the display
    delay(START_DELAY*1000);                  //
    return;
}

/*** SETUP ***/
void setup() {
    Serial.begin(9600);
    
    u8g2.begin();
    
    timer = timerBegin(0, 80, true); // Initialize timer

    delay(50);

    Serial.println("\nSmart Vent \n");

    init_buttons();
    
    timerAttachInterrupt(timer, &idle_on, true); // Attach interrupt
    timerAlarmWrite(timer, IDLE_TIME * 1000000, true); // Set alarm value to seconds, disable auto-reload
    timerAlarmEnable(timer); // Enable timer alarm

    start_display();
}


/*** LOOP ***/
void loop() {

  // Clear the display
  u8g2.firstPage();
  do {
    button_loops();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);	// primary font
    //u8g.setFont(u8g_font_10x20);  // secondary font
    // Selecting Menu Item
    if (idle_flag == 0){
        // idle_display();
        int centerX = (OLED_WIDTH - u8g2.getStrWidth("Idle Display")) / 2;
        int centerY = (OLED_HEIGHT - (u8g2.getFontAscent() - u8g2.getFontDescent())) / 2;
        u8g2.drawStr(centerX, centerY, "Idle Display");	// write something to the internal memory
    }
    else {
        if (selectedMenuItem == 0){
                // Center the text horizontally and vertically
                int centerX = (OLED_WIDTH - u8g2.getStrWidth(menuItems[currentMenuItem].name )) / 2;
                int centerY = (OLED_HEIGHT - (u8g2.getFontAscent() - u8g2.getFontDescent())) / 2;
                u8g2.drawStr(centerX, centerY, menuItems[currentMenuItem].name);	// write something to the internal memory
            }
    }
    
    
    u8g2.sendBuffer();					// transfer internal memory to the display
  } while (u8g2.nextPage());  
}