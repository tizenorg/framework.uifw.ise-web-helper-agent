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

#include <Ecore_X.h>
#include <Ecore_IMF.h>
#include <Elementary.h>

#include <X11/XF86keysym.h>

#include <vconf.h>
#include <vconf-keys.h>

#include <package_manager.h>
#include <pkgmgr-info.h>

#include "ise.h"
#include "common.h"
#include "packages.h"
#include "web_helper_agent.h"

#include "web_container.h"

#include <memory>

extern CISECommon *g_ise_common;

static CWebHelperAgent *g_web_helper_agent = NULL;
static package_manager_h g_package_manager = NULL;

KEYBOARD_STATE g_keyboard_state = {
    0,
    0,
    ISE_LAYOUT_STYLE_NORMAL,
    FALSE,
    TRUE,
    FALSE,
};

int
check_ic_temporary(int ic)
{
    if ((ic & 0xFFFF) == 0) {
        return TRUE;
    }
    return FALSE;
}

/**
 * Leave a log
 */
void
ise_log(const char *str)
{
    LOGD("ISELOG : %s", str);
}

/**
 * Send the given string to input framework
 */
void
ise_send_string(const char *key_value)
{
    int ic = -1;
    if (!check_ic_temporary(g_keyboard_state.ic)) {
        ic = g_keyboard_state.ic;
    }
    g_ise_common->hide_preedit_string(ic, "");
    g_ise_common->commit_string(ic, "", key_value);
    LOGD("ic : %x, %s", ic, key_value);
}

/**
* Send the preedit string to input framework
*/
void
ise_update_preedit_string(const char *str)
{
    int ic = -1;
    if (!check_ic_temporary(g_keyboard_state.ic)) {
        ic = g_keyboard_state.ic;
    }
    g_ise_common->update_preedit_string(ic, "", str);
    LOGD("ic : %x, %s", ic, str);
}

/**
* Send the given event to input framework
*/
void
ise_send_event(int key_event, int key_mask)
{
    int ic = -1;
    if (!check_ic_temporary(g_keyboard_state.ic)) {
        ic = g_keyboard_state.ic;
    }
    g_ise_common->forward_key_event(ic, "", key_event, scim::SCIM_KEY_NullMask);
    g_ise_common->forward_key_event(ic, "", key_event, scim::SCIM_KEY_ReleaseMask);
    LOGD("ic : %x, %x", ic, key_event);
}

/**
* Forward the given event to input framework
*/
void
ise_forward_key_event(int key_event)
{
    int ic = -1;
    if (!check_ic_temporary(g_keyboard_state.ic)) {
        ic = g_keyboard_state.ic;
    }
    g_ise_common->forward_key_event(ic, "", key_event, scim::SCIM_KEY_NullMask);
    g_ise_common->forward_key_event(ic, "", key_event, scim::SCIM_KEY_ReleaseMask);
    LOGD("ic : %x, %x", ic, key_event);
}

void ise_set_keyboard_size_hints(int portrait_width, int portrait_height, int landscape_width, int landscape_height)
{
    g_keyboard_state.portrait_width = portrait_width;
    g_keyboard_state.portrait_height = portrait_height;
    g_keyboard_state.landscape_width = landscape_width;
    g_keyboard_state.landscape_height = landscape_height;

    g_ise_common->set_keyboard_size_hints(portrait_width, portrait_height, landscape_width, landscape_height);

    if (g_keyboard_state.angle == 90 || g_keyboard_state.angle == 270) {
        web_container_resize(g_keyboard_state.landscape_width, g_keyboard_state.landscape_height);
    } else {
        web_container_resize(g_keyboard_state.portrait_width, g_keyboard_state.portrait_height);
    }
}

void ise_set_selection(int start_index, int end_index)
{
    g_ise_common->set_selection(start_index, end_index);
}

void ise_get_selection()
{
    g_ise_common->get_selection();
}

void ise_get_surrounding_text(int maxlen_before, int maxlen_after)
{
    g_ise_common->get_surrounding_text(maxlen_before, maxlen_after);
}

void ise_delete_surrounding_text(int offset, int len)
{
    g_ise_common->delete_surrounding_text(offset, len);
}

void web_content_prepared(const char* result_value)
{
    if (result_value) {
        std::string version_string = result_value;
        LOGD("Version : %s", version_string.c_str());

        std::vector<std::string> vec = CStringTokenizer::split(version_string, VERSION_DELIMITER);

        if (vec.size() == VERSION_TOKEN_NUM) {
            WEB_HELPER_AGENT_TYPE type =
                CWebHelperAgent::get_web_helper_agent_type_from_major_version(atoi(vec.at(0).c_str()));
            if (g_web_helper_agent) {
                g_web_helper_agent->exit();
                CWebHelperAgent::destroy_web_helper_agent(g_web_helper_agent);
            }
            g_web_helper_agent = CWebHelperAgent::create_web_helper_agent(type);
            if (g_web_helper_agent) {
                g_web_helper_agent->init();
            }
        }
    }

    const int MAX_COMMAND_LENGTH = 255;
    char activate_command[MAX_COMMAND_LENGTH + 1] = {'\0'};
    snprintf(activate_command, MAX_COMMAND_LENGTH, "%s();", WEB_CONTENT_ACTIVATE_COMMAND);
    activate_command[MAX_COMMAND_LENGTH] = '\0';

    if (web_container_javascript(activate_command, NULL) == EINA_FALSE) {
        LOGW("Failed while executing command : %s", WEB_CONTENT_ACTIVATE_COMMAND);
    }
}

void web_container_loaded()
{
    const int MAX_COMMAND_LENGTH = 255;
    char prepare_command[MAX_COMMAND_LENGTH + 1] = {'\0'};
    snprintf(prepare_command, MAX_COMMAND_LENGTH, "%s('%s');",
        WEB_CONTENT_PREPARE_COMMAND, CMagicKeyManager::get_magic_key().c_str());
    prepare_command[MAX_COMMAND_LENGTH] = '\0';
    LOGD("Executing command : %s", WEB_CONTENT_PREPARE_COMMAND);
    if (web_container_javascript(prepare_command, web_content_prepared) == false) {
        LOGW("Failed while executing command : %s", WEB_CONTENT_PREPARE_COMMAND);
    }
}

static bool
app_info_cb (package_info_app_component_type_e comp_type, const char *app_id, void *user_data)
{
    pkgmgrinfo_appinfo_h appinfo_handle;
    bool exist = false;
    bool *result = static_cast<bool*>(user_data);

    if (pkgmgrinfo_appinfo_get_appinfo (app_id, &appinfo_handle) != PMINFO_R_OK)
        return true;

    pkgmgrinfo_appinfo_is_category_exist (appinfo_handle, "http://tizen.org/category/ime", &exist);

    if (exist) {
        /* FIXME : need to use generated UUID */
        LOGD ("package being installed or updated, : %s %s\n", app_id, g_ise_common->get_ise_uuid());
        if (strcmp(app_id, g_ise_common->get_ise_uuid()) == 0) {
            LOGD ("Same appid, reloading");
            if (result) {
                *result = true;
            }
        }
    }

    pkgmgrinfo_appinfo_destroy_appinfo (appinfo_handle);

    return true;
}

static void check_helper_ise_info(const char *type, const char *package)
{
    package_info_h pkg_info = NULL;
    char *pkg_label = NULL;
    char *pkg_icon_path = NULL;
    bool result = false;

    if (!type)
        return;

    if (strncmp (type, "wgt", 3) != 0)
        return;

    if (package_info_create (package, &pkg_info) != PACKAGE_MANAGER_ERROR_NONE)
        return;

    if (!pkg_info)
        return;

    package_info_get_label (pkg_info, &pkg_label);
    package_info_get_icon (pkg_info, &pkg_icon_path);

    package_info_foreach_app_from_package (pkg_info, PACKAGE_INFO_UIAPP, app_info_cb, &result);

    if (pkg_label)
        free (pkg_label);

    if (pkg_icon_path)
        free (pkg_icon_path);

    package_info_destroy (pkg_info);

    if (result) {
        scim::HelperAgent *helper_agent = g_ise_common->get_helper_agent();
        if (helper_agent) {
            helper_agent->update_ise_exit();
            helper_agent->close_connection();
        }
        IISECommonEventCallback *callback = g_ise_common->get_core_event_callback();
        if (callback) {
            callback->exit(-1, "");
        }
        elm_exit();
    }
}

static void _package_manager_event_cb(const char *type, const char *package, package_manager_event_type_e event_type, package_manager_event_state_e event_state, int progress, package_manager_error_e error, void *user_data)
{
    if (!package || !type) return;

    if (event_type == PACKAGE_MANAGER_EVENT_TYPE_INSTALL ||
            event_type == PACKAGE_MANAGER_EVENT_TYPE_UPDATE) {

        if (event_state != PACKAGE_MANAGER_EVENT_STATE_COMPLETED)
            return;

        check_helper_ise_info (type, package);
    }
}

void
ise_create()
{
    if (g_package_manager == NULL) {
        package_manager_create(&g_package_manager);
        package_manager_set_event_cb(g_package_manager, _package_manager_event_cb, NULL);
    }

    if (g_ise_common) {
        if (g_ise_common->get_main_window()) {
            ecore_x_icccm_name_class_set(
                elm_win_xwindow_get(static_cast<Evas_Object*>(g_ise_common->get_main_window())),
                "Virtual Keyboard", "ISF" );
            elm_win_prop_focus_skip_set(g_ise_common->get_main_window(), EINA_TRUE);
        }
    }

    Evas_Object* bg = elm_bg_add(g_ise_common->get_main_window());
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_bg_color_set(bg, 0, 0, 0);
    elm_win_resize_object_add(g_ise_common->get_main_window(), bg);
    evas_object_show(bg);

    HelperInfoEx info;
    CPackages *packages = CPackages::get_instance();
    if (packages) {
        info = packages->get_package_information_by_uuid(g_ise_common->get_ise_uuid());
    }

    web_container_create(g_ise_common->get_main_window(), info, web_container_loaded);
}

void
ise_destroy()
{
    if (g_package_manager) {
        package_manager_destroy(g_package_manager);
        g_package_manager = NULL;
    }

    if (g_web_helper_agent) {
        g_web_helper_agent->exit();
        CWebHelperAgent::destroy_web_helper_agent(g_web_helper_agent);
    }

    web_container_destroy();
}

void
ise_attach_input_context(int ic)
{
    LOGD("attaching, ic : %x , %x , g_ic : %x , %x, g_focused_ic : %x , %x", ic, check_ic_temporary(ic),
        g_keyboard_state.ic, check_ic_temporary(g_keyboard_state.ic),
        g_keyboard_state.focused_ic, check_ic_temporary(g_keyboard_state.focused_ic));
    ise_focus_in(ic);
}

void
ise_detach_input_context(int ic)
{
    ise_focus_out(ic);
}

void
ise_focus_in(int ic)
{
    LOGD("ic : %x , %x , g_ic : %x , %x, g_focused_ic : %x , %x", ic, check_ic_temporary(ic),
        g_keyboard_state.ic, check_ic_temporary(g_keyboard_state.ic),
        g_keyboard_state.focused_ic, check_ic_temporary(g_keyboard_state.focused_ic));
    if (check_ic_temporary(g_keyboard_state.ic) && !check_ic_temporary(ic)) {
        g_keyboard_state.ic = ic;
    }
    g_keyboard_state.focused_ic = ic;
    if (ic == g_keyboard_state.ic) {
        if (g_ise_common) {
            if (g_keyboard_state.layout == ISE_LAYOUT_STYLE_PHONENUMBER ||
                g_keyboard_state.layout == ISE_LAYOUT_STYLE_IP ||
                g_keyboard_state.layout == ISE_LAYOUT_STYLE_MONTH ||
                g_keyboard_state.layout == ISE_LAYOUT_STYLE_NUMBERONLY) {
                    g_ise_common->set_keyboard_ise_by_uuid(DEFAULT_KEYBOARD_ISE_UUID);
            } else {
                g_ise_common->set_keyboard_ise_by_uuid(g_ise_common->get_keyboard_ise_uuid().c_str());
            }
        }
    }
    if (g_web_helper_agent) {
        g_web_helper_agent->on_focus_in(ic);
    }
}

void
ise_focus_out(int ic)
{
    g_keyboard_state.focused_ic = 0;

    if (g_web_helper_agent) {
        g_web_helper_agent->on_focus_out(ic);
    }
}

void
ise_show(int ic)
{
    if (g_ise_common) {
        LOGD("ic : %x , %x , g_ic : %x , %x, g_focused_ic : %x , %x", ic, check_ic_temporary(ic),
            g_keyboard_state.ic, check_ic_temporary(g_keyboard_state.ic),
            g_keyboard_state.focused_ic, check_ic_temporary(g_keyboard_state.focused_ic));
        if (check_ic_temporary(ic) && !check_ic_temporary(g_keyboard_state.focused_ic)) {
            ic = g_keyboard_state.focused_ic;
        }
        if (!check_ic_temporary(ic) && check_ic_temporary(g_keyboard_state.focused_ic)) {
            g_keyboard_state.focused_ic = ic;
        }
        g_keyboard_state.ic = ic;

        if (g_web_helper_agent) {
            g_web_helper_agent->on_show(ic);
        }

        evas_object_show(g_ise_common->get_main_window());
    }

    g_keyboard_state.visible_state = TRUE;
}

void
ise_hide(int ic)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_hide(ic);
    }
    evas_object_hide(g_ise_common->get_main_window());
    g_keyboard_state.visible_state = FALSE;
}

/**
 * Sets screen rotation
 */
void
ise_set_screen_rotation(int degree)
{
    g_keyboard_state.angle = degree;
    if (g_keyboard_state.angle == 90 || g_keyboard_state.angle == 270) {
        web_container_resize(g_keyboard_state.landscape_width, g_keyboard_state.landscape_height);
    } else {
        web_container_resize(g_keyboard_state.portrait_width, g_keyboard_state.portrait_height);
    }

    if (g_web_helper_agent) {
        g_web_helper_agent->on_set_rotation(degree);
    }
}

void
ise_update_cursor_position(int ic, int position)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_update_cursor_position(ic, position);
    }
}

void
ise_update_surrounding_text(int ic, const char *text, int cursor)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_update_surrounding_text(ic, text, cursor);
    }
}

void
ise_update_selection(int ic, const char *text)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_update_selection(ic, text);
    }
}

void
ise_set_language(unsigned int language)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_set_language(language);
    }
}

void ise_set_imdata(char *buf, unsigned int len)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_set_imdata(buf, len);
    }
}

void ise_get_imdata(char **buf, unsigned int *len)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_get_imdata(buf, len);
        if (buf && len) {
            if (*buf) {
                LOGD("imdata : %s, %d", *buf, *len);
            }
        }
    }
}

void ise_set_return_key_type(unsigned int type)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_set_return_key_type(type);
    }
}

void ise_get_return_key_type(unsigned int *type)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_get_return_key_type(type);
    }
}

void ise_set_return_key_disable(unsigned int disabled)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_set_return_key_type(disabled);
    }
}

void ise_get_return_key_disable(unsigned int *disabled)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_get_return_key_type(disabled);
    }
}

void
ise_set_layout(unsigned int layout)
{
    /* Check if the layoutIdx is in the valid range */
    if (layout < ISE_LAYOUT_STYLE_MAX) {
        if (g_keyboard_state.layout != layout) {
            g_keyboard_state.need_reset = TRUE;
        }
        g_keyboard_state.layout = layout;

        if (g_web_helper_agent) {
            g_web_helper_agent->on_set_layout(layout);
        }
    }
}

void
ise_get_layout(unsigned int *layout)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_get_layout(layout);
    }
}

void
ise_reset_input_context(int ic)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_reset_input_context(ic);
    }
}


void ise_process_key_event(unsigned int code, unsigned int mask, unsigned int layout, unsigned int *ret)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->on_process_key_event(code, mask, layout, ret);
    }
}

void
ise_reset_context()
{
}

void
ise_set_accessibility_state(int state)
{
}

static void set_caps_mode(int mode) {
}

void ise_signal_handler(int sig)
{
    if (g_web_helper_agent) {
        g_web_helper_agent->signal(sig);
    }
}

void
ise_set_caps_mode(unsigned int mode)
{
    if (mode) {
        g_keyboard_state.caps_mode = TRUE;
    } else {
        g_keyboard_state.caps_mode = FALSE;
    }
}

void ise_get_language_locale(char **locale)
{
}
