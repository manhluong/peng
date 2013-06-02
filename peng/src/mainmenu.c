/**
 * Copyright (©) 2013, Manh Luong Bui
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.\n\n2) Redistributions in binary form must
 * reproduce the above copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the distribution.
 *
 * 3) Neither the name of the Author nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "variables.h"
#include "functions.h"

uint8_t mainmenuStatus;
uint8_t volume;
bool turnScreen;
uint8_t selectedMainMenuRow;

/**
 * Not used. See overrideConfigProvider() & overrideSingleSelect().
 */
void mainMenuSelectClick(struct MenuLayer *ml,
						   struct MenuIndex *menuIndex,
						   void *clbCtx)
	{ }

/**
 * Not used. See overrideConfigProvider() & overrideSingleSelect().
 */
void mainMenuSelectLongClick(struct MenuLayer *ml,
								struct MenuIndex *menuIndex,
								void *clbCtx)
	{ }

//TODO make a flag to draw only if "dirty" state.
//ANYWAY seems that without menu_layer_reload_data(&mainMenu); it won't redraw.
void mainMenuDrawRow(GContext *ctx,
					   const Layer *cellLayer,
					   MenuIndex *menuIndex,
					   void *clbCtx)
	{
	graphics_context_set_text_color(ctx, GColorBlack);
	switch(menuIndex->row)
		{
		case PENG_INDEX:
			if(mainmenuStatus!=PENGING_STATUS)
				graphics_text_draw(ctx,
									PENG_ACTION,
									fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
									GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
									GTextOverflowModeWordWrap,
									GTextAlignmentLeft,
									NULL);
			else if(mainmenuStatus==PENGING_STATUS)
				graphics_text_draw(ctx,
									PENG_STOP,
									fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
									GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
									GTextOverflowModeWordWrap,
									GTextAlignmentLeft,
									NULL);
		break;
		case VOLUME_INDEX:
			if(mainmenuStatus!=SELECT_VOLUME_STATUS)
				{
				graphics_text_draw(ctx,
									VOLUME_TEXT,
									fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
									GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
									GTextOverflowModeTrailingEllipsis,
									GTextAlignmentLeft,
									NULL);
				}
			else if(mainmenuStatus==SELECT_VOLUME_STATUS)
				{
				graphics_text_draw(ctx,
									VOLUME_TEXT_SELECTED,
									fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
									GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
									GTextOverflowModeTrailingEllipsis,
									GTextAlignmentLeft,
									NULL);
				}
			//Two outline rectangles to make the bound thick.
			graphics_context_set_stroke_color(ctx, GColorBlack);
			graphics_draw_round_rect(ctx,
									GRect(6,
											20,
											cellLayer->frame.size.w-13,
											((cellLayer->frame.size.h)-21)),
											3);
			//Use this rectangle as inner bounds.
			graphics_draw_round_rect(ctx,
										GRect(7,
												21,
												cellLayer->frame.size.w-15,
												((cellLayer->frame.size.h)-23)),
												3);
			//Inner rectangle for actual volume level.
			graphics_context_set_fill_color(ctx, GColorBlack);
			//8 bits are enough.
			uint8_t barWidth = 0;
			if(volume==VOLUME_MAX)
				barWidth = cellLayer->frame.size.w-16;
			else if(volume==VOLUME_MIN)
				barWidth = 0;
			else if(volume > VOLUME_MIN && volume < VOLUME_MAX)
				{
				uint8_t totWidth = cellLayer->frame.size.w-16;
				barWidth = ((totWidth * volume) / 100);
				}
			graphics_fill_rect(ctx,
								GRect(8,
										22,
										barWidth,
										((cellLayer->frame.size.h)-24)),
								3,
								GCornersAll);
			break;
		case SCREEN_INDEX:
			//Label
			graphics_text_draw(ctx,
								SCREEN_TEXT,
								fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
								GRect(5 + cellLayer->frame.size.h-18,
										10,
										cellLayer->frame.size.w-5,
										cellLayer->frame.size.h-20),
								GTextOverflowModeWordWrap,
								GTextAlignmentLeft,
								NULL);
			//Double outline.
			graphics_draw_round_rect(ctx,
										GRect(5,
												10,
												cellLayer->frame.size.h-20,
												cellLayer->frame.size.h-20),
												3);
			graphics_draw_round_rect(ctx,
										GRect(6,
												11,
												cellLayer->frame.size.h-22,
												cellLayer->frame.size.h-22),
												3);
			if(turnScreen)
				{
				graphics_fill_rect(ctx,
									GRect(9,
											14,
											cellLayer->frame.size.h-28,
											cellLayer->frame.size.h-28),
									3,
									GCornersAll);
				}
			break;
		case HELP_INDEX:
			graphics_text_draw(ctx,
								HELP_TEXT,
								fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
								GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
								GTextOverflowModeTrailingEllipsis,
								GTextAlignmentLeft,
								NULL);
		break;
		case ABOUT_INDEX:
			graphics_text_draw(ctx,
								ABOUT_TEXT,
								fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
								GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
								GTextOverflowModeTrailingEllipsis,
								GTextAlignmentLeft,
								NULL);
		break;
		}
	}

int16_t mainMenuGetHeaderHeight(struct MenuLayer *menuLayer,
									uint16_t sectIndex,
									void *context)
	{
	return 0;
	}

int16_t mainMenuGetCellHeight(struct MenuLayer *menuLayer,
								MenuIndex *cellIndex,
								void *context)
	{
	return 44;
	}

uint16_t mainMenuGetNumRows (struct MenuLayer *ml,
								uint16_t sectionIndex,
								void *clbCtx)
	{
	return MENU_ROWS_NUM;
	}

uint16_t mainMenuGetNumSections(struct MenuLayer *menuLayer,
								void *context)
	{
	return 1;
	}

void addMainMenu()
	{
	menu_layer_init(&mainMenu,
					GRect(0, 0,
						  mainWnd.layer.frame.size.w,
						  mainWnd.layer.frame.size.h-15)); //-15 for the title bar.

	mainmenuStatus = SELECT_STATUS;
	selectedMainMenuRow = PENG_INDEX;
	turnScreen = true;
	volume = VOLUME_DEFAULT;

	ScrollLayerCallbacks overrideClbs =
		{
		.click_config_provider = &overrideConfigProvider,
		.content_offset_changed_handler = NULL //Not interested.
		};
	//Call this to effectively set the overrideConfigProvider.
	//This overrideConfigProvider will be called in scroll_layer_set_click_config_onto_window().
	scroll_layer_set_callbacks((ScrollLayer *)&mainMenu, overrideClbs);

	//The ClickConfigProvider of the Window is assigned to one internal of
	//MenuLayer, using scroll_layer_set_click_config_onto_window.
	menu_layer_set_click_config_onto_window(&mainMenu, &mainWnd);
	//Reset the MenuLayerCallbacks: it has handlers for click & long click.
	MenuLayerCallbacks mainMenuClbs =
		{
		.get_num_sections = mainMenuGetNumSections,
		.get_num_rows = &mainMenuGetNumRows,
		.get_cell_height = mainMenuGetCellHeight,
		.get_header_height = mainMenuGetHeaderHeight,
		.select_click = &mainMenuSelectClick,
		.select_long_click = &mainMenuSelectLongClick,
		.draw_row = &mainMenuDrawRow
		};
	menu_layer_set_callbacks(&mainMenu, NULL, mainMenuClbs);

	MenuIndex index =
		{
		.row = 	selectedMainMenuRow,
		.section = 1
		};
	menu_layer_set_selected_index (&mainMenu,
									index,
									MenuRowAlignCenter,
									true);

	layer_add_child(&mainWnd.layer, menu_layer_get_layer(&mainMenu));
	}

void overrideSingleSelect(ClickRecognizerRef recognizer, Window *window)
	{
	switch(selectedMainMenuRow)
		{
		case PENG_INDEX:
			if(mainmenuStatus==SELECT_STATUS)
				{
				uint8_t scr = CMD_SCREEN_OFF;
				if(turnScreen)
					scr = CMD_SCREEN_ON;
				//Send command.
				sendPengStartCmd(scr, volume);
				//Notify user.
				vibes_short_pulse();
				//MainMenu status update.
				mainmenuStatus = PENGING_STATUS;
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			else if(mainmenuStatus==PENGING_STATUS)
				{
				sendCmd(CMD_PENG, CMD_PENG_STOP);
				//Status switch is in onReceivedMsg().
				//This way the user can resend a Stop if it fails.
				//mainmenuStatus = SELECT_STATUS;
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			break;
		case VOLUME_INDEX:
			//Switch status.
			if(mainmenuStatus==SELECT_STATUS)
				mainmenuStatus = SELECT_VOLUME_STATUS;
			else if(mainmenuStatus==SELECT_VOLUME_STATUS)
				mainmenuStatus = SELECT_STATUS;
			//Update.
			menu_layer_reload_data(&mainMenu);
			break;
		case SCREEN_INDEX:
			//We will send the screen status at every Peng.
			turnScreen = !turnScreen;
			//Update.
			menu_layer_reload_data(&mainMenu);
			break;
		case HELP_INDEX:
			showHelp();
			break;
		case ABOUT_INDEX:
			showAbout();
			break;
		}
	}

void overrideSingleUp(ClickRecognizerRef recognizer, Window *window)
	{
	switch(mainmenuStatus)
		{
		case SELECT_VOLUME_STATUS:
			if(volume<VOLUME_MAX)
				{
				volume++;
				//Send with Peng!
				//sendCmd(CMD_VOLUME, volume);
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			break;
		case SELECT_STATUS:
			menu_layer_set_selected_next (&mainMenu,
											true,
											MenuRowAlignCenter,
											true);
			if(selectedMainMenuRow>0)
				selectedMainMenuRow--;
			break;
		}
	}

void overrideLongUp(ClickRecognizerRef recognizer, Window *window)
	{
	switch(mainmenuStatus)
		{
		case SELECT_VOLUME_STATUS:
			{
			uint8_t thrs = (VOLUME_MAX - VOLUME_MIN)/2;
			if(volume>=thrs)
				{
				volume = VOLUME_MAX;
				//Send with Peng!
				//sendCmd(CMD_VOLUME, volume);
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			else if(volume<thrs)
				{
				volume = thrs;
				//Send only if changed. App will store last value.
				sendCmd(CMD_VOLUME, volume);
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			break;
			}
		}
	}

void overrideSingleDown(ClickRecognizerRef recognizer, Window *window)
	{
	switch(mainmenuStatus)
		{
		case SELECT_VOLUME_STATUS:
			if(volume>VOLUME_MIN)
				{
				volume--;
				//Send only if changed. App will store last value.
				sendCmd(CMD_VOLUME, volume);
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			break;
		case SELECT_STATUS:
			menu_layer_set_selected_next (&mainMenu,
											false,
											MenuRowAlignCenter,
											true);
			if(selectedMainMenuRow<(MENU_ROWS_NUM-1))
				selectedMainMenuRow++;
			break;
		}
	}

void overrideLongDown(ClickRecognizerRef recognizer, Window *window)
	{
	switch(mainmenuStatus)
		{
		case SELECT_VOLUME_STATUS:
			{
			uint8_t thrs = (VOLUME_MAX - VOLUME_MIN)/2;
			if(volume>thrs)
				{
				volume = thrs;
				//Send only if changed. App will store last value.
				sendCmd(CMD_VOLUME, volume);
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			else if(volume<=thrs)
				{
				volume = VOLUME_MIN;
				//Send only if changed. App will store last value.
				sendCmd(CMD_VOLUME, volume);
				//Update.
				menu_layer_reload_data(&mainMenu);
				}
			break;
			}
		}
	}

void overrideConfigProvider(ClickConfig **config, void *context)
	{
	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler)overrideSingleSelect;
	config[BUTTON_ID_UP]->click.handler = (ClickHandler)overrideSingleUp;
		//config[BUTTON_ID_UP]->multi_click.handler = (ClickHandler) overrideMultiClickUp;
		//config[BUTTON_ID_UP]->multi_click.min = 2;
		//config[BUTTON_ID_UP]->multi_click.max = 2;
		//config[BUTTON_ID_UP]->multi_click.last_click_only = true;
	config[BUTTON_ID_UP]->long_click.handler = (ClickHandler) overrideLongUp;
	config[BUTTON_ID_UP]->long_click.delay_ms = 700;
	config[BUTTON_ID_DOWN]->click.handler = (ClickHandler)overrideSingleDown;
		//config[BUTTON_ID_DOWN]->multi_click.handler = (ClickHandler) overrideMultiClickDown;
		//config[BUTTON_ID_DOWN]->multi_click.min = 2;
		//config[BUTTON_ID_DOWN]->multi_click.max = 2;
		//config[BUTTON_ID_DOWN]->multi_click.last_click_only = true;
	config[BUTTON_ID_DOWN]->long_click.handler = (ClickHandler) overrideLongDown;
	config[BUTTON_ID_DOWN]->long_click.delay_ms = 700;
	}

