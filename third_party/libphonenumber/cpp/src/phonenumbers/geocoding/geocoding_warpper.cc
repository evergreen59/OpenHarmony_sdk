// Copyright (C) 2012 The Libphonenumber Authors

#include <algorithm>
#include "geocoding_warpper.h"
#include "phonenumbers/geocoding/phonenumber_offline_geocoder.h"
#include "phonenumbers/phonenumberutil.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unicode/unistr.h>  // NOLINT(build/include_order)
#include <unicode/locid.h>

using icu::UnicodeString;
using i18n::phonenumbers::PhoneNumber;
using i18n::phonenumbers::PhoneNumberUtil;
using i18n::phonenumbers::PhoneNumberOfflineGeocoder;
using icu::Locale;

extern "C" void exposeLocationName(const char* pNumber, const char* locale, char* res) {
    if(offlineGeocoder == NULL) {
        offlineGeocoder = new PhoneNumberOfflineGeocoder();
    }
    if (util == NULL) {
        util = PhoneNumberUtil::GetInstance();
    }
    icu::Locale uLocale = icu::Locale::createFromName(locale);
    i18n::phonenumbers::PhoneNumber phoneNumber;
    std::string number = pNumber;
    PhoneNumberUtil::ErrorType type = util->Parse(number, uLocale.getCountry(), &phoneNumber);
    if (type != PhoneNumberUtil::ErrorType::NO_PARSING_ERROR) {
        std::string empty = "";
        std::strcpy(res, empty.c_str());
    }
    std::string result = offlineGeocoder->GetDescriptionForNumber(phoneNumber, uLocale);
    std::strcpy(res, result.c_str());
}

