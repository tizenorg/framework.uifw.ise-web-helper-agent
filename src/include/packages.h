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

#ifndef _PACKAGES_H_
#define _PACKAGES_H_

#define Uses_SCIM_UTILITY
#define Uses_SCIM_OBJECT
#define Uses_SCIM_POINTER
#define Uses_SCIM_EVENT
#define Uses_SCIM_HELPER
#define Uses_SCIM_CONFIG_BASE

#include <stdio.h>
#include <string>
#include <vector>
#include <scim.h>

typedef struct {
    scim::HelperInfo helper_info;
    scim::String supported_languages;
    std::string application_path;
} HelperInfoEx;

class CPackages {
private:
    static CPackages* m_instance; /* For singleton */
    CPackages() {};
public:
    virtual ~CPackages() {};

    static CPackages* get_instance();

    void init();
    void fini();

    int get_number_of_packages();

    HelperInfoEx get_package_information_by_index(int index);
    HelperInfoEx get_package_information_by_uuid(scim::String uuid);

private:
};

#endif // _PACKAGES_H_
