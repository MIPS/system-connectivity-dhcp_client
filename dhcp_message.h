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

#ifndef DHCP_CLIENT_DHCP_MESSAGE_H_
#define DHCP_CLIENT_DHCP_MESSAGE_H_

#include <string>

#include <base/macros.h>

namespace dhcp_client {

class DHCPMessage {
 public:
  static const uint8_t kDHCPMessageTypeDiscover = 1;
  static const uint8_t kDHCPMessageTypeOffer = 2;
  static const uint8_t kDHCPMessageTypeRequest = 3;
  static const uint8_t kDHCPMessageTypeDecline = 4;
  static const uint8_t kDHCPMessageTypeAck = 5;
  static const uint8_t kDHCPMessageTypeNak = 6;
  static const uint8_t kDHCPMessageTypeRelease = 7;
  static const uint8_t kDHCPMessageTypeInform = 8;

  DHCPMessage();
  ~DHCPMessage();
  static bool InitFromBuffer(const unsigned char* buffer,
                             size_t length,
                             DHCPMessage* message);
  uint32_t GetTransactionID();
  std::string GetClientHardwareAddress();

 private:
  bool ParseDHCPOptions(const unsigned char* options,
                        size_t options_length);
  bool IsValid();

  // Message type.
  uint8_t opcode_;
  // Hardware address type.
  uint8_t hardware_address_type_;
  // Hardware address length.
  uint8_t hardware_address_length_;
  // Client sets to zero, optionally used by relay agents
  // when booting via a relay agent.
  uint8_t relay_hops_;
  // Transaction id.
  uint32_t transaction_id_;
  // Elapsed time from boot in seconds.
  uint16_t seconds_;
  // Broadcast flag
  uint16_t flags_;
  // Previously allocated client IP.
  uint32_t client_ip_address_;
  // Client IP address.
  uint32_t your_ip_address_;
  // IP address of next server to use in bootstrap;
  // returned in DHCPOFFER, DHCPACK by server.
  // It should be zero in client's messages.
  uint32_t next_server_ip_address_;
  // Relay agent IP address, used in booting via a relay agent.
  // It should be zero in client's messages.
  uint32_t agent_ip_address_;
  // Client's hardware address.
  std::string client_hardware_address_;
  // Server host name.
  std::string servername_;
  // Boot file name.
  std::string bootfile_;
  uint32_t cookie_;

  DISALLOW_COPY_AND_ASSIGN(DHCPMessage);
};

}  // namespace dhcp_client

#endif  // DHCP_CLIENT_DHCP_MESSAGE_H_
