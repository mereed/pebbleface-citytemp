#include "pebble.h"
	
static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_CLEAR_DAY,
  RESOURCE_ID_CLEAR_NIGHT,
  RESOURCE_ID_WINDY,
  RESOURCE_ID_COLD,
  RESOURCE_ID_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_HAZE,
  RESOURCE_ID_CLOUD,
  RESOURCE_ID_RAIN,
  RESOURCE_ID_SNOW,
  RESOURCE_ID_HAIL,
  RESOURCE_ID_CLOUDY,
  RESOURCE_ID_STORM,
  RESOURCE_ID_FOG,
  RESOURCE_ID_NA,
};

enum WeatherKey {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING 
  HIDE_TEMP = 0x2
};

static int hide_temp;

Window *window;
Layer *window_layer;

BitmapLayer *layer_batt_img;
BitmapLayer *layer_conn_img;

GBitmap *img_battery_100;
GBitmap *img_battery_90;
GBitmap *img_battery_80;
GBitmap *img_battery_70;
GBitmap *img_battery_60;
GBitmap *img_battery_50;
GBitmap *img_battery_40;
GBitmap *img_battery_30;
GBitmap *img_battery_20;
GBitmap *img_battery_10;
GBitmap *img_battery_charge;

GBitmap *img_bt_connect;
GBitmap *img_bt_disconnect;

int charge_percent = 0;

Layer* weather_holder;
Layer* weather_holder2;
Layer *date_holder;

BitmapLayer *icon_layer;
GBitmap *icon_bitmap = NULL;

TextLayer *temp_layer;
TextLayer *temp_layer2;
TextLayer *temp_layer3;
TextLayer *temp_layer4;

TextLayer *text_date_layer;
TextLayer *text_time_layer;

static AppSync sync;
static uint8_t sync_buffer[64];

static GFont          *custom_44;
static GFont          *custom_38;
static GFont          *custom_14;
static GFont          *custom_18;


void tempsize () {
	
	switch (hide_temp) {
  
	case 0:  //large
		 layer_set_hidden(text_layer_get_layer(temp_layer), false); 
		 layer_set_hidden(text_layer_get_layer(temp_layer2), false); 
		 layer_set_hidden(text_layer_get_layer(temp_layer3), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer4), true); 
	break;
		  
	case 1:  //small
		 layer_set_hidden(text_layer_get_layer(temp_layer), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer2), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer3), false); 
		 layer_set_hidden(text_layer_get_layer(temp_layer4), false); 
	break;

	case 2:  //none
		 layer_set_hidden(text_layer_get_layer(temp_layer), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer2), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer3), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer4), true); 
	break;
  }
} 		  
		  
static void sync_tuple_changed_callback(const uint32_t key,
                                        const Tuple* new_tuple,
                                        const Tuple* old_tuple,
                                        void* context) {

  // App Sync keeps new_tuple in sync_buffer, so we may use it directly
  switch (key) {
     case WEATHER_ICON_KEY:
     if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }
      icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[new_tuple->value->uint8]);
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
      break;    
	  
    case WEATHER_TEMPERATURE_KEY:
      text_layer_set_text(temp_layer, new_tuple->value->cstring);
      text_layer_set_text(temp_layer2, new_tuple->value->cstring);
      text_layer_set_text(temp_layer3, new_tuple->value->cstring);
      text_layer_set_text(temp_layer4, new_tuple->value->cstring);
      break;
	  
    case HIDE_TEMP:
	  hide_temp = new_tuple->value->uint8;
	  persist_write_bool(HIDE_TEMP, hide_temp);
	  tempsize();
	  break;
  }
}

void handle_battery(BatteryChargeState charge_state) {

    if (charge_state.is_charging) {
        bitmap_layer_set_bitmap(layer_batt_img, img_battery_charge);

    } else {
        if (charge_state.charge_percent <= 10) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_10);
        } else if (charge_state.charge_percent <= 20) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_20);
		} else if (charge_state.charge_percent <= 30) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_30);
		} else if (charge_state.charge_percent <= 40) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_40);
		} else if (charge_state.charge_percent <= 50) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_50);
        } else if (charge_state.charge_percent <= 60) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_60);
		} else if (charge_state.charge_percent <= 70) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_70);
		} else if (charge_state.charge_percent <= 80) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_80);
		} else if (charge_state.charge_percent <= 90) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_90);
		} else if (charge_state.charge_percent <= 99) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
		} else {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
        }      						
    }
    charge_percent = charge_state.charge_percent;
}

void handle_bluetooth(bool connected) {
    if (connected) {
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
    } else {
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_disconnect);
        vibes_long_pulse();
    }
}

void force_update(void) {
    handle_battery(battery_state_service_peek());
    handle_bluetooth(bluetooth_connection_service_peek());
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char date_text[] = "Xxxxxxxxx, 00 Xxx";
  static char time_text[] = "00:00";
  static int yday = -1;

  char *time_format;

  // Only update the date when it has changed.
  if (yday != tick_time->tm_yday) {

    strftime(date_text, sizeof(date_text), "%A, %e %b", tick_time);
    text_layer_set_text(text_date_layer, date_text);
  }

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Handle lack of non-padded hour format string for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    text_layer_set_text(text_time_layer, time_text + 1);
  } else {
    text_layer_set_text(text_time_layer, time_text);
  }
}

void handle_init(void) {
  // Setup messaging
  const int inbound_size = 64;
  const int outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
	
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);	
	
// resources
    img_bt_connect     = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CONNECT);
    img_bt_disconnect  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DISCONNECT);
	
    img_battery_100   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_100);
    img_battery_90   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_90);
    img_battery_80   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_80);
    img_battery_70   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_70);
    img_battery_60   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_60);
    img_battery_50   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_50);
    img_battery_40   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_40);
    img_battery_30   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_30);
    img_battery_20    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_20);
    img_battery_10    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_10);
    img_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_CHARGING);

  // Load fonts
  custom_44  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_44 ) );
  custom_38  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_38 ) );
  custom_14  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_14 ) );
  custom_18  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_18 ) );
 
  // active layers   
  layer_batt_img  = bitmap_layer_create(GRect(0, 0, 144, 5));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));

  layer_conn_img  = bitmap_layer_create(GRect(0, 111, 144, 57));
  bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_conn_img)); 

  // Setup weather and time layers
  Layer *weather_holder = layer_create(GRect(0, 0, 144, 168 ));
  Layer *weather_holder2 = layer_create(GRect(0, 0, 144, 168 ));
  layer_add_child(window_layer, weather_holder);
  layer_add_child(window_layer, weather_holder2);

  icon_layer = bitmap_layer_create(GRect(0, 17, 144, 137));
  layer_add_child(weather_holder, bitmap_layer_get_layer(icon_layer));	

  temp_layer2 = text_layer_create(GRect(8, 83, 144, 168));
  text_layer_set_text_color(temp_layer2, GColorWhite);
  text_layer_set_background_color(temp_layer2, GColorClear);
  text_layer_set_font(temp_layer2, custom_44);
  text_layer_set_text_alignment(temp_layer2, GTextAlignmentCenter);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer2));
	
  temp_layer = text_layer_create(GRect(6, 82, 144, 168));
  text_layer_set_text_color(temp_layer, GColorBlack);
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_font(temp_layer, custom_44);
  text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer));

  temp_layer3 = text_layer_create(GRect(101, 90, 35, 24));
  text_layer_set_text_color(temp_layer3, GColorWhite);
  text_layer_set_background_color(temp_layer3, GColorClear);
  text_layer_set_font(temp_layer3, custom_18);
  text_layer_set_text_alignment(temp_layer3, GTextAlignmentRight);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer3));
	
  temp_layer4 = text_layer_create(GRect(100, 89, 36, 24));
  text_layer_set_text_color(temp_layer4, GColorBlack);
  text_layer_set_background_color(temp_layer4, GColorClear);
  text_layer_set_font(temp_layer4, custom_18);
  text_layer_set_text_alignment(temp_layer4, GTextAlignmentRight);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer4));
	
  text_time_layer = text_layer_create(GRect(0, 17, 144, 49));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, custom_38);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
	
  text_date_layer = text_layer_create(GRect(0, 8, 144, 30));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, custom_14);
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));
	

  Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 14),
    TupletCString(WEATHER_TEMPERATURE_KEY, ""),
    TupletInteger(HIDE_TEMP, persist_read_bool(HIDE_TEMP)),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values,
                ARRAY_LENGTH(initial_values), sync_tuple_changed_callback,
                NULL, NULL);

  // Subscribe to notifications
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

  // handlers
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
	
  // draw first frame
  force_update();
}

void handle_deinit(void) {
	
  app_sync_deinit(&sync);

  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  tick_timer_service_unsubscribe();

  text_layer_destroy( text_time_layer );
  text_layer_destroy( text_date_layer );
  text_layer_destroy( temp_layer );
  text_layer_destroy( temp_layer2 );
  text_layer_destroy( temp_layer3 );
  text_layer_destroy( temp_layer4 );
	
  layer_remove_from_parent(bitmap_layer_get_layer(icon_layer));
  bitmap_layer_destroy(icon_layer);
  gbitmap_destroy(icon_bitmap);
  icon_bitmap = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(layer_conn_img));
  bitmap_layer_destroy(layer_conn_img);
  gbitmap_destroy(img_bt_connect);
  gbitmap_destroy(img_bt_disconnect);
  img_bt_connect = NULL;
  img_bt_disconnect = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(layer_batt_img));
  bitmap_layer_destroy(layer_batt_img);
	
  gbitmap_destroy(img_battery_100);
  gbitmap_destroy(img_battery_90);
  gbitmap_destroy(img_battery_80);
  gbitmap_destroy(img_battery_70);
  gbitmap_destroy(img_battery_60);
  gbitmap_destroy(img_battery_50);
  gbitmap_destroy(img_battery_40);
  gbitmap_destroy(img_battery_30);
  gbitmap_destroy(img_battery_20);
  gbitmap_destroy(img_battery_10);
  gbitmap_destroy(img_battery_charge);	

  fonts_unload_custom_font( custom_44 );
  fonts_unload_custom_font( custom_38 );
  fonts_unload_custom_font( custom_14 );
  fonts_unload_custom_font( custom_18 );

  layer_destroy( weather_holder );
  layer_destroy( weather_holder2 );
	
  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
