#include <pebble.h>
#include "menu.h"
#include "globals.c"
#include "splash_screen.h"

#define ROWS 0
#define TITLE 1
#define TYPE 2
#define CONTENT 3


static Window *window;
static MenuLayer *menuL;
char **titles;
int rows;

void menu_show(bool animated){
	window_stack_push(window, animated);
}

uint16_t get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return rows;
}

void draw_rows(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data){
	menu_cell_basic_draw(ctx, cell_layer, titles[cell_index->row], NULL, NULL);
}

void selected(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	splash_init("Downloading");
	splash_show(true);
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	dict_write_cstring(iter, TITLE, titles[cell_index->row]);
	dict_write_int8(iter, TYPE, 0);
	
	dict_write_end(iter);
	app_message_outbox_send();
}

void long_selected(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	window_stack_pop(true);
	splash_init("Downloading");
	splash_show(true);
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	dict_write_int8(iter, TYPE, 1);
	
	dict_write_end(iter);
	app_message_outbox_send();
}

void menu_window_load(){
	Layer *mLayer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(mLayer);
	
	menuL = menu_layer_create(bounds);
	menu_layer_set_callbacks(menuL, NULL, (MenuLayerCallbacks){
		.get_num_rows = get_num_rows,
		.draw_row  = draw_rows,
		.select_click  = selected,
		.select_long_click = long_selected
	});
	menu_layer_set_normal_colors(menuL, primary, secondary);
	menu_layer_set_highlight_colors(menuL, secondary, primary);
	menu_layer_set_click_config_onto_window(menuL, window);
	
	layer_add_child(mLayer, menu_layer_get_layer(menuL));
}
void menu_window_unload(){
	menu_layer_destroy(menuL);
}

void menu_init(char **titles_, int rows_){
	titles = titles_;
	rows = rows_;
	
	
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload
  });
}

void menu_destroy(){
	window_destroy(window);
}