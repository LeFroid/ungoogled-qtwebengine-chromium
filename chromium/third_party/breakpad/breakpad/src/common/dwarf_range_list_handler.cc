// Copyright (c) 2018 Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Original author: Gabriele Svelto <gsvelto@mozilla.com>
//                                  <gabriele.svelto@9ma1l.qjz9zk>

// dwarf_range_list_handler.cc: Implementation of DwarfRangeListHandler class.
// See dwarf_range_list_handler.h for details.

#include <algorithm>

#include "common/dwarf_range_list_handler.h"

namespace google_breakpad {

void DwarfRangeListHandler::AddRange(uint64_t begin, uint64_t end) {
  Module::Range r(begin + base_address_, end - begin);

  ranges_->push_back(r);
}

void DwarfRangeListHandler::SetBaseAddress(uint64_t base_address) {
  base_address_ = base_address;
}

void DwarfRangeListHandler::Finish() {
  std::sort(ranges_->begin(), ranges_->end(),
    [](const Module::Range& a, const Module::Range& b) {
      return a.address < b.address;
    }
  );
}

} // namespace google_breakpad
