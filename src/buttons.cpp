/**
 * @file  buttons.cpp
 * @copyright Copyright (c) 2024
 */

#include "buttons.h"
#include "common.h"
#include "oled.h"

void button_up_handler_wrapper(Button2 &btn);
void button_down_handler_wrapper(Button2 &btn);
void button_left_handler_wrapper(Button2 &btn);
void button_right_handler_wrapper(Button2 &btn);
void button_left_long_press_handler_wrapper(Button2 &btn);
int buf = 0;

extern OLED oled;
extern MHZ19B mhz19b;

Buttons::Buttons() {
  button_ticker = new TickTwo(std::bind(&Buttons::button_ticker_handler, this), 100);
}

void Buttons::init() {
  button_up.begin(BUTTON_UP_PIN);
  button_down.begin(BUTTON_DOWN_PIN);
  button_left.begin(BUTTON_LEFT_PIN);
  button_right.begin(BUTTON_RIGHT_PIN);

  button_up.setReleasedHandler(button_up_handler_wrapper);
  button_down.setReleasedHandler(button_down_handler_wrapper);
  button_left.setReleasedHandler(button_left_handler_wrapper);
  button_right.setReleasedHandler(button_right_handler_wrapper);

  button_left.setLongClickTime(3000);
  button_left.setLongClickDetectedHandler(button_left_long_press_handler_wrapper);

  button_ticker->start();
}

void Buttons::loop() {
  button_ticker->update();
  button_up.loop();
  button_down.loop();
  button_left.loop();
  button_right.loop();
}

void Buttons::button_up_handler(Button2 &btn) {
  Serial.println("Button up pressed");
  oled.main_screen_idle_ticker->start();
  if (oled.current_screen == MENU_SCREEN) {
    oled.curr_menu_item = decrement(oled.curr_menu_item, oled.menu_items_count);
  } 
  // Selected Menu Item
  else if (oled.current_screen == MENU_ITEM_SCREEN) {
    // Termperature Unit
    if (oled.curr_menu_item == 0) {
      buf += 1;
      Serial.printf("%d\n", buf);
    }
  } 

}

void Buttons::button_down_handler(Button2 &btn) {
  Serial.println("Button down pressed");
  oled.main_screen_idle_ticker->start();
  if (oled.current_screen == MENU_SCREEN) {
    oled.curr_menu_item = increment(oled.curr_menu_item, oled.menu_items_count);
  } 
  // Selected Menu Item
  else if (oled.current_screen == MENU_ITEM_SCREEN) {
    // Termperature Unit
    if (oled.curr_menu_item == 0) {
      buf += 1;
      Serial.printf("%d\n", buf);
    }
  } 

}

void Buttons::button_left_handler(Button2 &btn) {
  Serial.println("Button left pressed");
  oled.main_screen_idle_ticker->start();
  if (oled.current_screen == MENU_SCREEN) {
    oled.current_screen = MAIN_SCREEN;
  } else if (oled.current_screen == MENU_ITEM_SCREEN) {
    oled.current_screen = MENU_SCREEN;
  } 
  // Discard Changed Items
  else if (oled.current_screen == MENU_ITEM_SCREEN) {
    // Termperature Unit
    if (oled.curr_menu_item == 0) {
      Serial.printf("Discarding Changes.\n");
      oled.current_screen = MENU_SCREEN;
    }
  } 
}

void Buttons::button_right_handler(Button2 &btn) {
  Serial.println("Button right pressed");
  oled.main_screen_idle_ticker->start();
  if (oled.current_screen == MAIN_SCREEN) {
    oled.current_screen = MENU_SCREEN;
  } else if (oled.current_screen == MENU_SCREEN) {
    oled.current_screen = MENU_ITEM_SCREEN;
    if (oled.curr_menu_item == 0) {
      mhz19b.get_unit() == CELSIUS ? (buf = 0) : (buf = 1);
    }
  }
  // Saving Changed Items
  else if (oled.current_screen == MENU_ITEM_SCREEN) {
    // Termperature Unit
    if (oled.curr_menu_item == 0) {
      //mhz19b.set_unit((buf % 2) == 1 ? FAHRENHEIT : CELSIUS);
      if ((buf)%2 == 1) {
        mhz19b.set_unit(FAHRENHEIT);
        Serial.printf("Setting to: %d\n", buf%2);
      }
      else {
        mhz19b.set_unit(CELSIUS);
        Serial.printf("Setting to: %d\n", buf%2);
      }
      oled.current_screen = MENU_SCREEN;
    }
  } 
}

void Buttons::button_left_long_press_handler(Button2 &btn) {
  Serial.println("Button left long press");
  if (oled.current_screen == MAIN_SCREEN) {
    oled.toggle();
  }
}

void Buttons::button_ticker_handler() {
  // Serial.println("Button ticker");
}

/* BUTTONS OBJECT (defined in main.cpp) & WRAPPERS FOR BUTTON HANDLERS */
extern Buttons buttons;

void button_up_handler_wrapper(Button2 &btn) {
  buttons.button_up_handler(btn);
}

void button_down_handler_wrapper(Button2 &btn) {
  buttons.button_down_handler(btn);
}

void button_left_handler_wrapper(Button2 &btn) {
  buttons.button_left_handler(btn);
}

void button_right_handler_wrapper(Button2 &btn) {
  buttons.button_right_handler(btn);
}

void button_left_long_press_handler_wrapper(Button2 &btn) {
  buttons.button_left_long_press_handler(btn);
}