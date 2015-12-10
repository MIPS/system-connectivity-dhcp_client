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

#include "dhcp_client/dhcpv4.h"

namespace dhcp_client {

DHCPV4::DHCPV4(const std::string& interface_name,
               const std::string& hardware_address,
               unsigned int interface_index,
               const std::string& network_id,
               bool request_hostname,
               bool arp_gateway,
               bool unicast_arp,
               EventDispatcherInterface* event_dispatcher)
    : interface_name_(interface_name),
      hardware_address_(hardware_address),
      interface_index_(interface_index),
      network_id_(network_id),
      request_hostname_(request_hostname),
      arp_gateway_(arp_gateway),
      unicast_arp_(unicast_arp),
      event_dispatcher_(event_dispatcher),
      state_(State::INIT) {
}

DHCPV4::~DHCPV4() {
}

void DHCPV4::Start() {
}

void DHCPV4::Stop() {
}

}  // namespace dhcp_client

