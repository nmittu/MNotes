#include <pebble.h>
#include "globals.c"
#include "menu.h"
#include "splash_screen.h"
#include "scroll_text.h"

#define ROWS 0
#define TITLE 1
#define TYPE 2
#define CONTENT 3
#define P_RED 4
#define P_GREEN 5
#define P_BLUE 6
#define S_RED 7
#define S_GREEN 8
#define S_BLUE 9
#define SHADOWS 10
#define FONT 11

char **titles = NULL;
int rows = -1;

void init();
void deinit();

static void inbox_received_callback(DictionaryIterator *iterator, void *context){
	if(dict_find(iterator, TYPE)->value->int8 == 0){
		Tuple *rows_ = dict_find(iterator, ROWS);
		rows = (int) rows_->value->uint32;

		titles = (char**)malloc(rows * sizeof(char*));
		APP_LOG(APP_LOG_LEVEL_DEBUG, "test");

		Tuple *title;
		for(int i = 0; i < rows; i++){
			title = dict_find(iterator, 100+i);
			//free(titles[i]);
			titles[i] = (char *)malloc(sizeof(char)*25);
			strcpy(titles[i], title->value->cstring);
		}

		splash_remove(true);

		menu_init(titles, rows);
		menu_show(true);
	}else if(dict_find(iterator, TYPE)->value->int8 ==1){
		Tuple *content = dict_find(iterator, CONTENT);
		Tuple *title = dict_find(iterator, TITLE);
		scroll_text_init(content->value->cstring, title->value->cstring);
		splash_remove(true);
		scroll_text_show(true);
	}else if(dict_find(iterator, TYPE)->value->int8 ==2){
		splash_remove(true);
		splash_destroy();
		splash_init("Set Settings");
		splash_show(true);
	}else if(dict_find(iterator, TYPE)->value->int8 ==3){
		splash_remove(true);
		window_stack_pop(true);
	}else if(dict_find(iterator, TYPE)->value->int8 ==4){
		primary = GColorFromRGB(dict_find(iterator, P_RED)->value->uint8,
														 dict_find(iterator, P_GREEN)->value->uint8,
														 dict_find(iterator, P_BLUE)->value->uint8);
		secondary = GColorFromRGB(dict_find(iterator, S_RED)->value->uint8,
															 dict_find(iterator, S_GREEN)->value->uint8,
															 dict_find(iterator, S_BLUE)->value->uint8);
		
		persist_write_data(PRIMARY, &primary, sizeof(GColor));
		persist_write_data(SECONDARY, &secondary, sizeof(GColor));
		shadows = dict_find(iterator, SHADOWS)->value->int8;
		
		font = (char*) malloc(sizeof(char)*28);
		strcpy(font, dict_find(iterator, FONT)->value->cstring);
		
		persist_write_string(FONT, font);
		
		window_stack_pop_all(true);
		deinit();
		init();
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send success!!");
}

void init(){
	
	primary = GColorWhite;
	secondary = GColorBlack;
	font = FONT_KEY_GOTHIC_24;
	
	if(persist_exists(PRIMARY) && persist_exists(SECONDARY) && persist_exists(FONT)){
		persist_read_data(PRIMARY, &primary, sizeof(GColor));
		persist_read_data(SECONDARY, &secondary, sizeof(GColor));
		font = (char*) malloc(sizeof(char)*28);
		persist_read_string(FONT, font, sizeof(char)*28);
	}
		
	splash_init("Downloading");
	splash_show(true);
	
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit(){
	if(titles != NULL && rows != -1){
		for(int i = 0; i<rows;i++){
			free(titles[i]);
		}
	}
	free(titles);
	menu_destroy();
	splash_destroy();
}

int main(){
	init();
	app_event_loop();
	deinit();
}