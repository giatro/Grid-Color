#include <pebble.h>
#define KEY_BG_COLOR_RED 0
#define KEY_BG_COLOR_GREEN 1 
#define KEY_BG_COLOR_BLUE 2
#define KEY_ST_COLOR_RED 3
#define KEY_ST_COLOR_GREEN 4 
#define KEY_ST_COLOR_BLUE 5
#define KEY_ND_COLOR_RED 6
#define KEY_ND_COLOR_GREEN 7 
#define KEY_ND_COLOR_BLUE 8
#define KEY_RANDOM 9

Window *window;
TextLayer* time_layer[16];
char time_text[] = "1234";
char month_text[] = "DECEMBER    ";
char weekday_text[] = "THURDSDAY    ";
char day_text[] = "23RD";
char grid[16][2];
bool change = true;
GColor foregroundcolor;
GColor foregroundcolor2;
GColor backgroundcolor;

static void set_color() {
  backgroundcolor = GColorBlack;
  foregroundcolor = GColorWhite;  
  foregroundcolor2 = GColorWhite;
  
  int random = 0;
  #ifdef PBL_COLOR
  if(persist_exists(KEY_ST_COLOR_RED)) {
    int bgred   = persist_read_int(KEY_BG_COLOR_RED);
    int bggreen = persist_read_int(KEY_BG_COLOR_GREEN);
    int bgblue  = persist_read_int(KEY_BG_COLOR_BLUE);
    int stred   = persist_read_int(KEY_ST_COLOR_RED);
    int stgreen = persist_read_int(KEY_ST_COLOR_GREEN);
    int stblue  = persist_read_int(KEY_ST_COLOR_BLUE);
    int ndred   = persist_read_int(KEY_ND_COLOR_RED);
    int ndgreen = persist_read_int(KEY_ND_COLOR_GREEN);
    int ndblue  = persist_read_int(KEY_ND_COLOR_BLUE);
    random  = persist_read_int(KEY_RANDOM);
    backgroundcolor  = GColorFromRGB(bgred, bggreen, bgblue);
    foregroundcolor  = GColorFromRGB(stred, stgreen, stblue);
    foregroundcolor2 = GColorFromRGB(ndred, ndgreen, ndblue);
  }
  #endif
  window_set_background_color(window, backgroundcolor);
  for(int i = 0; i<4; i++){
    change = !change;
    for(int j = 0; j<4; j++){
      if (!(j % 2)) {
        change = !change;
      }
      change = random ? rand() % 2 : change;
      if (change) {
        text_layer_set_text_color(time_layer[i*4+j], foregroundcolor);
      } else {
        text_layer_set_text_color(time_layer[i*4+j], foregroundcolor2);
      }
    }
  }
}

void draw_time(){
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  
  if(clock_is_24h_style()==true){
    strftime(time_text, sizeof(time_text), "%H%M", tick_time);
  }else{
    strftime(time_text, sizeof(time_text), "%I%M", tick_time);
  }
  
  for(int i=0; i<4; i++){
    char s[] = " ";
    s[0] = time_text[i];
    strcpy(grid[i], s);
    text_layer_set_text(time_layer[i], grid[i]);
  }
}

void draw_date(){
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  
  strftime(weekday_text, sizeof(weekday_text), "%A   ", tick_time);
  for(int i=0; i<(int)(strlen(weekday_text)); i++){
  if(weekday_text[i]>96)weekday_text[i]-=32;
  }
    
  for(int i=0; i<4; i++){
    char s[] = " ";
    s[0] = weekday_text[i];
    strcpy(grid[4+i], s);
    text_layer_set_text(time_layer[4+i], grid[4+i]);
  }
    
  strftime(day_text, sizeof(day_text), "%dTH", tick_time);
  if(day_text[1]=='1'&&day_text[0]!='1'){
    day_text[2]='S';
    day_text[3]='T';
  }else if(day_text[1]=='2'&&day_text[0]!='1'){
    day_text[2]='N';
    day_text[3]='D';
  }else if(day_text[1]=='3'&&day_text[0]!='1'){
    day_text[2]='R';
    day_text[3]='D';
  }else{
    day_text[2]='T';
    day_text[3]='H';
  }
    
  for(int i=0; i<4; i++){
    char s[] = " ";
    s[0] = day_text[i];
    strcpy(grid[8+i], s);
    text_layer_set_text(time_layer[8+i], grid[8+i]);
  }
  
  strftime(month_text, sizeof(month_text), "%B   ", tick_time);
  for(int i=0; i<(int)(strlen(month_text)); i++){
  if(month_text[i]>96)month_text[i]-=32;
  }
    
  for(int i=0; i<4; i++){
    char s[] = " ";
    s[0] = month_text[i];
    strcpy(grid[12+i], s);
    text_layer_set_text(time_layer[12+i], grid[12+i]);
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  set_color();
  draw_time();
  
  if(tick_time->tm_min==0&&tick_time->tm_hour==0){
    draw_date();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message recived!");
  // Color
  Tuple *color_bg_red_t = dict_find(iterator, KEY_BG_COLOR_RED);
  Tuple *color_bg_green_t = dict_find(iterator, KEY_BG_COLOR_GREEN);
  Tuple *color_bg_blue_t = dict_find(iterator, KEY_BG_COLOR_BLUE);
  Tuple *color_st_red_t = dict_find(iterator, KEY_ST_COLOR_RED);
  Tuple *color_st_green_t = dict_find(iterator, KEY_ST_COLOR_GREEN);
  Tuple *color_st_blue_t = dict_find(iterator, KEY_ST_COLOR_BLUE);
  Tuple *color_nd_red_t = dict_find(iterator, KEY_ND_COLOR_RED);
  Tuple *color_nd_green_t = dict_find(iterator, KEY_ND_COLOR_GREEN);
  Tuple *color_nd_blue_t = dict_find(iterator, KEY_ND_COLOR_BLUE);
  Tuple *random_t = dict_find(iterator, KEY_RANDOM);
  if(color_st_red_t) {
    #ifdef PBL_COLOR
      int bgred   = color_bg_red_t->value->int32;
      int bggreen = color_bg_green_t->value->int32;
      int bgblue  = color_bg_blue_t->value->int32;
      int stred   = color_st_red_t->value->int32;
      int stgreen = color_st_green_t->value->int32;
      int stblue  = color_st_blue_t->value->int32;
      int ndred   = color_nd_red_t->value->int32;
      int ndgreen = color_nd_green_t->value->int32;
      int ndblue  = color_nd_blue_t->value->int32;
      int random  = random_t->value->int32;;
      backgroundcolor = GColorFromRGB(bgred, bggreen, bgblue);
      foregroundcolor = GColorFromRGB(stred, stgreen, stblue);
      foregroundcolor2 = GColorFromRGB(ndred, ndgreen, ndblue);
    
      // Persist values
      persist_write_int(KEY_BG_COLOR_RED, bgred);
      persist_write_int(KEY_BG_COLOR_GREEN, bggreen);
      persist_write_int(KEY_BG_COLOR_BLUE, bgblue);
      persist_write_int(KEY_ST_COLOR_RED, stred);
      persist_write_int(KEY_ST_COLOR_GREEN, stgreen);
      persist_write_int(KEY_ST_COLOR_BLUE, stblue);
      persist_write_int(KEY_ND_COLOR_RED, ndred);
      persist_write_int(KEY_ND_COLOR_GREEN, ndgreen);
      persist_write_int(KEY_ND_COLOR_BLUE, ndblue);
      persist_write_int(KEY_RANDOM, random);
    #endif
  }
  set_color();
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  // Color config
  backgroundcolor  = GColorBlack;
  foregroundcolor  = GColorWhite;  
  foregroundcolor2 = GColorWhite;

  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, backgroundcolor);
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  GFont time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCIFLY_47));
  for(int i = 0; i<4; i++){
    for(int j = 0; j<4; j++){
      time_layer[i*4+j] = text_layer_create(GRect(j*36-4, i*42-8, 44, 50));
      text_layer_set_text_color(time_layer[i*4+j], foregroundcolor);
      text_layer_set_font(time_layer[i*4+j], time_font);
      text_layer_set_text_alignment(time_layer[i*4+j], GTextAlignmentCenter);
      text_layer_set_background_color(time_layer[i*4+j], GColorClear);
      layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer[i*4+j]));
    }
  }
  for(int i=0; i<16; i++){
    strcpy(grid[i], " ");
  }
  
  draw_time();
  draw_date();
  set_color();
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit() {
  for(int i = 0; i<16; i++){
    text_layer_destroy(time_layer[i]);
  }
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}   