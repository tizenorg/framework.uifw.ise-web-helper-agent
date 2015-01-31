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

#include <dlfcn.h>

#include <dlog.h>

#include "web_container.h"

#define LOG_TAG "ISE_WEB_HELPER_AGENT"

static void *_plugin_handle = NULL;

bool web_container_create(Evas_Object *window, HelperInfoEx info, web_container_loaded_cb callback)
{
    bool ret = false;
    if (_plugin_handle == NULL) {
        _plugin_handle = dlopen(WEB_CONTAINER_PLUGIN_PATH, RTLD_NOW);
        if (_plugin_handle) {
            ret = true;
        } else {
            LOGD("dlopen failed : %s %s", WEB_CONTAINER_PLUGIN_PATH, dlerror());
        }
    }

    if (ret) {
        web_container_create_api api =
            (web_container_create_api)dlsym(_plugin_handle, WEB_CONTAINER_CREATE_API);
        char *result = dlerror();
        if (result) {
            LOGD("dlsym failed : %s %s", WEB_CONTAINER_CREATE_API, result);
        } else {
            api(window, info, callback);
        }
    }

    return ret;
}

bool web_container_destroy()
{
    bool ret = false;
    if (_plugin_handle) {
        web_container_destroy_api api =
            (web_container_destroy_api)dlsym(_plugin_handle, WEB_CONTAINER_DESTROY_API);
        char *result = dlerror();
        if (result) {
            LOGD("dlsym failed : %s %s", WEB_CONTAINER_DESTROY_API, result);
        } else {
            api();
        }

        dlclose(_plugin_handle);
        _plugin_handle = NULL;
        ret = true;
    }
    return ret;
}

bool web_container_resize(int width, int height)
{
    bool ret = false;
    if (_plugin_handle) {
        web_container_resize_api api =
            (web_container_resize_api)dlsym(_plugin_handle, WEB_CONTAINER_RESIZE_API);
        char *result = dlerror();
        if (result) {
            LOGD("dlsym failed : %s %s", WEB_CONTAINER_RESIZE_API, result);
        } else {
            api(width, height);
        }

        ret = true;
    }
    return ret;
}

bool web_container_javascript(const char *command, web_container_javascript_cb callback)
{
    bool ret = false;
    if (_plugin_handle) {
        web_container_javascript_api api =
            (web_container_javascript_api)dlsym(_plugin_handle, WEB_CONTAINER_JAVASCRIPT_API);
        char *result = dlerror();
        if (result) {
            LOGD("dlsym failed : %s %s", WEB_CONTAINER_JAVASCRIPT_API, result);
        } else {
            api(command, callback);
        }

        ret = true;
    }
    return ret;
}
