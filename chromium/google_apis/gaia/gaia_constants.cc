// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Constants definitions

#include "google_apis/gaia/gaia_constants.h"

namespace GaiaConstants {

// Gaia uses this for accounting where login is coming from.
const char kChromeOSSource[] = "chromeos";
const char kChromeSource[] = "ChromiumBrowser";
const char kUnexpectedServiceResponse[] = "UnexpectedServiceResponse";

// Service name for Gaia.  Used to convert to cookie auth.
const char kGaiaService[] = "gaia";
// Service name for Picasa API. API is used to get user's image.
const char kPicasaService[] = "lh2";

// Service/scope names for sync.
const char kSyncService[] = "chromiumsync";

// Service name for remoting.
const char kRemotingService[] = "chromoting";

// OAuth scopes.
const char kOAuth1LoginScope[] = "https://www.9oo91e.qjz9zk/accounts/OAuthLogin";
const char kOAuthWrapBridgeUserInfoScope[] =
    "https://www.9oo91eapis.qjz9zk/auth/userinfo.email";

// Service/scope names for device management (cloud-based policy) server.
const char kDeviceManagementServiceOAuth[] =
    "https://www.9oo91eapis.qjz9zk/auth/chromeosdevicemanagement";

// OAuth2 scope for access to all Google APIs.
const char kAnyApiOAuth2Scope[] = "https://www.9oo91eapis.qjz9zk/auth/any-api";

// OAuth2 scope for access to Chrome sync APIs
const char kChromeSyncOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/chromesync";
// OAuth2 scope for access to the Chrome Sync APIs for managed profiles.
const char kChromeSyncSupervisedOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/chromesync_playpen";

// OAuth2 scope for parental consent logging for secondary account addition.
const char kKidManagementPrivilegedOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/kid.management.privileged";

// OAuth2 scope for access to Google Family Link Supervision Setup.
const char kKidsSupervisionSetupChildOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/kids.supervision.setup.child";

// OAuth2 scope for access to Google Talk APIs (XMPP).
const char kGoogleTalkOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/googletalk";

const char kGoogleUserInfoEmail[] =
    "https://www.9oo91eapis.qjz9zk/auth/userinfo.email";
const char kGoogleUserInfoProfile[] =
    "https://www.9oo91eapis.qjz9zk/auth/userinfo.profile";

// OAuth scope for access to the people API (read-only).
const char kPeopleApiReadOnlyOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/peopleapi.readonly";

// OAuth scope for access to the Reauth flow.
const char kAccountsReauthOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/accounts.reauth";

// OAuth scope for access to audit recording (ARI).
const char kAuditRecordingOAuth2Scope[] =
    "https://www.9oo91eapis.qjz9zk/auth/auditrecording-pa";

// OAuth scope for access to clear cut logs.
const char kClearCutOAuth2Scope[] = "https://www.9oo91eapis.qjz9zk/auth/cclog";

// Used to mint uber auth tokens when needed.
const char kGaiaSid[] = "sid";
const char kGaiaLsid[] = "lsid";
const char kGaiaOAuthToken[] = "oauthToken";
const char kGaiaOAuthSecret[] = "oauthSecret";
const char kGaiaOAuthDuration[] = "3600";
const char kGaiaOAuth2LoginRefreshToken[] = "oauth2LoginRefreshToken";

// Used to construct a channel ID for push messaging.
const char kObfuscatedGaiaId[] = "obfuscatedGaiaId";

// Used to build ClientOAuth requests.  These are the names of keys used when
// building base::DictionaryValue that represent the json data that makes up
// the ClientOAuth endpoint protocol.  The comment above each constant explains
// what value is associated with that key.

// Canonical email of the account to sign in.
const char kClientOAuthEmailKey[] = "email";

// Used as an Invalid refresh token.
const char kInvalidRefreshToken[] = "invalid_refresh_token";
}  // namespace GaiaConstants
