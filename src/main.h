#include <pebble.h>
#pragma once

#define SETTINGS_KEY 1
#define NUM_CLOCK_TICKS 11

// A structure containing our settings
typedef struct ClaySettings {
  char SelectClock;
  
  GColor LeftStripeColor;
  GColor RightStripeColor;
  GColor WatchBandColor;
  GColor WatchFaceColor;
  GColor TextColor;
  GColor BatteryColor;
  GColor HandsColor;
  GColor SecondHandColor;
  
  bool InvertOutline;
  bool BatteryBarToggle;
  bool StrapDetails;
  bool HourDots; 
} __attribute__((__packed__)) ClaySettings;



static void update_time();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void battery_callback(BatteryChargeState state);
static void battery_update_proc(Layer *layer, GContext *ctx);
static void canvas_update_proc(Layer *layer, GContext *ctx);
static void hands_update_proc(Layer *layer, GContext *ctx);
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed);
static void default_settings();
static void load_settings();
static void save_settings();
static void update_display();
static void inbox_received_handler(DictionaryIterator *iter, void *context);
static void window_load(Window *window);
static void window_unload(Window *window);
static void init(void);
static void deinit(void);



static const struct GPathInfo ANALOG_BG_POINTS[] = {
  { 4, (GPoint []) {
      {68, 0},
      {71, 0},
      {71, 12},
      {68, 12}
    }
  },
  { 4, (GPoint []) {
      {72, 0},
      {75, 0},
      {75, 12},
      {72, 12}
    }
  },
  { 4, (GPoint []) {
      {112, 10},
      {114, 12},
      {108, 23},
      {106, 21}
    }
  },
  { 4, (GPoint []) {
      {132, 47},
      {144, 40},
      {144, 44},
      {135, 49}
    }
  },
  { 4, (GPoint []) {
      {135, 118},
      {144, 123},
      {144, 126},
      {132, 120}
    }
  },
  { 4, (GPoint []) {
      {108, 144},
      {114, 154},
      {112, 157},
      {106, 147}
    }
  },
  { 4, (GPoint []) {
      {70, 155},
      {73, 155},
      {73, 167},
      {70, 167}
    }
  },
  { 4, (GPoint []) {
      {32, 10},
      {30, 12},
      {36, 23},
      {38, 21}
    }
  },
  { 4, (GPoint []) {
      {12, 47},
      {-1, 40},
      {-1, 44},
      {9, 49}
    }
  },
  { 4, (GPoint []) {
      {9, 118},
      {-1, 123},
      {-1, 126},
      {12, 120}
    }
  },
  { 4, (GPoint []) {
      {36, 144},
      {30, 154},
      {32, 157},
      {38, 147}
    }
  }
};



static const GPathInfo MINUTE_HAND_POINTS = {
  2, (GPoint []) {
    { 0, 0},
    { 0, -45}
  }
};



static const GPathInfo HOUR_HAND_POINTS = {
  2, (GPoint []) {
    { 0, 0},
    { 0, -27}
  }
};