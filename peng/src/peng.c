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

#define MY_UUID { 0x89, 0x5F, 0x90, 0x96, 0x6D, 0x88, 0x48, 0xB5, 0xAF, 0xB7, 0xEA, 0x0C, 0x92, 0x94, 0x7E, 0x01 }

PBL_APP_INFO
	(
	MY_UUID,
	"Peng", "Manh Luong Bui",
	0, 2, /* App version */
	DEFAULT_MENU_ICON,
	APP_INFO_STANDARD_APP
	);

Window mainWnd;
MenuLayer mainMenu;

Window helpWnd;
ScrollLayer helpScroll;
TextLayer helpTextLayer;

Window aboutWnd;
ScrollLayer aboutScroll;
TextLayer aboutTextLayer;

//As with firmware 1.0.2, there are weird errors if not static.
static AppMessageCallbacksNode app_callbacks;

void handle_init(AppContextRef ctx)
	{
	(void)ctx;

	app_callbacks =
		(AppMessageCallbacksNode){
		.callbacks =
			{
			.out_sent = out_sent_peng,
			.out_failed = out_failed_peng,
			.in_received = in_received_peng,
			.in_dropped = in_failed_peng
			},
		.context = NULL
		};
	app_message_register_callbacks(&app_callbacks);

	window_init(&mainWnd, "Peng");
	addMainMenu();
	window_stack_push(&mainWnd, true);
	}

void pbl_main(void *params)
	{
	PebbleAppHandlers handlers =
		{
		.init_handler = &handle_init,
		.messaging_info =
			{
			.buffer_sizes = {
							.inbound = 256,
							.outbound = 256
							}
			}
		};
	app_event_loop(params, &handlers);
	}

/**
 * Cleanup.
 */
void window_unload(Window *window)
	{
	if(mainmenuStatus==PENGING_STATUS)
		sendCmd(CMD_PENG, CMD_PENG_STOP);
	}

void out_sent_peng(DictionaryIterator* sent, void* context)
	{
	if(dict_find(sent, CMD_PENG) &&
		dict_find(sent, CMD_SCREEN) &&
		dict_find(sent, CMD_VOLUME))
		{
		//MainMenu status update.
		mainmenuStatus = PENGING_STATUS;
		//Update.
		menu_layer_reload_data(&mainMenu);
		//Notify user.
		vibes_short_pulse();
		}
	}

void in_received_peng(DictionaryIterator* received, void* context)
	{
	Tuple* cmdTuple = NULL;
	cmdTuple = dict_find(received, CMD_PENG);

	if (cmdTuple == NULL)//We've got only one command from Peng for Android.
		{
		return;
		}

	uint8_t cmd = cmdTuple->value->int8;

	if (cmd == CMD_PENG_STOP)
		{
		mainmenuStatus = SELECT_STATUS;
		//Update.
		menu_layer_reload_data(&mainMenu);
		//Just for fun.
		vibes_long_pulse();
		}
	}

void out_failed_peng(DictionaryIterator* failed, AppMessageResult reason, void* context)
	{

	}

void in_failed_peng(void* context, AppMessageResult reason)
	{

	}

/**
 * Send a particular command with a particular value.
 */
void sendCmd(uint8_t cmd, uint8_t val)
	{
	Tuplet value = TupletInteger(cmd, val);
	DictionaryIterator *iter;
	app_message_out_get(&iter);
	if(iter == NULL)
		return;
	dict_write_tuplet(iter, &value);
	dict_write_end(iter);
	app_message_out_send();
	app_message_out_release();
	}

/**
 * Send both screen & volume in same dictionary.
 * Necessary because Service onStartCommand may be called multiple times.
 */
void sendPengStartCmd(uint8_t screen, uint8_t volume)
	{
	Tuplet pengVal = TupletInteger(CMD_PENG, ((uint8_t)CMD_PENG_START));
	Tuplet screenVal = TupletInteger(CMD_SCREEN, screen);
	Tuplet volVal = TupletInteger(CMD_VOLUME, volume);
	DictionaryIterator *iter;
	app_message_out_get(&iter);
	if(iter == NULL)
		return;
	dict_write_tuplet(iter, &pengVal);
	dict_write_tuplet(iter, &screenVal);
	dict_write_tuplet(iter, &volVal);
	dict_write_end(iter);
	app_message_out_send();
	app_message_out_release();
	}
