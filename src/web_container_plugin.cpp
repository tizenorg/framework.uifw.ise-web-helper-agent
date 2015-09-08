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

#include "web_container.h"

#include <dlog.h>
#include <Evas.h>

#include <EWebKit.h>

#include <i_runnable_widget_object.h>
#include <core_module.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ISF_WEB_CONTAINER"

typedef std::shared_ptr<WRT::IRunnableWidgetObject> WrtCorePtr;

static WrtCorePtr _pWrt;
static Evas_Object *_pCurrentWebView = NULL;

static web_container_loaded_cb _loaded_callback = NULL;

static void web_container_loaded(Evas_Object* webview, void* eventInfo)
{
    if (_loaded_callback) {
        _loaded_callback();
    }
}

static void ewk_javascript_callback(Evas_Object* webview, const char* result_value, void* user_data)
{
    if (user_data) {
        web_container_javascript_cb callback = (web_container_javascript_cb)user_data;
        callback(result_value);
    }
}

bool web_container_create(Evas_Object *window, HelperInfoEx info, web_container_loaded_cb callback)
{
    bool ret = false;

    WRT::CoreModuleSingleton::Instance().Init();

    WrtCorePtr wrt;
    /* FIXME : we need appid instead of uuid here */
    wrt = WRT::CoreModuleSingleton::Instance().getRunnableWidgetObject(info.helper_info.uuid);
    if (wrt) {
        LOGD("Loading URL : %s, %d", info.application_path.c_str(),
            wrt->PrepareView(info.application_path.c_str(), window, NULL));

#ifdef _WEARABLE
        wrt->CheckBeforeLaunch();
#endif

        WRT::UserDelegatesPtr cbs(new WRT::UserDelegates);
        using namespace std::placeholders;
        _loaded_callback = callback;
        cbs->loadFinishedCallback = std::bind(web_container_loaded, _1, _2);
        wrt->SetUserDelegates(cbs);

        _pCurrentWebView = wrt->GetCurrentWebview();

        _pWrt = wrt;
        _pWrt->Show();

        ret = true;
    }

    return ret;
}

bool web_container_destroy()
{
    bool ret = false;

    if (_pWrt) {
        ret = true;
        _pWrt->Hide();
    }
    _pWrt.reset();

    WRT::CoreModuleSingleton::Instance().Terminate();

    return ret;
}

bool web_container_resize(int width, int height)
{
    bool ret = false;

    if (_pCurrentWebView) {
        evas_object_resize(_pCurrentWebView, width, height);
        evas_object_move(_pCurrentWebView, 0, 0);
        evas_object_show(_pCurrentWebView);
        ret = true;
    }

    return ret;
}

bool web_container_javascript(const char *command, web_container_javascript_cb callback)
{
    bool ret = false;

    if (_pCurrentWebView) {
        if (ewk_view_script_execute(_pCurrentWebView, command, ewk_javascript_callback, (void*)callback) == EINA_FALSE) {
            LOGW("Failed while executing command : %s", command);
        }
        ret = true;
    }

    return ret;
}

/* API conformance checking */
static web_container_create_api create_api_test = web_container_create;
static web_container_destroy_api destroy_api_test = web_container_destroy;
static web_container_resize_api resize_api_test = web_container_resize;
static web_container_javascript_api javascript_api_test = web_container_javascript;
