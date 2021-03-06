#!/usr/bin/python
#
# Copyright 2019 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Creates a GN include file for building dav1d from source."""

__author__ = "dalecurtis@ch40m1um.qjz9zk (Dale Curtis)"

import datetime
import glob

COPYRIGHT = """# Copyright %d The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# NOTE: this file is autogenerated by dav1d/generate_sources.py - DO NOT EDIT.

""" % (
    datetime.datetime.now().year)

# .c files which don't need -DBIT_DEPTH specified for each compilation.
DAV1D_ENTRY_POINT_SOURCES = [
    "libdav1d/src/lib.c",
    "libdav1d/src/thread_task.c",
    "libdav1d/src/thread_task.h",
]


def WriteArray(fd, var_name, array, filter_list=[], last_entry=False):
  if len(array) == 0:
    fd.write("var_name = []\n")
    return

  fd.write("%s = [\n" % var_name)
  for item in sorted(array):
    if item not in filter_list:
      fd.write("  \"%s\",\n" % item)
  fd.write("]\n")
  if not last_entry:
    fd.write("\n")


def WriteGn(fd):
  fd.write(COPYRIGHT)
  WriteArray(fd, "x86_asm_sources", glob.glob("libdav1d/src/x86/*.asm"))
  WriteArray(fd, "x86_template_sources", glob.glob("libdav1d/src/x86/*_tmpl.c"))

  # TODO(dalecurtis): May want to exclude "util.S" here.
  WriteArray(fd, "arm32_asm_sources", glob.glob("libdav1d/src/arm/32/*.S"))
  WriteArray(fd, "arm64_asm_sources", glob.glob("libdav1d/src/arm/64/*.S"))
  WriteArray(fd, "arm_template_sources", glob.glob("libdav1d/src/arm/*_tmpl.c"))

  template_sources = glob.glob("libdav1d/src/*_tmpl.c")
  WriteArray(fd, "template_sources", template_sources)

  # Generate list of sources which need to be compiled multiple times with the
  # correct -DBIT_DEPTH=8|10 option specified each time.
  WriteArray(fd, "c_sources", glob.glob("libdav1d/src/*.[c|h]"),
             DAV1D_ENTRY_POINT_SOURCES + template_sources)

  WriteArray(
      fd, "entry_point_sources", DAV1D_ENTRY_POINT_SOURCES, last_entry=True)


def main():
  with open("dav1d_generated.gni", "w") as fd:
    WriteGn(fd)


if __name__ == "__main__":
  main()
