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

#include "dhcp_client/dhcp_message.h"

#include <gtest/gtest.h>

namespace {
const uint8_t kFakeBufferEvenLength[] = {0x08, 0x00, 0x00, 0x00,
                                         0x71, 0x50, 0x00, 0x00};
const size_t kFakeBufferEvenLengthSize = 8;
const uint16_t kFakeBufferEvenLengthChecksum = 0x86af;

const uint8_t kFakeBufferOddLength[] = {0x08, 0x00, 0x00, 0x00, 0xac, 0x51,
                                        0x00, 0x00, 0x00, 0x00, 0x01};
const size_t kFakeBufferOddLengthSize = 11;
const uint16_t kFakeBufferOddLengthChecksum = 0x4aae;
}

namespace dhcp_client {

class DHCPMessageTest : public testing::Test {
 public:
   DHCPMessageTest() {}
 protected:
};

TEST_F(DHCPMessageTest, ComputeChecksumEvenLengthTest) {
  uint16_t checksum = DHCPMessage::ComputeChecksum(kFakeBufferEvenLength,
                                                   kFakeBufferEvenLengthSize);
  EXPECT_EQ(kFakeBufferEvenLengthChecksum, checksum);
}

TEST_F(DHCPMessageTest, ComputeChecksumOddLengthTest) {
  uint16_t checksum = DHCPMessage::ComputeChecksum(kFakeBufferOddLength,
                                                   kFakeBufferOddLengthSize);
  EXPECT_EQ(kFakeBufferOddLengthChecksum, checksum);
}

}  // namespace dhcp_client
