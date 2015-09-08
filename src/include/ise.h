/*
 * Copyright (c) 2012 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _ISE_H_
#define _ISE_H_

#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_IMF.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif  /*  */

//#include "languages.h"

#define ISE_VERSION "0.0.1-1"
#define LOCALEDIR "/usr/share/locale"

#define PRIMARY_LATIN_LANGUAGE "English"

#define DEFAULT_KEYBOARD_ISE_UUID "d75857a5-4148-4745-89e2-1da7ddaf7999"

//#define INPUT_MODE_NATIVE	MAX_INPUT_MODE /* Native mode. It will distinguish to the current user language */

//#define ISE_RELEASE_AUTOCOMMIT_BLOCK_INTERVAL 1300

#include <dlog.h>
#undef LOG_TAG
#define LOG_TAG "ISE_WEB_HELPER_AGENT"

enum ISE_LAYOUT{
    ISE_LAYOUT_STYLE_NORMAL = 0,
    ISE_LAYOUT_STYLE_NUMBER,
    ISE_LAYOUT_STYLE_EMAIL,
    ISE_LAYOUT_STYLE_URL,
    ISE_LAYOUT_STYLE_PHONENUMBER,
    ISE_LAYOUT_STYLE_IP,
    ISE_LAYOUT_STYLE_MONTH,
    ISE_LAYOUT_STYLE_NUMBERONLY,
    ISE_LAYOUT_STYLE_INVALID,
    ISE_LAYOUT_STYLE_HEX,
    ISE_LAYOUT_STYLE_TERMINAL,
    ISE_LAYOUT_STYLE_PASSWORD,
    ISE_LAYOUT_STYLE_DATETIME,

    ISE_LAYOUT_STYLE_MAX
};

typedef struct {
    int ic;
    int focused_ic;
    unsigned int layout;
    unsigned int caps_mode;
    unsigned int need_reset;
    unsigned int visible_state;

    int angle;
    int portrait_width;
    int portrait_height;
    int landscape_width;
    int landscape_height;
} KEYBOARD_STATE;

void ise_signal_handler(int sig);

void ise_log(const char *str);
void ise_send_string(const char *key_value);
void ise_update_preedit_string(const char *str);
void ise_send_event(int key_event, int key_mask);
void ise_forward_key_event(int key_event);
void ise_set_keyboard_size_hints(int portrait_width, int portrait_height, int landscape_width, int landscape_height);
void ise_set_selection(int start_index, int end_index);
void ise_get_selection();
void ise_get_surrounding_text(int maxlen_before, int maxlen_after);
void ise_delete_surrounding_text(int offset, int len);

void ise_create();
void ise_destroy();
void ise_attach_input_context(int ic);
void ise_detach_input_context(int ic);

void ise_focus_in(int ic);
void ise_focus_out(int ic);
void ise_show(int ic);
void ise_hide(int ic);
void ise_set_screen_rotation(int degree);
void ise_update_cursor_position(int ic, int position);
void ise_update_surrounding_text(int ic, const char *text, int cursor);
void ise_update_selection(int ic, const char *text);
void ise_set_language(unsigned int language);
void ise_set_imdata(char *buf, unsigned int len);
void ise_get_imdata(char **buf, unsigned int *len);
void ise_set_return_key_type(unsigned int type);
void ise_get_return_key_type(unsigned int *type);
void ise_set_return_key_disable(unsigned int disabled);
void ise_get_return_key_disable(unsigned int *disabled);
void ise_set_layout(unsigned int layout);
void ise_get_layout(unsigned int *layout);
void ise_reset_input_context(int ic);
void ise_process_key_event(unsigned int code, unsigned int mask, unsigned int layout, unsigned int *ret);

void ise_reset_context();
void ise_set_accessibility_state(int state);
void ise_set_caps_mode(unsigned int mode);
void ise_get_language_locale(int ic, char **locale);

#endif
