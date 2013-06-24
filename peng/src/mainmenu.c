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
uint8_t flashlight;

/******************************************************/

//Need to manually track selection because menu_layer_get_selected_index() missing from the header.
//Issue #22 in github: https://github.com/pebble/pebblekit/issues/22
MenuIndex selection;

void increaseSelection()
	{
	uint16_t maxRowsInSection = mainMenuGetNumRows (&mainMenu,
													selection.section,
													NULL);
	uint16_t maxSectionsNumber = mainMenuGetNumSections(&mainMenu, NULL);
	if(selection.row < maxRowsInSection-1)
		selection.row++;
	else if(selection.section < maxSectionsNumber-1)
		{
		selection.row = 0;
		selection.section++;
		}
	}

void decreaseSelection()
	{
	if(selection.row > 0)
		selection.row--;
	else if(selection.section > 0)
		{
		selection.section--;
		selection.row = (mainMenuGetNumRows (&mainMenu,
												selection.section,
												NULL) -1);
		}
	}

/******************************************************/

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

void mainHeaderDraw(GContext *ctx,
					const Layer *cell_layer,
					uint16_t section_index,
					void *callback_context)
	{
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_fill_rect(ctx,
						GRect(0,
								0,
								cell_layer->frame.size.w,
								cell_layer->frame.size.h),
								0,
								GCornerNone);
	graphics_context_set_text_color(ctx, GColorWhite);
	switch(section_index)
		{
		case MENU_SECTION_PENG:
			graphics_text_draw(ctx,
								MENU_SECTION_PENG_TITLE,
								fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
								GRect(5,0,cell_layer->frame.size.w-5,cell_layer->frame.size.h),
								GTextOverflowModeTrailingEllipsis,
								GTextAlignmentLeft,
								NULL);
			break;
		case MENU_SECTION_FLASHLIGHT:
			graphics_text_draw(ctx,
								MENU_SECTION_FLASHLIGHT_TITLE,
								fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
								GRect(5,0,cell_layer->frame.size.w-5,cell_layer->frame.size.h),
								GTextOverflowModeTrailingEllipsis,
								GTextAlignmentLeft,
								NULL);
			break;
		case MENU_SECTION_INFO:
			graphics_text_draw(ctx,
								MENU_SECTION_INFO_TITLE,
								fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
								GRect(5,0,cell_layer->frame.size.w-5,cell_layer->frame.size.h),
								GTextOverflowModeTrailingEllipsis,
								GTextAlignmentLeft,
								NULL);
			break;
		}
	}

//Without menu_layer_reload_data(&mainMenu); it won't redraw.
void mainMenuDrawRow(GContext *ctx,
					   const Layer *cellLayer,
					   MenuIndex *menuIndex,
					   void *clbCtx)
	{
	graphics_context_set_text_color(ctx, GColorBlack);
	if(menuIndex->section == MENU_SECTION_PENG)
		{
		switch(menuIndex->row)
			{
			case PENG_INDEX:
				if(mainmenuStatus!=PENGING_STATUS)
					graphics_text_draw(ctx,
							PENG_ACTION,
							fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
							GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
							GTextOverflowModeTrailingEllipsis,
							GTextAlignmentLeft,
							NULL);
				else if(mainmenuStatus==PENGING_STATUS)
					graphics_text_draw(ctx,
							PENG_STOP,
							fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
							GRect(5,0,cellLayer->frame.size.w-5,cellLayer->frame.size.h),
							GTextOverflowModeTrailingEllipsis,
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
			}
		}
	else if(menuIndex->section == MENU_SECTION_FLASHLIGHT &&
			menuIndex->row == FLASHLIGHT_INDEX)
		{
		//Label
		graphics_text_draw(ctx,
							FLASHLIGHT_TEXT,
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
		if(flashlight==CMD_FLASHLIGHT_ON)
			{
			graphics_fill_rect(ctx,
								GRect(9,
										14,
										cellLayer->frame.size.h-28,
										cellLayer->frame.size.h-28),
								3,
								GCornersAll);
			}
		else if(flashlight==CMD_FLASHLIGHT_NOT_AVAILABLE)
			{
			graphics_draw_line(ctx,
								GPoint(5,
									cellLayer->frame.size.h-25),
								GPoint(cellLayer->frame.size.w-5,
									cellLayer->frame.size.h-25));
			graphics_draw_line(ctx,
								GPoint(5,
										cellLayer->frame.size.h-23),
								GPoint(cellLayer->frame.size.w-5,
										cellLayer->frame.size.h-23));
			graphics_draw_line(ctx,
								GPoint(5,
										cellLayer->frame.size.h-22),
								GPoint(cellLayer->frame.size.w-5,
										cellLayer->frame.size.h-22));
			graphics_draw_line(ctx,
								GPoint(5,
										cellLayer->frame.size.h-20),
								GPoint(cellLayer->frame.size.w-5,
										cellLayer->frame.size.h-20));
			}
		}
	else if(menuIndex->section == MENU_SECTION_INFO)
		{
		switch(menuIndex->row)
			{
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
	}

int16_t mainMenuGetHeaderHeight(struct MenuLayer *menuLayer,
									uint16_t sectIndex,
									void *context)
	{
	return 17;
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
	if(sectionIndex==MENU_SECTION_PENG)
		return MENU_ROWS_NUM_PENG;
	if(sectionIndex==MENU_SECTION_FLASHLIGHT)
		return MENU_ROWS_NUM_FLASHLIGHT;
	if(sectionIndex==MENU_SECTION_INFO)
		return MENU_ROWS_NUM_INFO;
	return 0;
	}

uint16_t mainMenuGetNumSections(struct MenuLayer *menuLayer,
								void *context)
	{
	return 3;
	}

void addMainMenu()
	{
	menu_layer_init(&mainMenu,
					GRect(0, 0,
						  mainWnd.layer.frame.size.w,
						  mainWnd.layer.frame.size.h-15)); //-15 for the title bar.

	mainmenuStatus = SELECT_STATUS;
	turnScreen = true;
	//Start with not available. Check flashlight status asap.
	flashlight = CMD_FLASHLIGHT_NOT_AVAILABLE;
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
		.draw_row = &mainMenuDrawRow,
		.draw_header = &mainHeaderDraw
		};
	menu_layer_set_callbacks(&mainMenu, NULL, mainMenuClbs);

	selection.row = PENG_INDEX;
	selection.section = MENU_SECTION_PENG;
	menu_layer_set_selected_index (&mainMenu,
									selection,
									MenuRowAlignCenter,
									true);

	layer_add_child(&mainWnd.layer, menu_layer_get_layer(&mainMenu));

	//Check flashlight status.
	//Expect CMD_FLASHLIGHT_CHECK in response.
	sendCmd(CMD_FLASHLIGHT, CMD_FLASHLIGHT_CHECK);
	}

void overrideSingleSelect(ClickRecognizerRef recognizer, Window *window)
	{
	if(selection.section == MENU_SECTION_PENG)
		{
		switch(selection.row)
			{
			case PENG_INDEX:
				if(mainmenuStatus==SELECT_STATUS)
					{
					uint8_t scr = CMD_SCREEN_OFF;
					if(turnScreen)
						scr = CMD_SCREEN_ON;
					//Send command.
					sendPengStartCmd(scr, volume);
					//Status switch is in out_sent_peng().
					//So status changes only if the cmd is actually sent.
					//mainmenuStatus = PENGING_STATUS;
					}
				else if(mainmenuStatus==PENGING_STATUS)
					{
					//Expect CMD_PENG in response.
					sendCmd(CMD_PENG, CMD_PENG_STOP);
					//Status switch is in in_received_peng().
					//This way the user can resend a Stop if it fails.
					//mainmenuStatus = SELECT_STATUS;
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
			}
		}
	else if(selection.section == MENU_SECTION_FLASHLIGHT &&
			selection.row == FLASHLIGHT_INDEX)
		{
		//Send cmd.
		//Expect CMD_FLASHLIGHT_CHECK in response, which change the status too.
		if(flashlight!=CMD_FLASHLIGHT_NOT_AVAILABLE)
			{
			if(flashlight==CMD_FLASHLIGHT_ON)
				sendCmd(CMD_FLASHLIGHT, CMD_FLASHLIGHT_OFF);
			else if(flashlight==CMD_FLASHLIGHT_OFF)
				sendCmd(CMD_FLASHLIGHT, CMD_FLASHLIGHT_ON);
			}
		else//Manually send a check flashlight presence.
			sendCmd(CMD_FLASHLIGHT, CMD_FLASHLIGHT_CHECK);//Expect CMD_FLASHLIGHT_CHECK in response.
		}
	else if(selection.section == MENU_SECTION_INFO)
		{
		switch(selection.row)
			{
			case HELP_INDEX:
				showHelp();
				break;
			case ABOUT_INDEX:
				showAbout();
				break;
			}
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
				menu_layer_reload_data(&mainMenu);
				}
			break;
		case SELECT_STATUS:
			menu_layer_set_selected_next (&mainMenu,
											true,
											MenuRowAlignCenter,
											true);
			decreaseSelection();
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
				menu_layer_reload_data(&mainMenu);
				}
			else if(volume<thrs)
				{
				volume = thrs;
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
				menu_layer_reload_data(&mainMenu);
				}
			break;
		case SELECT_STATUS:
			menu_layer_set_selected_next (&mainMenu,
											false,
											MenuRowAlignCenter,
											true);
			increaseSelection();
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
				menu_layer_reload_data(&mainMenu);
				}
			else if(volume<=thrs)
				{
				volume = VOLUME_MIN;
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

