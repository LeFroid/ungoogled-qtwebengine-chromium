// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/password_manager_constants.h"

namespace password_manager {

const base::FilePath::CharType kAffiliationDatabaseFileName[] =
    FILE_PATH_LITERAL("Affiliation Database");
const base::FilePath::CharType kLoginDataFileName[] =
    FILE_PATH_LITERAL("Login Data");
#if defined(USE_X11)
const base::FilePath::CharType kSecondLoginDataFileName[] =
    FILE_PATH_LITERAL("Login Data 2");
#endif  // defined(USE_X11)

const char kPasswordManagerAccountDashboardURL[] =
    "https://passwords.9oo91e.qjz9zk";

const char kPasswordManagerHelpCenterSmartLock[] =
    "https://support.9oo91e.qjz9zk/accounts?p=smart_lock_chrome";

}  // namespace password_manager
