#include <pebble.h>
#include "main.h"

// Define window, layers, fonts, ints, etc
static Window *s_window;
static TextLayer *s_time_layer, *s_date_layer;
static Layer *s_canvas_layer, *s_battery_layer, *s_hands_layer;
static GFont s_time_font, s_date_font;
static int s_battery_level;

// Define constant paths for analog clock minute/hour hands
static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;

// A struct for our specific settings (see main.h)
ClaySettings settings;



// Initialize the default settings
static void default_settings() {
  // Default settings
  settings.LeftStripeColor = GColorDarkGray;
  settings.RightStripeColor = GColorDarkGray;
  settings.WatchBandColor = GColorWhite;
  settings.WatchFaceColor = GColorWhite;
  settings.TextColor = GColorBlack;
  settings.HandsColor = GColorBlack;
  settings.SecondHandColor = GColorLightGray;
  settings.BatteryColor = GColorLightGray;
  settings.BatteryBarToggle = true;
  settings.StrapDetails = true;
  settings.InvertOutline = false;
  settings.HourDots = true;
  settings.SelectClock = 'd';
}



// Read settings from persistent storage
static void load_settings() {
  // Load the default settings
  default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}



// Save the settings to persistent storage
static void save_settings() {
  // Make settings persist
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  update_display();
}



// Update the display elements one second after settings submission
static void update_display() {
  // copies a modified deinit(); to simulate a loop
  // Destroy clock hands 
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(s_tick_paths[i]);
  }
  tick_timer_service_unsubscribe();
  // Don't Destroy Window
  //window_destroy(s_window);
  
  // copies a modified init(); to simulate a loop
  // Create main Window element and assign to pointer
  s_window = window_create();
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_window, true);
  
  // Register with TickTimerService, per second updates for settings change only
  if (settings.SelectClock == 'a') {
    //if () use && instead of a nested if
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  } else {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  }
  
  // Draw initial hand paths for analog
  if (settings.SelectClock == 'a') {
    s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
    s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
    Layer *window_layer = window_get_root_layer(s_window);
    GRect bounds = layer_get_bounds(window_layer);
    GPoint center = grect_center_point(&bounds);
    gpath_move_to(s_minute_arrow, center);
    gpath_move_to(s_hour_arrow, center);
    for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
      s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
    }
  // Make sure the time is displayed from the start for digital
  } else if (settings.SelectClock == 'd') {
      update_time();
  } else {}
}



// Handle the response from AppMessage
static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Right Background Stripe Color
  Tuple *lg_color_t = dict_find(iter, MESSAGE_KEY_LeftStripeColor);
  if (lg_color_t) {
    settings.LeftStripeColor = GColorFromHEX(lg_color_t->value->int32);
  }
  
  // Left Background Stripe Color
  Tuple *rg_color_t = dict_find(iter, MESSAGE_KEY_RightStripeColor);
  if (rg_color_t) {
    settings.RightStripeColor = GColorFromHEX(rg_color_t->value->int32);
  }
  
  // Watchband Middle Stripe Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_WatchBandColor);
  if (bg_color_t) {
    settings.WatchBandColor = GColorFromHEX(bg_color_t->value->int32);
  }
  
  // Watchface Circle Color
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_WatchFaceColor);
  if (fg_color_t) {
    settings.WatchFaceColor = GColorFromHEX(fg_color_t->value->int32);
  }
  
  // Text Color
  Tuple *tg_color_t = dict_find(iter, MESSAGE_KEY_TextColor);
  if (tg_color_t) {
    settings.TextColor = GColorFromHEX(tg_color_t->value->int32);
  }
  
  // Hour/Minute Hands Color
  Tuple *hand_color_t = dict_find(iter, MESSAGE_KEY_HandsColor);
  if (hand_color_t) {
    settings.HandsColor = GColorFromHEX(hand_color_t->value->int32);
  }
  
  // Second Hand Color
  Tuple *sec_color_t = dict_find(iter, MESSAGE_KEY_SecondHandColor);
  if (sec_color_t) {
    settings.SecondHandColor = GColorFromHEX(sec_color_t->value->int32);
  }
  
  // Battery Color
  Tuple *yg_color_t = dict_find(iter, MESSAGE_KEY_BatteryColor);
  if (yg_color_t) {
    settings.BatteryColor = GColorFromHEX(yg_color_t->value->int32);
  }
  
  // Toggle Strap Holes
  Tuple *strap_tog_t = dict_find(iter, MESSAGE_KEY_StrapDetails);
  if (strap_tog_t) {
    settings.StrapDetails = strap_tog_t->value->int8 == 1;
  }
    
  // Toggle Hour Markings on analog watchface
  Tuple *hr_tog_t = dict_find(iter, MESSAGE_KEY_HourDots);
  if (hr_tog_t) {
    settings.HourDots = hr_tog_t->value->int8 == 1;
  }
  
  // (Toggle) Invert Outline Color Detail
  Tuple *out_tog_t = dict_find(iter, MESSAGE_KEY_InvertOutline);
  if (out_tog_t) {
    settings.InvertOutline = out_tog_t->value->int8 == 1;
  }
  
  // Toggle Battery Bar
  Tuple *battery_tog_t = dict_find(iter, MESSAGE_KEY_BatteryBarToggle);
  if (battery_tog_t) {
    settings.BatteryBarToggle = battery_tog_t->value->int8 == 1;
  }
  
  // Make Selection between analog and digital
  Tuple *select_t = dict_find(iter, MESSAGE_KEY_SelectClock);
  if (select_t) {
    settings.SelectClock = select_t->value->int8;
  }

  // Save the new settings to persistent storage
  save_settings();
}



// Get time updates and display them in 12h digital format
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%b %d", tick_time);
  // Show the date on the TextLayer
  text_layer_set_text(s_date_layer, date_buffer);
}



// Necessary for time updates
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Update time
  update_time();
}



// Record Battery Level
static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
}



// Draw battery meter
static void battery_update_proc(Layer *layer, GContext *ctx) {
  // Find bounds
  GRect bounds = layer_get_bounds(layer);
  // Find the arclength of the battery circle
  int arcangle = (int)(float)(((float)s_battery_level / 100.0F) * 360.0F);
  // Angles
  uint16_t angle_start = DEG_TO_TRIGANGLE(0);
  uint16_t angle_end = DEG_TO_TRIGANGLE(arcangle);
  
  // Draw the battery arc
  graphics_context_set_stroke_color(ctx, settings.BatteryColor);
  graphics_context_set_stroke_width(ctx, 5);
  graphics_draw_arc(ctx, bounds, GOvalScaleModeFitCircle, angle_start, angle_end);
}



// Draw background
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Custom background layer drawing happens here
  // Screen size is 144 x 168 pixels (x_max, -y_max) for Aplite, Basalt
  // Screen size is 180 x 180 pixels (x_max, -y_max) for Chalk (+36, +12)
  
  // Rounded rectangle corner radius
  uint8_t no_corner_radius = 0;
  // Set center for all watchtypes
  GPoint center = GPoint(PBL_IF_ROUND_ELSE(90, 72), PBL_IF_ROUND_ELSE(90, 84));
  
  // Fill the left rectangle
  // Set the fill color and rectangle bounds 
  graphics_context_set_fill_color(ctx, settings.LeftStripeColor);
  GRect left_rect_bounds = GRect(0, 0, PBL_IF_ROUND_ELSE(66, 48), PBL_IF_ROUND_ELSE(180, 168));
  graphics_fill_rect(ctx, left_rect_bounds, no_corner_radius, GCornersAll);
  
  // Fill the middle rectangle
  graphics_context_set_fill_color(ctx, settings.WatchBandColor);
  GRect middle_rect_bounds = GRect(PBL_IF_ROUND_ELSE(66, 48), 0, 48, PBL_IF_ROUND_ELSE(180, 168));
  graphics_fill_rect(ctx, middle_rect_bounds, no_corner_radius, GCornersAll);
  
  // Fill the right rectangle
  graphics_context_set_fill_color(ctx, settings.RightStripeColor);
  GRect right_rect_bounds = GRect(PBL_IF_ROUND_ELSE(114, 96), 0, PBL_IF_ROUND_ELSE(66, 48), PBL_IF_ROUND_ELSE(180, 168));
  graphics_fill_rect(ctx, right_rect_bounds, no_corner_radius, GCornersAll);
  
  // Draw two lines
  if (settings.StrapDetails) {
    if (settings.InvertOutline) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
    } else {
      graphics_context_set_stroke_color(ctx, GColorBlack);
    }
    graphics_context_set_stroke_width(ctx, 3);
    GPoint startleft = GPoint(PBL_IF_ROUND_ELSE(66, 48), 0);
    GPoint endleft = GPoint(PBL_IF_ROUND_ELSE(66, 48), PBL_IF_ROUND_ELSE(180, 168));
    graphics_draw_line(ctx, startleft, endleft);
    GPoint startright = GPoint(PBL_IF_ROUND_ELSE(114, 96), 0);
    GPoint endright = GPoint(PBL_IF_ROUND_ELSE(114, 96), PBL_IF_ROUND_ELSE(180, 168));
    graphics_draw_line(ctx, startright, endright);
  }
  
  // Draw a circle 
  if (settings.InvertOutline) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  } else {
    graphics_context_set_stroke_color(ctx, GColorBlack);
  }
  graphics_context_set_fill_color(ctx, settings.WatchFaceColor);
  // Set the stroke width (must be an odd integer value)
  graphics_context_set_stroke_width(ctx, 7);
  // Set radius
  uint8_t radius = 54;
  // Draw the outline of a circle
  graphics_draw_circle(ctx, center, radius);
  // Fill a circle
  graphics_fill_circle(ctx, center, radius);
  
  // Draw small dots around circle if option is selected
  if (settings.StrapDetails) {
    uint8_t small_radius = 2;
    graphics_context_set_stroke_width(ctx, 7);
    graphics_context_set_fill_color(ctx, settings.WatchBandColor);
    GPoint cen6 = GPoint(PBL_IF_ROUND_ELSE(90, 72), PBL_IF_ROUND_ELSE(164, 158));
    GPoint cen12 = GPoint(PBL_IF_ROUND_ELSE(90, 72), PBL_IF_ROUND_ELSE(16, 10));
    graphics_draw_circle(ctx, cen6, small_radius);
    graphics_fill_circle(ctx, cen6, small_radius);
    graphics_draw_circle(ctx, cen12, small_radius);
    graphics_fill_circle(ctx, cen12, small_radius);
  } else {}
  
  if (settings.SelectClock == 'a' && settings.HourDots) {
    uint8_t small_radius = 2;
    // Draw hour placements for analog clock
    GPoint center1 = GPoint(PBL_IF_ROUND_ELSE(122, 104), PBL_IF_ROUND_ELSE(34, 28));
    GPoint center2 = GPoint(PBL_IF_ROUND_ELSE(146, 128), PBL_IF_ROUND_ELSE(58, 52));
    GPoint center3 = GPoint(PBL_IF_ROUND_ELSE(154, 136), PBL_IF_ROUND_ELSE(90, 84));
    GPoint center4 = GPoint(PBL_IF_ROUND_ELSE(146, 128), PBL_IF_ROUND_ELSE(122, 116));
    GPoint center5 = GPoint(PBL_IF_ROUND_ELSE(122, 104), PBL_IF_ROUND_ELSE(146, 140));
    GPoint center6 = GPoint(PBL_IF_ROUND_ELSE(90, 72), PBL_IF_ROUND_ELSE(154, 148));
    GPoint center7 = GPoint(PBL_IF_ROUND_ELSE(58, 40), PBL_IF_ROUND_ELSE(146, 140));
    GPoint center8 = GPoint(PBL_IF_ROUND_ELSE(34, 16), PBL_IF_ROUND_ELSE(122, 116));
    GPoint center9 = GPoint(PBL_IF_ROUND_ELSE(26, 8), PBL_IF_ROUND_ELSE(90, 84));
    GPoint center10 = GPoint(PBL_IF_ROUND_ELSE(34, 16), PBL_IF_ROUND_ELSE(58, 52));
    GPoint center11 = GPoint(PBL_IF_ROUND_ELSE(58, 40), PBL_IF_ROUND_ELSE(34, 28));
    GPoint center12 = GPoint(PBL_IF_ROUND_ELSE(90, 72), PBL_IF_ROUND_ELSE(26, 20));

    if (settings.InvertOutline) {
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }
    graphics_fill_circle(ctx, center1, small_radius);
    graphics_fill_circle(ctx, center2, small_radius);
    graphics_fill_circle(ctx, center3, small_radius);
    graphics_fill_circle(ctx, center4, small_radius);
    graphics_fill_circle(ctx, center5, small_radius);
    graphics_fill_circle(ctx, center6, small_radius);
    graphics_fill_circle(ctx, center7, small_radius);
    graphics_fill_circle(ctx, center8, small_radius);
    graphics_fill_circle(ctx, center9, small_radius);
    graphics_fill_circle(ctx, center10, small_radius);
    graphics_fill_circle(ctx, center11, small_radius);
    graphics_fill_circle(ctx, center12, small_radius);
  } else{}
}



// Draw analog watchface hands layer
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  const uint8_t second_hand_length = 50;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  uint16_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60; 
  //int32_t previously
  
  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (uint16_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
    //int16_t and int32_t previously
    .y = (int16_t)(-cos_lookup(second_angle) * (uint16_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  };

  // minute/hour hand
  graphics_context_set_fill_color(ctx, settings.HandsColor);
  graphics_context_set_stroke_color(ctx, settings.HandsColor);
  graphics_context_set_stroke_width(ctx, 5);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);
  
  graphics_context_set_fill_color(ctx, settings.HandsColor);
  graphics_context_set_stroke_color(ctx, settings.HandsColor);
  
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);
  
  // second hand
  graphics_context_set_stroke_color(ctx, settings.SecondHandColor);
  graphics_context_set_stroke_width(ctx, 3);
  graphics_draw_line(ctx, second_hand, center);

  // dot in the middle
  graphics_context_set_fill_color(ctx, settings.SecondHandColor);
  graphics_fill_circle(ctx, center, 3);
}



// Update per second instead of per minute
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}



// Window Load event
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  // Add to Window
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  // Mark for redrawing at the earliest opportunity
  layer_mark_dirty(s_canvas_layer);
  
  // Create battery meter Layer if battery bar is toggled on
  if(settings.BatteryBarToggle) {
    s_battery_layer = layer_create(GRect(PBL_IF_ROUND_ELSE(42, 24), PBL_IF_ROUND_ELSE(42, 36), 96, 96));
    layer_set_update_proc(s_battery_layer, battery_update_proc);
    // Add to Window
    layer_add_child(window_get_root_layer(window), s_battery_layer);
    // Update meter
    layer_mark_dirty(s_battery_layer);
  } else {}
  
  // If Digital clockface is selected
  if (settings.SelectClock == 'd') {    
    // Create the TextLayer with specific bounds
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    // Improve the layout to be more like a watchface
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_text_color(s_time_layer, settings.TextColor);
    text_layer_set_background_color(s_time_layer, GColorClear);
    // Create GFont for time and apply GFont to TextLayer
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOTO_SANS_BOLD_36));
    text_layer_set_font(s_time_layer, s_time_font);
    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    
    // Create date TextLayer
    s_date_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(96, 90), bounds.size.w, 50));
    // Improve layout
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    text_layer_set_text_color(s_date_layer, settings.TextColor);
    text_layer_set_background_color(s_date_layer, GColorClear);
    // Create GFont for date and apply GFont to TextLayer
    s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOTO_SANS_REGULAR_18));
    text_layer_set_font(s_date_layer, s_date_font);
    // Add to Window
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  // If Analog clockface is selected
  } else if (settings.SelectClock == 'a') {
    s_hands_layer = layer_create(bounds);
    layer_set_update_proc(s_hands_layer, hands_update_proc);
    layer_add_child(window_layer, s_hands_layer);
  } else{}
}



// Window Unload event
static void window_unload(Window *window) {
  // Unload GFonts
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  // Destroy Layers
  layer_destroy(s_hands_layer);
  layer_destroy(s_battery_layer);
  layer_destroy(s_canvas_layer);
}



// Initialize
static void init(void) {
  // Load settings
  load_settings();
  
  // Listen for AppMessages
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);

  // Create main Window element and assign to pointer
  s_window = window_create();
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_window, true);
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  
  // Register with TickTimerService, minute_unit for digital/analog, second_unit for analog only
  if (settings.SelectClock == 'a') {
    // Analog will always show seconds
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
  
  // Draw initial hand paths 
  if (settings.SelectClock == 'a') {
    s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
    s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

    Layer *window_layer = window_get_root_layer(s_window);
    GRect bounds = layer_get_bounds(window_layer);
    GPoint center = grect_center_point(&bounds);
    gpath_move_to(s_minute_arrow, center);
    gpath_move_to(s_hour_arrow, center);

    for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
      s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
    }
  // Make sure the time is displayed from the start
  } else if (settings.SelectClock == 'd') {
    update_time();
  } else{}
}



// Shut down
static void deinit(void) {
  // Destroy clock hands 
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);
  
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(s_tick_paths[i]);
  }
  
  tick_timer_service_unsubscribe();
  
  // Destroy Window
  window_destroy(s_window);
}



// Main loop
int main(void) {
  init();
  app_event_loop();
  deinit();
}
