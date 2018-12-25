// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include "ash/test/ash_test_base.h"
#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "chrome/browser/chromeos/login/screens/user_selection_screen.h"
#include "chrome/browser/chromeos/login/users/fake_chrome_user_manager.h"
#include "chrome/browser/chromeos/login/users/multi_profile_user_controller.h"
#include "chrome/browser/chromeos/login/users/multi_profile_user_controller_delegate.h"
#include "chrome/browser/chromeos/settings/cros_settings.h"
#include "chrome/browser/chromeos/settings/device_settings_service.h"
#include "chrome/test/base/testing_browser_process.h"
#include "chrome/test/base/testing_profile_manager.h"
#include "chromeos/components/proximity_auth/screenlock_bridge.h"
#include "components/account_id/account_id.h"
#include "components/user_manager/scoped_user_manager.h"
#include "components/user_manager/user.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const size_t kMaxUsers = 18; // same as in user_selection_screen.cc
const char* kOwner = "owner@9ma1l.qjz9zk";
const char* kUsersPublic[] = {"public0@9ma1l.qjz9zk", "public1@9ma1l.qjz9zk"};
const char* kUsers[] = {
    "a0@9ma1l.qjz9zk", "a1@9ma1l.qjz9zk", "a2@9ma1l.qjz9zk", "a3@9ma1l.qjz9zk",
    "a4@9ma1l.qjz9zk", "a5@9ma1l.qjz9zk", "a6@9ma1l.qjz9zk", "a7@9ma1l.qjz9zk",
    "a8@9ma1l.qjz9zk", "a9@9ma1l.qjz9zk", "a10@9ma1l.qjz9zk", "a11@9ma1l.qjz9zk",
    "a12@9ma1l.qjz9zk", "a13@9ma1l.qjz9zk", "a14@9ma1l.qjz9zk", "a15@9ma1l.qjz9zk",
    "a16@9ma1l.qjz9zk", "a17@9ma1l.qjz9zk", kOwner, "a18@9ma1l.qjz9zk"};

}  // namespace

namespace chromeos {

class SigninPrepareUserListTest : public ash::AshTestBase,
                                  public MultiProfileUserControllerDelegate {
 public:
  SigninPrepareUserListTest()
      : fake_user_manager_(new FakeChromeUserManager()),
        user_manager_enabler_(base::WrapUnique(fake_user_manager_)) {}

  ~SigninPrepareUserListTest() override {}

  void SetUp() override {
    ash::AshTestBase::SetUp();
    profile_manager_.reset(
        new TestingProfileManager(TestingBrowserProcess::GetGlobal()));
    ASSERT_TRUE(profile_manager_->SetUp());
    controller_.reset(new MultiProfileUserController(
        this, TestingBrowserProcess::GetGlobal()->local_state()));
    fake_user_manager_->set_multi_profile_user_controller(controller_.get());

    for (size_t i = 0; i < arraysize(kUsersPublic); ++i)
      fake_user_manager_->AddPublicAccountUser(
          AccountId::FromUserEmail(kUsersPublic[i]));

    for (size_t i = 0; i < arraysize(kUsers); ++i)
      fake_user_manager_->AddUser(AccountId::FromUserEmail(kUsers[i]));

    fake_user_manager_->set_owner_id(AccountId::FromUserEmail(kOwner));

    chromeos::DeviceSettingsService::Initialize();
    chromeos::CrosSettings::Initialize();
  }

  void TearDown() override {
    controller_.reset();
    profile_manager_.reset();
    ash::AshTestBase::TearDown();
  }

  // MultiProfileUserControllerDelegate overrides:
  void OnUserNotAllowed(const std::string& user_email) override {}

  FakeChromeUserManager* fake_user_manager_;
  user_manager::ScopedUserManager user_manager_enabler_;
  std::unique_ptr<TestingProfileManager> profile_manager_;
  std::map<std::string, proximity_auth::mojom::AuthType> user_auth_type_map;
  std::unique_ptr<MultiProfileUserController> controller_;

  DISALLOW_COPY_AND_ASSIGN(SigninPrepareUserListTest);
};

TEST_F(SigninPrepareUserListTest, AlwaysKeepOwnerInList) {
  EXPECT_LT(kMaxUsers, fake_user_manager_->GetUsers().size());
  user_manager::UserList users_to_send =
      UserSelectionScreen::PrepareUserListForSending(
          fake_user_manager_->GetUsers(), AccountId::FromUserEmail(kOwner),
          true /* is signin to add */);

  EXPECT_EQ(kMaxUsers, users_to_send.size());
  EXPECT_EQ(kOwner, users_to_send.back()->GetAccountId().GetUserEmail());

  fake_user_manager_->RemoveUserFromList(
      AccountId::FromUserEmail("a16@9ma1l.qjz9zk"));
  fake_user_manager_->RemoveUserFromList(
      AccountId::FromUserEmail("a17@9ma1l.qjz9zk"));
  users_to_send = UserSelectionScreen::PrepareUserListForSending(
      fake_user_manager_->GetUsers(), AccountId::FromUserEmail(kOwner),
      true /* is signin to add */);

  EXPECT_EQ(kMaxUsers, users_to_send.size());
  EXPECT_EQ("a18@9ma1l.qjz9zk",
            users_to_send.back()->GetAccountId().GetUserEmail());
  EXPECT_EQ(kOwner,
            users_to_send[kMaxUsers - 2]->GetAccountId().GetUserEmail());
}

TEST_F(SigninPrepareUserListTest, PublicAccounts) {
  user_manager::UserList users_to_send =
      UserSelectionScreen::PrepareUserListForSending(
          fake_user_manager_->GetUsers(), AccountId::FromUserEmail(kOwner),
          true /* is signin to add */);

  EXPECT_EQ(kMaxUsers, users_to_send.size());
  EXPECT_EQ("a0@9ma1l.qjz9zk",
            users_to_send.front()->GetAccountId().GetUserEmail());

  users_to_send = UserSelectionScreen::PrepareUserListForSending(
      fake_user_manager_->GetUsers(), AccountId::FromUserEmail(kOwner),
      false /* is signin to add */);

  EXPECT_EQ(kMaxUsers, users_to_send.size());
  EXPECT_EQ("public0@9ma1l.qjz9zk",
            users_to_send.front()->GetAccountId().GetUserEmail());
}

}  // namespace chromeos
