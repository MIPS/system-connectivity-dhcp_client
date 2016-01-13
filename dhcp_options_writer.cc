//
// Copyright (C) 2015 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "dhcp_client/dhcp_options_writer.h"

#include <netinet/in.h>

#include <string>
#include <utility>
#include <vector>

#include <base/logging.h>
#include <base/macros.h>

#include "dhcp_client/dhcp_options.h"

namespace {
base::LazyInstance<dhcp_client::DHCPOptionsWriter> g_dhcp_options_writer
    = LAZY_INSTANCE_INITIALIZER;
}  // namespace

namespace dhcp_client {

DHCPOptionsWriter* DHCPOptionsWriter::GetInstance() {
  return g_dhcp_options_writer.Pointer();
}

int DHCPOptionsWriter::WriteUInt8Option(uint8_t* buffer,
                                        uint8_t option_code,
                                        uint8_t value) {
  *buffer++ = option_code;
  *buffer++ = sizeof(uint8_t);
  *buffer++ = value;
  return sizeof(uint8_t) + 2;
}

int DHCPOptionsWriter::WriteUInt16Option(uint8_t* buffer,
                                         uint8_t option_code,
                                         uint16_t value) {
  *buffer++ = option_code;
  *buffer++ = sizeof(uint16_t);
  *reinterpret_cast<uint16_t*>(buffer) = htons(value);
  buffer += sizeof(uint16_t);
  return sizeof(uint16_t) + 2;
}

int DHCPOptionsWriter::WriteUInt32Option(uint8_t* buffer,
                                         uint8_t option_code,
                                         uint32_t value) {
  *buffer++ = option_code;
  *buffer++ = sizeof(uint32_t);
  *reinterpret_cast<uint32_t*>(buffer) = htonl(value);
  buffer += sizeof(uint32_t);
  return sizeof(uint32_t) + 2;
}

int DHCPOptionsWriter::WriteUInt8ListOption(uint8_t* buffer,
    uint8_t option_code,
    const std::vector<uint8_t>& value) {
  *buffer++ = option_code;
  *buffer++ = value.size() * sizeof(uint8_t);
  for (uint8_t element : value) {
    *buffer++ = element;
  }
  return value.size() * sizeof(uint8_t) + 2;
}

int DHCPOptionsWriter::WriteUInt16ListOption(uint8_t* buffer,
    uint8_t option_code,
    const std::vector<uint16_t>& value) {
  if (value.size() == 0) {
    LOG(ERROR) << "Faild to write option: " << static_cast<int>(option_code)
               << ", because value size cannot be 0";
    return -1;
  }
  *buffer++ = option_code;
  *buffer++ = value.size() * sizeof(uint16_t);
  for (uint16_t element : value) {
    *reinterpret_cast<uint16_t*>(buffer) = htons(element);
    buffer += sizeof(uint16_t);
  }
  return value.size() * sizeof(uint16_t) + 2;
}

int DHCPOptionsWriter::WriteUInt32ListOption(uint8_t* buffer,
    uint8_t option_code,
    const std::vector<uint32_t>& value) {
  if (value.size() == 0) {
    LOG(ERROR) << "Faild to write option: " << static_cast<int>(option_code)
               << ", because value size cannot be 0";
    return -1;
  }
  *buffer++ = option_code;
  *buffer++ = value.size() * sizeof(uint32_t);
  for (uint32_t element : value) {
    *reinterpret_cast<uint32_t*>(buffer) = htonl(element);
    buffer += sizeof(uint32_t);
  }
  return value.size() * sizeof(uint32_t) + 2;
}

int DHCPOptionsWriter::WriteUInt32PairListOption(uint8_t* buffer,
    uint8_t option_code,
    const std::vector<std::pair<uint32_t, uint32_t>>& value) {
  if (value.size() == 0) {
    LOG(ERROR) << "Faild to write option: " << static_cast<int>(option_code)
               << ", because value size cannot be 0";
    return -1;
  }
  *buffer++ = option_code;
  *buffer++ = value.size() * 2 * sizeof(uint32_t);
  for (auto element : value) {
    *reinterpret_cast<uint32_t*>(buffer)  = htonl(element.first);
    buffer += sizeof(uint32_t);
    *reinterpret_cast<uint32_t*>(buffer) = htonl(element.second);
    buffer += sizeof(uint32_t);
  }
  return value.size() * sizeof(uint32_t) * 2 + 2;
}

int DHCPOptionsWriter::WriteBoolOption(uint8_t* buffer,
                                       uint8_t option_code,
                                       const bool value) {
  *buffer++ = option_code;
  *buffer++ = sizeof(uint8_t);
  *buffer++ = value ? 1 : 0;
  return sizeof(uint8_t) + 2;
}

int DHCPOptionsWriter::WriteStringOption(uint8_t* buffer,
    uint8_t option_code,
    const std::string& value) {
  if (value.size() == 0) {
    LOG(ERROR) << "Faild to write option: " << static_cast<int>(option_code)
               << ", because value size cannot be 0";
    return -1;
  }
  *buffer++ = option_code;
  *buffer++ = value.size() * sizeof(uint8_t);
  for (unsigned int i = 0; i < value.size(); i++) {
    *buffer++ = static_cast<uint8_t>(value[i]);
  }
  return value.size() * sizeof(uint8_t) + 2;
}

void DHCPOptionsWriter::WriteEndTag(uint8_t* buffer) {
  *buffer = kDHCPOptionEnd;
}

}  // namespace dhcp_client
