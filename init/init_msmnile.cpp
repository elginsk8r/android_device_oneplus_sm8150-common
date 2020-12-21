/*
   Copyright (C) 2017-2018 The Android Open Source Project

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <android-base/logging.h>
#include <android-base/properties.h>

#include <string>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using ::android::init::property_set;

constexpr const char* RO_PROP_SOURCES[] = {
        nullptr, "product.", "product_services.", "odm.", "vendor.", "system.", "bootimage.",
};

constexpr const char* BUILD_ID = "QKQ1.190716.003";

constexpr const char* BUILD_INC_VERSION[] = {
        "2011052232",
        "2011052232",
        "2002141334",
        "2010092151",
        "2011052234",
        "2011052221",
        "2009150235",
};

constexpr const char* BUILD_DEVICE[] = {
        "OnePlus7",
        "OnePlus7Pro",
        "OnePlus7ProNR",
        "OnePlus7ProTMO",
        "OnePlus7T",
        "OnePlus7TPro",
        "OnePlus7TProNR",
};

constexpr const char* BUILD_VARIANT[] = {
        "OnePlus7",
        "OnePlus7Pro",
        "OnePlus7ProNR_EEA",
        "OnePlus7ProTMO",
        "OnePlus7T",
        "OnePlus7TPro",
        "OnePlus7TProNR",
};

void property_override(char const prop[], char const value[]) {
    prop_info* pi = (prop_info*)__system_property_find(prop);
    if (pi) {
        __system_property_update(pi, value, strlen(value));
    }
}

void load_props(const char* model, int id) {
    const auto ro_prop_override = [](const char* source, const char* prop, const char* value,
                                     bool product) {
        std::string prop_name = "ro.";

        if (product) prop_name += "product.";
        if (source != nullptr) prop_name += source;
        if (!product) prop_name += "build.";
        prop_name += prop;

        property_override(prop_name.c_str(), value);
    };

    char description[PROP_VALUE_MAX+1];
    char fingerprint[PROP_VALUE_MAX+1];

    snprintf(description, PROP_VALUE_MAX, "%s-user 10 %s %s release-keys",
        BUILD_DEVICE[id], BUILD_ID, BUILD_INC_VERSION[id]);

    snprintf(fingerprint, PROP_VALUE_MAX, "OnePlus/%s/%s:10/%s/%s:user/release-keys",
        BUILD_VARIANT[id], BUILD_DEVICE[id], BUILD_ID, BUILD_INC_VERSION[id]);

    for (const auto& source : RO_PROP_SOURCES) {
        ro_prop_override(source, "device", model, true);
        ro_prop_override(source, "model", model, true);
        ro_prop_override(source, "fingerprint", fingerprint,
                         false);
    }
    ro_prop_override(nullptr, "description", description, false);

    // ro.build.fingerprint property has not been set
    property_set("ro.build.fingerprint", fingerprint);
}

void vendor_load_properties() {
    int project_name = stoi(android::base::GetProperty("ro.boot.project_name", ""));
    if (project_name == 18827) {
        /* 5G Europe */
        load_props("OnePlus7Pro", 2);
    } else if (project_name == 18831) {
        /* T-Mobile */
        load_props("OnePlus7Pro", 3);
    } else if (project_name == 19861) {
        /* T-Mobile 5G McLaren */
        load_props("OnePlus7TPro", 6);
    } else {
        /* Global Variants */
        if (project_name == 18865) {
            load_props("OnePlus7T", 4);
        } else if (project_name > 18865) {
            load_props("OnePlus7TPro", 5);
        } else if (project_name != 18857) {
            load_props("OnePlus7Pro", 1);
        } else {
            load_props("OnePlus7", 0);
        }
    }
}
