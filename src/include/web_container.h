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

#define WEB_CONTAINER_PLUGIN_PATH "/usr/lib/scim-1.0/web-container.so"

#define WEB_CONTENT_PREPARE_COMMAND "WebHelperClient.impl.prepare"
#define WEB_CONTENT_ACTIVATE_COMMAND "WebHelperClient.impl.activate"

#include "packages.h"
#include "Evas.h"

/* Plugin interfaces */
#define WEB_CONTAINER_CREATE_API "web_container_create"
#define WEB_CONTAINER_DESTROY_API "web_container_destroy"
#define WEB_CONTAINER_RESIZE_API "web_container_resize"
#define WEB_CONTAINER_JAVASCRIPT_API "web_container_javascript"

typedef void (*web_container_javascript_cb)(const char* result_value);
typedef void (*web_container_loaded_cb)();

typedef bool (*web_container_create_api) (Evas_Object *window, HelperInfoEx info, web_container_loaded_cb callback);
typedef bool (*web_container_destroy_api) ();
typedef bool (*web_container_resize_api) (int width, int height);
typedef bool (*web_container_javascript_api) (const char *command, web_container_javascript_cb callback);

/* Container interfaces */
extern "C" {
bool web_container_create(Evas_Object *window, HelperInfoEx info, web_container_loaded_cb callback);
bool web_container_destroy();

bool web_container_resize(int width, int height);
bool web_container_javascript(const char *command, web_container_javascript_cb callback);
}
