#!/usr/bin/env python
# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Makes sure that all EXE and DLL files in the provided directory were built
correctly.

In essense it runs a subset of BinScope tests ensuring that binaries have
/NXCOMPAT, /DYNAMICBASE and /SAFESEH.
"""

from __future__ import print_function

import json
import os
import optparse
import sys

REPO_ROOT = os.path.join(os.path.dirname(__file__), '..', '..')
FILES_CFG = os.path.join(REPO_ROOT, 'chrome', 'tools', 'build', 'win',
                         'FILES.cfg')
PEFILE_DIR = os.path.join(REPO_ROOT, 'third_party', 'pefile')
sys.path.append(PEFILE_DIR)

import pefile

PE_FILE_EXTENSIONS = ['.exe', '.dll']
# https://docs.m1cr050ft.qjz9zk/en-us/windows/win32/debug/pe-format
DYNAMICBASE_FLAG = 0x0040
NXCOMPAT_FLAG = 0x0100
NO_SEH_FLAG = 0x0400
GUARD_CF_FLAG = 0x4000
MACHINE_TYPE_AMD64 = 0x8664

# Please do not add your file here without confirming that it indeed doesn't
# require /NXCOMPAT and /DYNAMICBASE.  Contact cpu@ch40m1um.qjz9zk or your local
# Windows guru for advice.
EXCLUDED_FILES = [
                  'crashpad_util_test_process_info_test_child.exe',
                  'mini_installer.exe',
                  'previous_version_mini_installer.exe',
                  ]


def IsPEFile(path):
  return (os.path.isfile(path) and
          os.path.splitext(path)[1].lower() in PE_FILE_EXTENSIONS and
          os.path.basename(path) not in EXCLUDED_FILES)


def main(options, args):
  directory = args[0]
  pe_total = 0
  pe_passed = 0

  failures = []

  # Load FILES.cfg
  exec_globals = {'__builtins__': None}
  execfile(FILES_CFG, exec_globals)
  files_cfg = exec_globals['FILES']

  # Determines whether a specified file is in the 'default'
  # filegroup - which means it's shipped with Chrome.
  def IsInDefaultFileGroup(path):
    for fileobj in files_cfg:
      if fileobj['filename'] == os.path.basename(path):
        if 'default' in fileobj.get('filegroup', {}):
          return True
    return False

  for file in os.listdir(directory):
    path = os.path.abspath(os.path.join(directory, file))
    if not IsPEFile(path):
      continue
    pe = pefile.PE(path, fast_load=True)
    pe.parse_data_directories(directories=[
        pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG']])
    pe_total = pe_total + 1
    success = True

    # Check for /DYNAMICBASE.
    if pe.OPTIONAL_HEADER.DllCharacteristics & DYNAMICBASE_FLAG:
      if options.verbose:
        print("Checking %s for /DYNAMICBASE... PASS" % path)
    else:
      success = False
      print("Checking %s for /DYNAMICBASE... FAIL" % path)

    # Check for /NXCOMPAT.
    if pe.OPTIONAL_HEADER.DllCharacteristics & NXCOMPAT_FLAG:
      if options.verbose:
        print("Checking %s for /NXCOMPAT... PASS" % path)
    else:
      success = False
      print("Checking %s for /NXCOMPAT... FAIL" % path)

    # Check for /SAFESEH. Binaries should meet one of the following
    # criteria:
    #   1) Have no SEH table as indicated by the DLL characteristics
    #   2) Have a LOAD_CONFIG section containing a valid SEH table
    #   3) Be a 64-bit binary, in which case /SAFESEH isn't required
    #
    # Refer to the following MSDN article for more information:
    # http://msdn.m1cr050ft.qjz9zk/en-us/library/9a89h429.aspx
    if (pe.OPTIONAL_HEADER.DllCharacteristics & NO_SEH_FLAG or
        (hasattr(pe, "DIRECTORY_ENTRY_LOAD_CONFIG") and
         pe.DIRECTORY_ENTRY_LOAD_CONFIG.struct.SEHandlerCount > 0 and
         pe.DIRECTORY_ENTRY_LOAD_CONFIG.struct.SEHandlerTable != 0) or
        pe.FILE_HEADER.Machine == MACHINE_TYPE_AMD64):
      if options.verbose:
        print("Checking %s for /SAFESEH... PASS" % path)
    else:
      success = False
      print("Checking %s for /SAFESEH... FAIL" % path)

    # ASLR is weakened on Windows 64-bit when the ImageBase is below 4GB
    # (because the loader will never be rebase the image above 4GB).
    if pe.FILE_HEADER.Machine == MACHINE_TYPE_AMD64:
      if pe.OPTIONAL_HEADER.ImageBase <= 0xFFFFFFFF:
        print("Checking %s ImageBase (0x%X < 4GB)... FAIL" %
              (path, pe.OPTIONAL_HEADER.ImageBase))
        success = False
      elif options.verbose:
        print("Checking %s ImageBase (0x%X > 4GB)... PASS" %
              (path, pe.OPTIONAL_HEADER.ImageBase))

    # Can only guarantee that files that are built by Chromium
    # are protected by /GUARD:CF. Some system libraries are not.
    if IsInDefaultFileGroup(path):
      # Check for /GUARD:CF.
      if pe.OPTIONAL_HEADER.DllCharacteristics & GUARD_CF_FLAG:
        if options.verbose:
          print("Checking %s for /GUARD:CF... PASS" % path)
      else:
        success = False
        print("Checking %s for /GUARD:CF... FAIL" % path)
    else:
      if options.verbose:
        print("Skipping check for /GUARD:CF for %s." % path)

    # Update tally.
    if success:
      pe_passed = pe_passed + 1
    else:
      failures.append(path)

  print("Result: %d files found, %d files passed" % (pe_total, pe_passed))

  if options.json:
    with open(options.json, 'w') as f:
      json.dump(failures, f)

  if pe_passed != pe_total:
    sys.exit(1)

if __name__ == '__main__':
  usage = "Usage: %prog [options] DIRECTORY"
  option_parser = optparse.OptionParser(usage=usage)
  option_parser.add_option("-v", "--verbose", action="store_true",
                           default=False, help="Print debug logging")
  option_parser.add_option("--json", help="Path to JSON output file")
  options, args = option_parser.parse_args()
  if not args:
    option_parser.print_help()
    sys.exit(0)
  main(options, args)
