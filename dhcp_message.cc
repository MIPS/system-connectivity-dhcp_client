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

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>

#include <memory>
#include <string>

#include <base/logging.h>

namespace dhcp_client {

namespace {
const int kClientHardwareAddressLength = 16;
const int kServerNameLength = 64;
const int kBootFileLength = 128;
const int kDHCPOptionLength = 312;
const uint32_t kMagicCookie = 0x63825363;
const size_t kDHCPMessageMaxLength = 548;
const size_t kDHCPMessageMinLength = 236;
const uint8_t kDHCPMessageBootRequest = 1;
const uint8_t kDHCPMessageBootReply = 2;

// Follow the naming in rfc2131 for this struct.
struct __attribute__((__packed__)) RawDHCPMessage {
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t xid;
  uint16_t secs;
  uint16_t flags;
  uint32_t ciaddr;
  uint32_t yiaddr;
  uint32_t siaddr;
  uint32_t giaddr;
  uint8_t chaddr[kClientHardwareAddressLength];
  uint8_t sname[kServerNameLength];
  uint8_t file[kBootFileLength];
  uint32_t cookie;
  uint8_t options[kDHCPOptionLength];
};
}  // namespace

DHCPMessage::DHCPMessage() {}
DHCPMessage::~DHCPMessage() {}

bool DHCPMessage::InitFromBuffer(const unsigned char* buffer,
                                 size_t length,
                                 DHCPMessage* message) {
  if (buffer == NULL) {
    LOG(WARNING) << "Invalid buffer address";
    return false;
  }
  if (length < kDHCPMessageMinLength || length > kDHCPMessageMaxLength) {
    LOG(WARNING) << "Invalid DHCP message length";
    return false;
  }
  const RawDHCPMessage* raw_message
      = reinterpret_cast<const RawDHCPMessage*>(buffer);
  size_t options_length = reinterpret_cast<const unsigned char*>(raw_message) +
      length - reinterpret_cast<const unsigned char*>(raw_message->options) + 1;
  message->opcode_ = raw_message->op;
  message->hardware_address_type_ = raw_message->htype;
  message->hardware_address_length_ = raw_message->hlen;
  if (message->hardware_address_length_ > kClientHardwareAddressLength) {
    LOG(WARNING) << "Invalid hardware address length";
  }
  message->relay_hops_ = raw_message->hops;
  message->transaction_id_ = ntohl(raw_message->xid);
  message->seconds_ = ntohs(raw_message->secs);
  message->flags_ = ntohs(raw_message->flags);
  message->client_ip_address_ = ntohl(raw_message->ciaddr);
  message->your_ip_address_ = ntohl(raw_message->yiaddr);
  message->next_server_ip_address_ = ntohl(raw_message->siaddr);
  message->agent_ip_address_ = ntohl(raw_message->giaddr);
  message->cookie_ = ntohl(raw_message->cookie);
  message->client_hardware_address_
      .assign(reinterpret_cast<const char*>(raw_message->chaddr),
              message->hardware_address_length_);
  message->servername_.assign(reinterpret_cast<const char*>(raw_message->sname),
                              kServerNameLength);
  message->bootfile_.assign(reinterpret_cast<const char*>(raw_message->file),
                            kBootFileLength);
  // Validate the DHCP Message
  if (!message->IsValid()) {
    return false;
  }
  if (!message->ParseDHCPOptions(raw_message->options, options_length)) {
    LOG(WARNING) << "Failed to parse DHCP options";
    return false;
  }
  // TODO(nywang): A DHCP message must have option 53: DHCP Message Type.
  return true;
}

bool DHCPMessage::ParseDHCPOptions(const unsigned char* options,
                                   size_t options_length) {
  // TODO(nywang): Read DHCP options.
  return true;
}

uint32_t DHCPMessage::GetTransactionID() {
  return transaction_id_;
}

std::string DHCPMessage::GetClientHardwareAddress() {
  return client_hardware_address_;
}

bool DHCPMessage::IsValid() {
  if (opcode_ != kDHCPMessageBootReply) {
    LOG(ERROR) << "Invalid DHCP message op code";
    return false;
  }
  if (hardware_address_type_ != ARPHRD_ETHER) {
    LOG(ERROR) << "DHCP message device family id does not match";
    return false;
  }
  if (hardware_address_length_ != IFHWADDRLEN) {
    LOG(ERROR) <<
        "DHCP message device hardware address length does not match";
    return false;
  }
  // We have nothing to do with the 'hops' field.

  // The reply message from server should have the same xid we cached in client.
  // DHCP state machine will take charge of this checking.

  // According to RFC 2131, all secs field in reply messages should be 0.
  if (seconds_) {
    LOG(ERROR) << "Invalid DHCP message secs";
    return false;
  }

  // Check broadcast flags.
  // It should be 0 because we do not request broadcast reply.
  if (flags_) {
    LOG(ERROR) << "Invalid DHCP message flags";
    return false;
  }

  // We need to ensure the message contains the correct client hardware address.
  // DHCP state machine will take charge of this checking.

  // We do not use the bootfile field.
  if (cookie_ != kMagicCookie) {
    LOG(ERROR) << "DHCP message cookie does not match";
    return false;
  }
  return true;
}

}  // namespace dhcp_client
