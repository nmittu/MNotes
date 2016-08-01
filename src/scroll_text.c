#include <pebble.h>
#include "scroll_text.h"
#include "globals.c"
#include "splash_screen.h"

#define paddingX 4
#define paddingY 4
#define maxH 2000

#define TITLE 1
#define TYPE 2

Window *window;
ScrollLayer *scrollL;
TextLayer *textL;
char *text;
char *title;

void scroll_text_reload(){
	window_stack_pop(false);
	scroll_text_init(text, title);
	scroll_text_show(false);
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	splash_init("Caching");
	splash_show(true);
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	dict_write_int8(iter, TYPE, 2);
	dict_write_cstring(iter, TITLE, title);
	
	dict_write_end(iter);
	app_message_outbox_send();
}

void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
}

void config_provider(Window *window) {
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

void scroll_text_show(bool animate){
	window_stack_push(window, animate);
}

void scroll_text_init(char* text_, char* title_){
	title = title_;
	
	window = window_create();
	Layer *rootLayer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(rootLayer);
	
	window_set_background_color(window, primary);
	
	scrollL = scroll_layer_create(bounds);
	textL = text_layer_create(GRect(paddingX, paddingY, bounds.size.w - (paddingX*2), maxH));
	text_layer_set_text(textL, text_);
	text_layer_set_font(textL, fonts_get_system_font(font));
	text_layer_set_background_color(textL, GColorClear);
	text_layer_set_text_color(textL, secondary);
	scroll_layer_add_child(scrollL, text_layer_get_layer(textL));
	
	GSize max_size = text_layer_get_content_size(textL);
	
	scroll_layer_set_callbacks(scrollL, (ScrollLayerCallbacks){
		.click_config_provider = (ClickConfigProvider)config_provider
	});
	scroll_layer_set_content_size(scrollL, GSize(bounds.size.w, max_size.h + (paddingY * 3)));
	scroll_layer_set_shadow_hidden(scrollL, false);
	scroll_layer_set_click_config_onto_window(scrollL, window);
	
	layer_add_child(rootLayer, scroll_layer_get_layer(scrollL));
		
	scroll_layer_set_shadow_hidden(scrollL, !shadows);
	
	#if defined(PBL_ROUND)
		text_layer_enable_screen_text_flow_and_paging(textL, 2);
		scroll_layer_set_paging(scrollL, true);
	#endif
}

void scroll_text_destroy(){
	window_destroy(window);
	scroll_layer_destroy(scrollL);
	text_layer_destroy(textL);
}
