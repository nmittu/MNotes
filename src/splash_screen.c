#include <pebble.h>
#include "splash_screen.h"
#include "globals.c"

static Window *splash_window = NULL;
static TextLayer *text = NULL;
char *text_ = NULL;

void splash_remove(bool animate){
	window_stack_pop(animate);
}

void splash_show(bool animate){
	window_stack_push(splash_window, animate);
}

void splash_window_load(){
	Layer *mLayer = window_get_root_layer(splash_window);
	GRect bounds = layer_get_bounds(mLayer);
	
	text = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
	text_layer_set_background_color(text, GColorClear);
	text_layer_set_text_color(text, secondary);
	text_layer_set_text(text, text_);
	text_layer_set_font(text, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	text_layer_set_text_alignment(text, GTextAlignmentCenter);
	
	layer_add_child(mLayer, text_layer_get_layer(text));
}

void splash_window_unload(){
	text_layer_destroy(text);
	window_destroy(splash_window);
}

void splash_init(char *text__){
	text_ = text__;
	splash_window = window_create();
	
	window_set_background_color(splash_window, primary);
	
	window_set_window_handlers(splash_window, (WindowHandlers) {
    .load = splash_window_load,
    .unload = splash_window_unload
  });
	
}

void splash_destroy(){
}