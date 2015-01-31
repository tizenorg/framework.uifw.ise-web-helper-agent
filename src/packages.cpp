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

#include <stdio.h>
#include <string.h>
#include <dlog.h>
#include <pkgmgr-info.h>

#include "packages.h"

#define ISEUUID "1373647e-5d53-4019-b810-ed068e44873c"
#define ISENAME "Web Keyboard Container"

CPackages* CPackages::m_instance = NULL; /* For singleton */
static std::vector<HelperInfoEx> helper_infos;

int app_list_cb(pkgmgrinfo_appinfo_h handle, void *user_data)
{
    int ret;
    char *app_id = NULL;
    char *pkg_id = NULL, *pkg_label = NULL, *pkg_type = NULL, *pkg_root_path = NULL, *pkg_icon_path = NULL;
    pkgmgrinfo_appinfo_h appinfo_handle = NULL;
    pkgmgrinfo_pkginfo_h pkginfo_handle = NULL;
    HelperInfoEx helper_info_ex;

    /* Get appid */
    ret = pkgmgrinfo_appinfo_get_appid(handle, &app_id);
    if (ret != PMINFO_R_OK)
        return 0;

    ret = pkgmgrinfo_appinfo_get_appinfo(app_id, &appinfo_handle);
    if (ret != PMINFO_R_OK)
        return 0;

    /* Get package id */
    ret = pkgmgrinfo_appinfo_get_pkgname(appinfo_handle, &pkg_id);
    if (ret != PMINFO_R_OK) {
        pkgmgrinfo_appinfo_destroy_appinfo(appinfo_handle);
        return 0;
    }

    /* Get package info handle */
    ret = pkgmgrinfo_pkginfo_get_pkginfo(pkg_id, &pkginfo_handle);
    if (ret != PMINFO_R_OK) {
        pkgmgrinfo_appinfo_destroy_appinfo(appinfo_handle);
        return 0;
    }

    /* Get the type of package */
    ret = pkgmgrinfo_pkginfo_get_type(pkginfo_handle, &pkg_type);
    if (ret != PMINFO_R_OK) {
        pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo_handle);
        pkgmgrinfo_appinfo_destroy_appinfo(appinfo_handle);
        return 0;
    }

    if (pkg_type && !strncmp(pkg_type, "wgt", 3)) {
        // FIXME : need to get UUID
        helper_info_ex.helper_info.uuid = scim::String(app_id);

        /* Get the label of package */
        if (pkgmgrinfo_pkginfo_get_label(pkginfo_handle, &pkg_label) == PMINFO_R_OK)
            helper_info_ex.helper_info.name = scim::String(pkg_label);

        /* Get the icon path of package */
        if (pkgmgrinfo_pkginfo_get_icon(pkginfo_handle, &pkg_icon_path) == PMINFO_R_OK)
            helper_info_ex.helper_info.icon = scim::String(pkg_icon_path);

        // FIXME : fill the helper option from package or app information
        helper_info_ex.helper_info.option = scim::SCIM_HELPER_STAND_ALONE | scim::SCIM_HELPER_NEED_SCREEN_INFO |
            scim::SCIM_HELPER_NEED_SPOT_LOCATION_INFO | scim::SCIM_HELPER_AUTO_RESTART;

        // FIXME : fill the input language from package or app information
        helper_info_ex.supported_languages = scim::String("en_US");

        /* Get the root path of package */
        if (pkgmgrinfo_pkginfo_get_root_path(pkginfo_handle, &pkg_root_path) == PMINFO_R_OK)
            helper_info_ex.application_path = scim::String("file://") + scim::String(pkg_root_path) + scim::String("/res/wgt/index.html");

        helper_infos.push_back(helper_info_ex);
    }

    pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo_handle);
    pkgmgrinfo_appinfo_destroy_appinfo(appinfo_handle);

    return 0;
}

CPackages* CPackages::get_instance()
{
    if (!m_instance) {
        m_instance = new CPackages();
    }
    return (CPackages*)m_instance;
}

void
CPackages::init()
{
    /* Collect web keyboards' information here */
    int ret = 0;
    pkgmgrinfo_appinfo_filter_h handle;
    ret = pkgmgrinfo_appinfo_filter_create(&handle);
    if (ret != PMINFO_R_OK)
        return;

    ret = pkgmgrinfo_appinfo_filter_add_string(handle, PMINFO_APPINFO_PROP_APP_CATEGORY, "http://tizen.org/category/ime");
    if (ret == PMINFO_R_OK) {
        pkgmgrinfo_appinfo_filter_foreach_appinfo(handle, app_list_cb, NULL);
    }

    pkgmgrinfo_appinfo_filter_destroy(handle);
}

void
CPackages::fini()
{

}

int
CPackages::get_number_of_packages()
{
    return helper_infos.size();
}

HelperInfoEx
CPackages::get_package_information_by_index(int index)
{
    HelperInfoEx ret;
    if (index >= 0 && index < (int)helper_infos.size()) {
        ret = helper_infos.at(index);
    }
    return ret;
}

HelperInfoEx
CPackages::get_package_information_by_uuid(scim::String uuid)
{
    HelperInfoEx ret;
    for (int loop = 0;loop < (int)helper_infos.size();loop++) {
        if (helper_infos.at(loop).helper_info.uuid.compare(uuid) == 0) {
            ret = helper_infos.at(loop);
        }
    }
    return ret;
}
