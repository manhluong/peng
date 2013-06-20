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

#ifndef VARIABLES_H
#define VARIABLES_H

#define MENU_ROWS_NUM 5

#define PENG_INDEX 0
#define VOLUME_INDEX 1
#define SCREEN_INDEX 2
#define HELP_INDEX 3
#define ABOUT_INDEX 4

#define PENG_ACTION "Peng!"
#define PENG_STOP "STOP!"
#define VOLUME_TEXT " Volume "
#define VOLUME_TEXT_SELECTED "[Volume]"
#define SCREEN_TEXT "Turn screen on"
#define HELP_TEXT "Help"
#define ABOUT_TEXT "About"

#define CMD_PENG 0x00
#define CMD_PENG_START 0x00
#define CMD_PENG_STOP 0x01

#define CMD_VOLUME 0x02 //Send volume number with this.

#define CMD_SCREEN 0x03
#define CMD_SCREEN_ON 0x04
#define CMD_SCREEN_OFF 0x05

#define SELECT_STATUS 0x00
#define SELECT_VOLUME_STATUS 0x01
#define PENGING_STATUS 0x02

#define VOLUME_MAX 100
#define VOLUME_MIN 0
#define VOLUME_DEFAULT VOLUME_MAX

extern Window mainWnd;
extern MenuLayer mainMenu;

extern Window helpWnd;
extern ScrollLayer helpScroll;
extern TextLayer helpTextLayer;

extern Window aboutWnd;
extern ScrollLayer aboutScroll;
extern TextLayer aboutTextLayer;

extern uint8_t mainmenuStatus;
extern uint8_t volume;
extern bool turnScreen;

extern uint8_t selectedMainMenuRow;

#endif
