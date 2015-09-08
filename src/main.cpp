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

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Elementary.h>
#include "common.h"
#include "ise.h"

using namespace scim;

#include <Elementary.h>

CISECommon *g_ise_common = NULL;

extern KEYBOARD_STATE g_keyboard_state;

class CCoreEventCallback : public IISECommonEventCallback
{
    void init();
    void exit(int ic, const char *ic_uuid);

    void attach_input_context(int ic, const char *ic_uuid);
    void detach_input_context(int ic, const char *ic_uuid);

    void focus_in(int ic, const char *ic_uuid);
    void focus_out(int ic, const char *ic_uuid);

    void ise_show(int ic, const int degree, Ise_Context &context);
    void ise_hide(int ic, const char *ic_uuid);

    void set_rotation_degree(int degree);

    void update_cursor_position(int ic, const char *ic_uuid, int cursor_pos);
    void update_surrounding_text(int ic, const char *text, int cursor);
    void update_selection(int ic, const char *text);

    void set_language(unsigned int language);

    void set_imdata(char *buf, unsigned int len);
    void get_imdata(char **buf, unsigned int *len);

    void set_return_key_type(unsigned int type);
    void get_return_key_type(unsigned int *type);

    void set_return_key_disable(unsigned int disabled);
    void get_return_key_disable(unsigned int *disabled);

    void set_layout(unsigned int layout);
    void get_layout(unsigned int *layout);

    void reset_input_context(int ic, const char *uuid);

    void process_key_event(unsigned int code, unsigned int mask, unsigned int layout, unsigned int *ret);

    void set_display_language(const char *language);
    void set_accessibility_state(const int state);

    void set_caps_mode(int mode);

    void get_language_locale(int ic, char **locale);

    void signal_handler(int sig);
};

void CCoreEventCallback::init()
{
    LOGD("CCoreEventCallback::init()\n");
    ise_create();
}

void CCoreEventCallback::exit(int ic, const char *ic_uuid)
{
    ::ise_hide(ic);
    ise_destroy();
}

void CCoreEventCallback::attach_input_context(int ic, const char *ic_uuid)
{
    ise_attach_input_context(ic);
}

void CCoreEventCallback::detach_input_context(int ic, const char *ic_uuid)
{
    ise_detach_input_context(ic);
}

void CCoreEventCallback::focus_in(int ic, const char *ic_uuid)
{
    ise_focus_in(ic);
}

void CCoreEventCallback::focus_out(int ic, const char *ic_uuid)
{
    ise_focus_out(ic);
}

void CCoreEventCallback::ise_show(int ic, const int degree, Ise_Context &context)
{
    //g_ise_common->set_keyboard_ise_by_uuid(KEYBD_ISE_UUID);

    ise_reset_context(); // reset ISE

    /*if (context.language == ECORE_IMF_INPUT_PANEL_LANG_ALPHABET) {
        ise_explictly_set_language(PRIMARY_LATIN_LANGUAGE_INDEX);
    }*/

    ise_set_layout(context.layout);

    ise_set_return_key_type(context.return_key_type);
    ise_set_return_key_disable(context.return_key_disabled);

    ise_set_caps_mode(context.caps_mode);
    ise_update_cursor_position(ic, context.cursor_pos);

    /* Do not follow the application's rotation angle if we are already in visible state,
       since in that case we will receive the angle through ROTATION_CHANGE_REQUEST message */
    if (!(g_keyboard_state.visible_state)) {
        ise_set_screen_rotation(degree);
    } else {
        LOGD("Skipping rotation angle , %d", degree);
    }

    ::ise_show(ic);
}

void CCoreEventCallback::ise_hide(int ic, const char *ic_uuid)
{
    ::ise_hide(ic);
}

void CCoreEventCallback::set_rotation_degree(int degree)
{
    ise_set_screen_rotation(degree);
}

void CCoreEventCallback::update_cursor_position(int ic, const char *ic_uuid, int cursor_pos)
{
    ise_update_cursor_position(ic, cursor_pos);
}

void CCoreEventCallback::update_surrounding_text(int ic, const char *text, int cursor)
{
    ise_update_surrounding_text(ic, text, cursor);
}

void CCoreEventCallback::update_selection(int ic, const char *text)
{
    ise_update_selection(ic, text);
}

void CCoreEventCallback::set_language(unsigned int language)
{
    ise_set_language(language);
}

void CCoreEventCallback::set_imdata(char *buf, unsigned int len)
{
    ise_set_imdata(buf, len);
}

void CCoreEventCallback::get_imdata(char **buf, unsigned int *len)
{
    ise_get_imdata(buf, len);
}

void CCoreEventCallback::set_return_key_type(unsigned int type)
{
    ise_set_return_key_type(type);
}

void CCoreEventCallback::get_return_key_type(unsigned int *type)
{
    ise_get_return_key_type(type);
}

void CCoreEventCallback::set_return_key_disable(unsigned int disabled)
{
    ise_set_return_key_disable(disabled);
}

void CCoreEventCallback::get_return_key_disable(unsigned int *disabled)
{
    ise_get_return_key_disable(disabled);
}

void CCoreEventCallback::set_layout(unsigned int layout)
{
    ise_set_layout(layout);
}

void CCoreEventCallback::get_layout(unsigned int *layout)
{
    ise_get_layout(layout);
}

void CCoreEventCallback::reset_input_context(int ic, const char *uuid)
{
    ise_reset_input_context(ic);
}

void CCoreEventCallback::process_key_event(unsigned int code, unsigned int mask, unsigned int layout, unsigned int *ret)
{
    ise_process_key_event(code, mask, layout, ret);
}

void CCoreEventCallback::set_display_language(const char *language)
{
    setlocale(LC_ALL, language);
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    LOGD("Language : %s\n", (language ? language : "NULL"));
}

void CCoreEventCallback::set_accessibility_state(const int state)
{
    ise_set_accessibility_state(state);
}

void CCoreEventCallback::set_caps_mode(int mode)
{
    ise_set_caps_mode(mode);
}

void CCoreEventCallback::signal_handler(int sig)
{
    ise_signal_handler(sig);
}

void CCoreEventCallback::get_language_locale(int ic, char **locale)
{
    ise_get_language_locale(ic, locale);
}

static CCoreEventCallback g_core_event_callback;

extern "C"
{
    void scim_module_init(void) {
        if (g_ise_common == NULL) {
            g_ise_common = CISECommon::get_instance();
        }
        if  (g_ise_common) {
            g_ise_common->init();
            g_ise_common->set_core_event_callback(&g_core_event_callback);
        }
    }

    void scim_module_exit(void) {
        if (g_ise_common) {
            g_ise_common->exit();

            g_ise_common = NULL;
        }
    }

    unsigned int scim_helper_module_number_of_helpers(void) {
        if (g_ise_common) {
            return g_ise_common->get_number_of_helpers();
        }
        return 0;
    }

    bool scim_helper_module_get_helper_info(unsigned int idx, HelperInfo &info) {
        if (g_ise_common) {
            return g_ise_common->get_helper_info(idx, info);
        }
        return false;
    }

    String scim_helper_module_get_helper_language(unsigned int idx) {
        if (g_ise_common) {
            return g_ise_common->get_helper_language(idx);
        }
        return String("");
    }

    void scim_helper_module_run_helper(const String &uuid, const ConfigPointer &config, const String &display) {
        if (g_ise_common) {
            g_ise_common->run(uuid.c_str(), config, display.c_str());
        }
    }
}

