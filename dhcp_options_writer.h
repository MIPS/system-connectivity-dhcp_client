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

#ifndef DHCP_CLIENT_OPTION_WRITER_H_
#define DHCP_CLIENT_OPTION_WRITER_H_

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <base/lazy_instance.h>

namespace dhcp_client {

class DHCPOptionsWriter {
 public:
  ~DHCPOptionsWriter() {}
  static DHCPOptionsWriter* GetInstance();
  int WriteUInt8Option(uint8_t* buffer, uint8_t option_code, uint8_t value);
  int WriteUInt16Option(uint8_t* buffer,
                        uint8_t option_code,
                        uint16_t value);
  int WriteUInt32Option(uint8_t* buffer,
                        uint8_t option_code,
                        uint32_t value);
  int WriteUInt8ListOption(uint8_t* buffer,
                           uint8_t option_code,
                           const std::vector<uint8_t>& value);
  int WriteUInt16ListOption(uint8_t* buffer,
                            uint8_t option_code,
                            const std::vector<uint16_t>& value);
  int WriteUInt32ListOption(uint8_t* buffer,
                            uint8_t option_code,
                            const std::vector<uint32_t>& value);
  int WriteUInt32PairListOption(uint8_t* buffer,
      uint8_t option_code,
      const std::vector<std::pair<uint32_t, uint32_t>>& value);
  int WriteBoolOption(uint8_t* buffer,
                      uint8_t option_code,
                      const bool value);
  int WriteStringOption(uint8_t* buffer,
                        uint8_t option_code,
                        const std::string& value);
  void WriteEndTag(uint8_t* buffer);

 protected:
  DHCPOptionsWriter() {}

 private:
  friend struct base::DefaultLazyInstanceTraits<DHCPOptionsWriter>;
};

}  // namespace dhcp_client

#endif  // DHCP_CLIENT_OPTION_WRITER_H_
