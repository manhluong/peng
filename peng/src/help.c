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

/**
 * Simply push to the stack a window with a TextLayer.
 */
void showHelp()
	{
	window_init(&helpWnd, HELP_TEXT);
	//Create the TextLayer.
	text_layer_init(&helpTextLayer, helpWnd.layer.frame);
	text_layer_set_text(&helpTextLayer,
			"Please be advised that this app is a test: use it at your own risk.\nMore info: www.luongbui.com\n\nUse UP/DOWN to navigate the menu.\n\nSELECT behavior changes according to selected item.\n\nSELECT at PENG! to trig the paired smartphone. Press again to stop. Peng will reset the status after it received the ack from the smartphone.\nIf it doesn't stop, just press Stop! again.\n\nSELECT at Volume to change volume level. While you read [Volume], UP/DOWN to change volume level. Long press to set to 0, halfway and maximum.\n\nVolume is set only for the ringtone. Android app will try to restore your previous volume level.\n\nSELECT again to return to menu navigation.\n\nSELECT at the checkbox to choose to turn on / off the screen at ringtone trig.\n");
	//Boost the maximum TextLayer size.
	GSize txtMaxSize = text_layer_get_max_used_size(app_get_current_graphics_context(),&helpTextLayer);
	txtMaxSize.h = (helpTextLayer.layer.bounds.size.h * 3) + 120;
	text_layer_set_size(&helpTextLayer, txtMaxSize);
	//Create the ScrollLayer.
	scroll_layer_init(&helpScroll,
						GRect(0, 0,
								helpWnd.layer.frame.size.w,
								helpWnd.layer.frame.size.h-15));
	scroll_layer_add_child(&helpScroll, &helpTextLayer.layer);
	//Set the content size of the ScrollLayer with the boosted TextLayer size.
	scroll_layer_set_content_size(&helpScroll,
									txtMaxSize);
	//Default UP/DOWN behavior.
	scroll_layer_set_click_config_onto_window(&helpScroll, &helpWnd);

	//Add the ScrollLayer.
	layer_add_child(&helpWnd.layer, &helpScroll.layer);

	window_stack_push(&helpWnd, true);
	}
