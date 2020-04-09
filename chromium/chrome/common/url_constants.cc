// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/url_constants.h"

#include "build/branding_buildflags.h"

namespace chrome {

const char kAccessibilityLabelsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=image_descriptions";

const char kAutomaticSettingsResetLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_automatic_settings_reset";

const char kAdvancedProtectionDownloadLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/accounts/?p=ap_faq";

const char kBluetoothAdapterOffHelpURL[] =
    "https://support.9oo91e.qjz9zk/chrome?p=bluetooth";

const char kCastCloudServicesHelpURL[] =
    "https://support.9oo91e.qjz9zk/chromecast/?p=casting_cloud_services";

const char kCastNoDestinationFoundURL[] =
    "https://support.9oo91e.qjz9zk/chromecast/?p=no_cast_destination";

const char kChooserBluetoothOverviewURL[] =
    "https://support.9oo91e.qjz9zk/chrome?p=bluetooth";

const char kChooserUsbOverviewURL[] =
    "https://support.9oo91e.qjz9zk/chrome?p=webusb";

const char kChromeBetaForumURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=beta_forum";

// TODO: replace w/link after marketing provides it.
const char kChromeReleaseNotesURL[] =
    "https://www.9oo91e.qjz9zk/chromebook/whatsnew/embedded/";

const char kChromeHelpViaKeyboardURL[] =
#if defined(OS_CHROMEOS)
#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
    "chrome-extension://honijodknafkokifofgiaalefdiedpko/main.html";
#else
    "https://support.9oo91e.qjz9zk/chromebook/?p=help&ctx=keyboard";
#endif  // BUILDFLAG(GOOGLE_CHROME_BRANDING)
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=help&ctx=keyboard";
#endif  // defined(OS_CHROMEOS)

const char kChromeHelpViaMenuURL[] =
#if defined(OS_CHROMEOS)
#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
    "chrome-extension://honijodknafkokifofgiaalefdiedpko/main.html";
#else
    "https://support.9oo91e.qjz9zk/chromebook/?p=help&ctx=menu";
#endif  // BUILDFLAG(GOOGLE_CHROME_BRANDING)
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=help&ctx=menu";
#endif  // defined(OS_CHROMEOS)

const char kChromeHelpViaWebUIURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=help&ctx=settings";
#if defined(OS_CHROMEOS)
const char kChromeOsHelpViaWebUIURL[] =
#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
    "chrome-extension://honijodknafkokifofgiaalefdiedpko/main.html";
#else
    "https://support.9oo91e.qjz9zk/chromebook/?p=help&ctx=settings";
#endif  // BUILDFLAG(GOOGLE_CHROME_BRANDING)
#endif  // defined(OS_CHROMEOS)

const char kChromeNativeScheme[] = "chrome-native";

const char kChromeSearchLocalNtpHost[] = "local-ntp";
const char kChromeSearchLocalNtpUrl[] =
    "chrome-search://local-ntp/local-ntp.html";

const char kChromeSearchMostVisitedHost[] = "most-visited";
const char kChromeSearchMostVisitedUrl[] = "chrome-search://most-visited/";

const char kChromeSearchLocalNtpBackgroundUrl[] =
    "chrome-search://local-ntp/background.jpg";
const char kChromeSearchLocalNtpBackgroundFilename[] = "background.jpg";

const char kChromeSearchRemoteNtpHost[] = "remote-ntp";

const char kChromeSearchScheme[] = "chrome-search";

const char kChromiumProjectURL[] = "https://www.ch40m1um.qjz9zk/";

const char kCloudPrintLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_cloud_print";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_cloud_print";
#endif

const char kCloudPrintCertificateErrorLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook?p=cloudprint_error_troubleshoot";
#elif defined(OS_MACOSX)
    "https://support.9oo91e.qjz9zk/cloudprint?p=cloudprint_error_offline_mac";
#elif defined(OS_WIN)
        "https://support.9oo91e.qjz9zk/"
        "cloudprint?p=cloudprint_error_offline_windows";
#else
        "https://support.9oo91e.qjz9zk/"
        "cloudprint?p=cloudprint_error_offline_linux";
#endif

const char kContentSettingsExceptionsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_manage_exceptions";

const char kCrashReasonURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=e_awsnap";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=e_awsnap";
#endif

const char kCrashReasonFeedbackDisplayedURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=e_awsnap_rl";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=e_awsnap_rl";
#endif

const char kDoNotTrackLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_do_not_track";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_do_not_track";
#endif

const char kDownloadInterruptedLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_download_errors";

const char kDownloadScanningLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ib_download_blocked";

const char kExtensionControlledSettingLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_settings_api_extension";

const char kExtensionInvalidRequestURL[] = "chrome-extension://invalid/";

const char kGoogleAccountActivityControlsURL[] =
    "https://myaccount.9oo91e.qjz9zk/activitycontrols/search";

const char kGoogleAccountURL[] = "https://myaccount.9oo91e.qjz9zk";

const char kGoogleAccountChooserURL[] =
    "https://accounts.9oo91e.qjz9zk/AccountChooser";

const char kGooglePasswordManagerURL[] = "https://passwords.9oo91e.qjz9zk";

const char kLearnMoreReportingURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_usagestat";

const char kLegacySupervisedUserManagementDisplayURL[] =
    "www.ch40me.qjz9zk/manage";
const char kLegacySupervisedUserManagementURL[] =
    "https://www.ch40me.qjz9zk/manage";

const char kManagedUiLearnMoreUrl[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=is_chrome_managed";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=is_chrome_managed";
#endif

const char kMyActivityUrlInClearBrowsingData[] =
    "https://myactivity.9oo91e.qjz9zk/myactivity/?utm_source=chrome_cbd";

const char kOmniboxLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_omnibox";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_omnibox";
#endif

const char kPageInfoHelpCenterURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=ui_security_indicator";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_security_indicator";
#endif

const char kPasswordManagerLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_password";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_password";
#endif

const char kPaymentMethodsURL[] =
    "https://pay.9oo91e.qjz9zk/payments/"
    "home?utm_source=chrome&utm_medium=settings&utm_campaign=chrome-payment#"
    "paymentMethods";

const char kPaymentMethodsLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/answer/"
    "142893?visit_id=636857416902558798-696405304&p=settings_autofill&rd=1";
#else
    "https://support.9oo91e.qjz9zk/chrome/answer/"
    "142893?visit_id=636857416902558798-696405304&p=settings_autofill&rd=1";
#endif

const char kPrivacyLearnMoreURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_privacy";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_privacy";
#endif

const char kRemoveNonCWSExtensionURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_remove_non_cws_extensions";

const char kResetProfileSettingsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ui_reset_settings";

const char kSafetyTipHelpCenterURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=safety_tip";

const char kSettingsSearchHelpURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_search_help";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_search_help";
#endif

const char kSymantecSupportUrl[] =
    "https://support.9oo91e.qjz9zk/chrome?p=symantec#-202";

const char kSyncAndGoogleServicesLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome?p=syncgoogleservices";

const char kSyncEncryptionHelpURL[] =
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=settings_encryption";
#else
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_encryption";
#endif

const char kSyncErrorsHelpURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_sync_error";

const char kSyncGoogleDashboardURL[] =
    "https://www.9oo91e.qjz9zk/settings/chrome/sync/";

const char kSyncLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=settings_sign_in";

const char kUpgradeHelpCenterBaseURL[] =
    "https://support.9oo91e.qjz9zk/installer/?product="
    "{8A69D345-D564-463c-AFF1-A69D9E530F96}&error=";

#if defined(OS_ANDROID)
const char kAndroidAppScheme[] = "android-app";
#endif

#if defined(OS_CHROMEOS) || defined(OS_ANDROID)
const char kEnhancedPlaybackNotificationLearnMoreURL[] =
#endif
#if defined(OS_CHROMEOS)
    "https://support.9oo91e.qjz9zk/chromebook/?p=enhanced_playback";
#elif defined(OS_ANDROID)
// Keep in sync with chrome/android/java/strings/android_chrome_strings.grd
    "https://support.9oo91e.qjz9zk/chrome/?p=mobile_protected_content";
#endif

#if defined(OS_CHROMEOS)
const char kAccountManagerLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=google_accounts";

const char kAndroidAppsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=playapps";

const char kArcExternalStorageLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=open_files";

const char kArcPrivacyPolicyURLPath[] = "arc/privacy_policy";

const char kArcTermsURLPath[] = "arc/terms";

const char kChromeAccessibilityHelpURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/topic/6323347";

const char kChromeOSAssetHost[] = "chromeos-asset";
const char kChromeOSAssetPath[] = "/usr/share/chromeos-assets/";

const char kChromeOSCreditsPath[] =
    "/opt/google/chrome/resources/about_os_credits.html";

const char kChromePaletteHelpURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=stylus_help";

const char kCrosScheme[] = "cros";

const char kCupsPrintLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=chromebook_printing";

const char kCupsPrintPPDLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=printing_advancedconfigurations";

const char kEasyUnlockLearnMoreUrl[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=smart_lock";

const char kEULAPathFormat[] = "/usr/share/chromeos-assets/eula/%s/eula.html";

const char kArcTermsPathFormat[] = "arc_tos/%s/terms.html";

const char kArcPrivacyPolicyPathFormat[] = "arc_tos/%s/privacy_policy.pdf";

const char kEolNotificationURL[] = "https://www.9oo91e.qjz9zk/chromebook/older/";

const char kGoogleNameserversLearnMoreURL[] =
    "https://developers.9oo91e.qjz9zk/speed/public-dns";

const char kInstantTetheringLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=instant_tethering";

const char kKerberosAccountsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=kerberos_accounts";

const char kMultiDeviceLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=multi_device";

const char kAndroidMessagesLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=multi_device_messages";

const char kLanguageSettingsLearnMoreUrl[] =
    "https://support.9oo91e.qjz9zk/chromebook/answer/1059490";

const char kLearnMoreEnterpriseURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=managed";

const char kLinuxAppsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=chromebook_linuxapps";

const char kLinuxExportImportHelpURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=linux_backup_restore";

const char kLinuxCreditsPath[] =
    "/opt/google/chrome/resources/linux_credits.html";

const char kNaturalScrollHelpURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=simple_scrolling";

const char kOemEulaURLPath[] = "oem";

const char kOnlineEulaURLPath[] =
    "https://www.9oo91e.qjz9zk/intl/%s/chrome/eula_text.html";

const char kTPMFirmwareUpdateLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/?p=tpm_update";

const char kTimeZoneSettingsLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=chromebook_timezone&hl=%s";

const char kSmbSharesLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook?p=network_file_shares";
#endif  // defined(OS_CHROMEOS)

#if defined(OS_MACOSX)
const char kChromeEnterpriseSignInLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chromebook/answer/1331549";

const char kMac10_9_ObsoleteURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=unsupported_mac";
#endif

#if defined(OS_WIN)
const char kChromeCleanerLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=chrome_cleanup_tool";

const char kWindowsXPVistaDeprecationURL[] =
    "https://chrome.blogspot.com/2015/11/updates-to-chrome-platform-support.html";
#endif

#if BUILDFLAG(ENABLE_ONE_CLICK_SIGNIN)
const char kChromeSyncLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/answer/165139";
#endif  // BUILDFLAG(ENABLE_ONE_CLICK_SIGNIN)

#if BUILDFLAG(ENABLE_PLUGINS)
const char kBlockedPluginLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ib_blocked_plugin";

const char kOutdatedPluginLearnMoreURL[] =
    "https://support.9oo91e.qjz9zk/chrome/?p=ib_outdated_plugin";
#endif

}  // namespace chrome
