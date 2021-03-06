// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_NATIVE_FILE_SYSTEM_PERMISSION_CONTEXT_H_
#define CONTENT_PUBLIC_BROWSER_NATIVE_FILE_SYSTEM_PERMISSION_CONTEXT_H_

#include "base/files/file_path.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/native_file_system_permission_grant.h"
#include "content/public/browser/native_file_system_write_item.h"
#include "url/origin.h"

namespace content {

// Entry point to an embedder implemented permission context for the Native File
// System API. Instances of this class can be retrieved via a BrowserContext.
// All these methods must always be called on the UI thread.
class NativeFileSystemPermissionContext {
 public:
  // The type of action a user took that resulted in needing a permission grant
  // for a particular path. This is used to signal to the permission context if
  // the path was the result of a "save" operation, which an implementation can
  // use to automatically grant write access to the path.
  enum class UserAction {
    // The path for which a permission grant is requested was the result of a
    // "open" dialog. As such, only read access to files should be automatically
    // granted, but read access to directories as well as write access to files
    // or directories should not be granted without needing to request it.
    kOpen,
    // The path for which a permission grant is requested was the result of a
    // "save" dialog, and as such it could make sense to return a grant that
    // immediately allows write access without needing to request it.
    kSave,
    // The path for which a permission grant is requested was the result of
    // loading a handle from storage. As such the grant should not start out
    // as granted, even for read access.
    kLoadFromStorage,
    // The path for which a permission grant is requested was the result of a
    // drag&drop operation. Read access should start out granted, but write
    // access will require a prompt.
    kDragAndDrop,
  };

  // This enum helps distinguish between file or directory Native File System
  // handles.
  enum class HandleType { kFile, kDirectory };

  // Returns the read permission grant to use for a particular path.
  virtual scoped_refptr<NativeFileSystemPermissionGrant> GetReadPermissionGrant(
      const url::Origin& origin,
      const base::FilePath& path,
      HandleType handle_type,
      UserAction user_action) = 0;

  // Returns the permission grant to use for a particular path. This could be a
  // grant that applies to more than just the path passed in, for example if a
  // user has already granted write access to a directory, this method could
  // return that existing grant when figuring the grant to use for a file in
  // that directory.
  virtual scoped_refptr<NativeFileSystemPermissionGrant>
  GetWritePermissionGrant(const url::Origin& origin,
                          const base::FilePath& path,
                          HandleType handle_type,
                          UserAction user_action) = 0;

  // These values are persisted to logs. Entries should not be renumbered and
  // numeric values should never be reused.
  enum class SensitiveDirectoryResult {
    kAllowed = 0,   // Access to directory is okay.
    kTryAgain = 1,  // User should pick a different directory.
    kAbort = 2,     // Abandon entirely, as if picking was cancelled.
    kMaxValue = kAbort
  };
  // Checks if access to the given |paths| should be allowed or blocked. This is
  // used to implement blocks for certain sensitive directories such as the
  // "Windows" system directory, as well as the root of the "home" directory.
  // Calls |callback| with the result of the check, after potentially showing
  // some UI to the user if the path should not be accessed.
  virtual void ConfirmSensitiveDirectoryAccess(
      const url::Origin& origin,
      const std::vector<base::FilePath>& paths,
      HandleType handle_type,
      GlobalFrameRoutingId frame_id,
      base::OnceCallback<void(SensitiveDirectoryResult)> callback) = 0;

  enum class AfterWriteCheckResult { kAllow, kBlock };

  // Returns whether the give |origin| already allows read permission, or it is
  // possible to request one. This is used to block file dialogs from being
  // shown if permission won't be granted anyway.
  virtual bool CanObtainReadPermission(const url::Origin& origin) = 0;

  // Returns whether the give |origin| already allows write permission, or it is
  // possible to request one. This is used to block save file dialogs from being
  // shown if there is no need to ask for it.
  virtual bool CanObtainWritePermission(const url::Origin& origin) = 0;

 protected:
  virtual ~NativeFileSystemPermissionContext() = default;
};

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_NATIVE_FILE_SYSTEM_PERMISSION_CONTEXT_H_
