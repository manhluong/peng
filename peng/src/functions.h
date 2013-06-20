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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//MainMenu.

void addMainMenu();
void overrideSingleSelect(ClickRecognizerRef recognizer, Window *window);
void overrideSingleUp(ClickRecognizerRef recognizer, Window *window);
void overrideLongUp(ClickRecognizerRef recognizer, Window *window);
void overrideSingleDown(ClickRecognizerRef recognizer, Window *window);
void overrideLongDown(ClickRecognizerRef recognizer, Window *window);
void overrideConfigProvider(ClickConfig **config, void *context);

//Sending messages.

void sendCmd(uint8_t cmd, uint8_t val);
void sendPengStartCmd(uint8_t screen, uint8_t volume);

//Info windows.

void showHelp();
void showAbout();

void in_received_peng(DictionaryIterator* received, void* context);
void out_sent_peng(DictionaryIterator* sent, void* context);
void out_failed_peng(DictionaryIterator* failed, AppMessageResult reason, void* context);
void in_failed_peng(void* context, AppMessageResult reason);

#endif
