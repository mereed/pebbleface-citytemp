/*

Copyright (C) 2016 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-------------------------------------------------------------------

*/


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
  HIDE_TEMP = 0x2,
  CITY = 0x3
};

static int hide_temp;
static int cityimage;

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

static GFont          custom_40;
static GFont          custom_37;
static GFont          custom_14;
static GFont          custom_18;


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

void skyline_image() {		
		
	if (img_bt_connect) {
		gbitmap_destroy(img_bt_connect);
		img_bt_connect = NULL;
    }
	
	switch (cityimage) {

		case 0:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_DEFAULT);
			break;
		case 1:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_DEFAULT);
			break;
		case 2:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_ATHENS);
			break;
		case 3:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_AUCKLAND);
			break;
		case 4:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_BEIJING);
			break;
		case 5:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_BERLIN);
			break;
		case 6:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_BRISBANE);
			break;
		case 7:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_CHICAGO);
			break;
		case 8:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_DELHI);
			break;
		case 9:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_DUBAI);
			break;
		case 10:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_DUBLIN);
			break;
		case 11:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_HONGKONG);
			break;
		case 12:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_LONDON);
			break;
		case 13:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_LOSANGELES);
			break;
		case 14:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_MADRID);
			break;
		case 15:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_MELBOURNE);
			break;
		case 16:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_MOSCOW);
			break;
		case 17:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_NEWYORK);
			break;
		case 18:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_PARIS);
			break;
		case 19:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_RIO);
			break;
		case 20:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_ROME);
			break;
		case 21:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_SANFRANCISCO);
			break;
		case 22:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_SEOUL);
			break;
		case 23:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_SHANGHAI);
			break;
		case 24:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_SINGAPORE);
			break;
		case 25:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_SYDNEY);
			break;
		case 26:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_TOKYO);
			break;
		case 27:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_TORONTO);
			break;
		case 28:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_VANCOUVER);
			break;
		case 29:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_WASHINGTONDC);
			break;
		case 30:
			img_bt_connect = gbitmap_create_with_resource(RESOURCE_ID_WELLINGTON);
			break;

	
/*


ottawa
istanbul
miami
montreal
mumbai
cairo
canberra

munich
vienna
cape town
florence
sofia
prague
oslo
venice

johannesburg
helsinki
amsterdam
glasgow
brussels
venice
barcelona
jakarta
taipei

*/
    }
		
	   if (img_bt_connect != NULL) {
		bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
		layer_set_hidden(bitmap_layer_get_layer(layer_conn_img), false);
		layer_mark_dirty(bitmap_layer_get_layer(layer_conn_img));
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
	  
    case CITY:
	  cityimage = new_tuple->value->uint8;
	  persist_write_bool(CITY, cityimage);
	  skyline_image();
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
    if (!connected) {
		
		 layer_set_hidden(text_layer_get_layer(temp_layer), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer2), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer3), true); 
		 layer_set_hidden(text_layer_get_layer(temp_layer4), true); 

		layer_set_hidden(bitmap_layer_get_layer(icon_layer), true); 

        bitmap_layer_set_bitmap(layer_conn_img, img_bt_disconnect);
		vibes_long_pulse();

    } else {
		
		layer_set_hidden(bitmap_layer_get_layer(icon_layer), false); 
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
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
    img_bt_connect     = gbitmap_create_with_resource(RESOURCE_ID_DEFAULT);
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
  custom_40  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_40 ) );
  custom_37  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_37 ) );
  custom_14  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_14 ) );
  custom_18  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_18 ) );
 
  // active layers   


#ifdef PBL_PLATFORM_CHALK
  layer_batt_img  = bitmap_layer_create(GRect(50, 4, 79, 17));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));
#else
  layer_batt_img  = bitmap_layer_create(GRect(0, 0, 144, 5));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));	
#endif
	
	
#ifdef PBL_PLATFORM_CHALK
  layer_conn_img  = bitmap_layer_create(GRect(0, 118, 180, 57));
#else
  layer_conn_img  = bitmap_layer_create(GRect(0, 111, 144, 57));
#endif
  bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_conn_img)); 

  // Setup weather and time layers
  Layer *weather_holder = layer_create(GRect(0, 0, 180, 180 ));
  Layer *weather_holder2 = layer_create(GRect(0, 0, 180, 180 ));
  layer_add_child(window_layer, weather_holder);
  layer_add_child(window_layer, weather_holder2);

#ifdef PBL_PLATFORM_CHALK
	icon_layer = bitmap_layer_create(GRect(0, 25, 180, 137));
#else
	icon_layer = bitmap_layer_create(GRect(0, 17, 144, 137));
#endif
  layer_add_child(weather_holder, bitmap_layer_get_layer(icon_layer));	

#ifdef PBL_PLATFORM_CHALK
  temp_layer2 = text_layer_create(GRect(12, 91, 180, 180));
#else
  temp_layer2 = text_layer_create(GRect(12, 83, 144, 168));
#endif
  text_layer_set_text_color(temp_layer2, GColorWhite);
  text_layer_set_background_color(temp_layer2, GColorClear);
  text_layer_set_font(temp_layer2, custom_40);
  text_layer_set_text_alignment(temp_layer2, GTextAlignmentCenter);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer2));
	
#ifdef PBL_PLATFORM_CHALK
	temp_layer = text_layer_create(GRect(10, 90, 180, 180));
#else
	temp_layer = text_layer_create(GRect(10, 82, 144, 168));
#endif
  text_layer_set_text_color(temp_layer, GColorBlack);
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_font(temp_layer, custom_40);
  text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer));

#ifdef PBL_PLATFORM_CHALK
	temp_layer3 = text_layer_create(GRect(121, 105, 35, 2));
#else
	temp_layer3 = text_layer_create(GRect(101, 90, 35, 2));
#endif
  text_layer_set_text_color(temp_layer3, GColorWhite);
  text_layer_set_background_color(temp_layer3, GColorClear);
  text_layer_set_font(temp_layer3, custom_18);
  text_layer_set_text_alignment(temp_layer3, GTextAlignmentRight);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer3));
	
#ifdef PBL_PLATFORM_CHALK
	temp_layer4 = text_layer_create(GRect(120, 104, 36, 24));
#else
	temp_layer4 = text_layer_create(GRect(100, 89, 36, 24));
#endif
  text_layer_set_text_color(temp_layer4, GColorBlack);
  text_layer_set_background_color(temp_layer4, GColorClear);
  text_layer_set_font(temp_layer4, custom_18);
  text_layer_set_text_alignment(temp_layer4, GTextAlignmentRight);
  layer_add_child(weather_holder2, text_layer_get_layer(temp_layer4));	
	
#ifdef PBL_PLATFORM_CHALK
	text_time_layer = text_layer_create(GRect(0, 11, 180, 49));
#else
	text_time_layer = text_layer_create(GRect(0, 17, 144, 49));
#endif
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, custom_37);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
	
#ifdef PBL_PLATFORM_CHALK
	text_date_layer = text_layer_create(GRect(0, 49, 180, 30));
#else
	text_date_layer = text_layer_create(GRect(0, 8, 144, 30));
#endif
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, custom_14);
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));
	

  Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 14),
    TupletCString(WEATHER_TEMPERATURE_KEY, ""),
    TupletInteger(HIDE_TEMP, persist_read_bool(HIDE_TEMP)),
    TupletInteger(CITY, persist_read_bool(CITY))
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

  fonts_unload_custom_font( custom_40 );
  fonts_unload_custom_font( custom_37 );
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
