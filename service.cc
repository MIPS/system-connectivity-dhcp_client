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

#include "dhcp_client/service.h"

#include <string>

#include "dhcp_client/manager.h"

using std::string;

namespace {
const char kConstantInterfaceName[] = "interface_name";
const char kConstantDHCPType[] = "type";
const char kConstantNetworkIdentifier[] = "identifier";
const char kConstantRequestHostname[] = "request_hostname";
const char kConstantArpGateway[] = "arp_gateway";
const char kConstantUnicastArp[] = "unicast_arp";
const char kConstantRequestNontemporaryAddress[] = "request_na";
const char kConstantRequestPrefixDelegation[] = "request_pf";
}

namespace dhcp_client {

Service::Service(Manager* manager,
                 int service_identifier,
                 EventDispatcherInterface* event_dispatcher,
                 const brillo::VariantDictionary& configs)
    : manager_(manager),
      identifier_(service_identifier),
      event_dispatcher_(event_dispatcher),
      request_hostname_(false),
      arp_gateway_(false),
      unicast_arp_(false),
      request_na_(false),
      request_pd_(false) {
  ParseConfigs(configs);
}

Service::~Service() {
}

void Service::Start() {
}

void Service::Stop() {
}

void Service::ParseConfigs(const brillo::VariantDictionary& configs) {
  for (const auto& key_and_value : configs) {
    const std::string& key = key_and_value.first;
    const auto& value = key_and_value.second;
    if (key ==  kConstantInterfaceName && value.IsTypeCompatible<string>()) {
      interface_name_ = value.Get<string>();
    } else if (key == kConstantDHCPType && value.IsTypeCompatible<int32_t>()) {
      type_  = value.Get<int32_t>();
    } else if (key == kConstantNetworkIdentifier &&
               value.IsTypeCompatible<string>()) {
      network_id_ = value.Get<string>();
    } else if (key == kConstantRequestHostname &&
               value.IsTypeCompatible<bool>()) {
      request_hostname_ = value.Get<bool>();
    } else if (key == kConstantArpGateway && value.IsTypeCompatible<bool>()) {
      arp_gateway_ = value.Get<bool>();
    } else if (key == kConstantUnicastArp && value.IsTypeCompatible<bool>()) {
      unicast_arp_ = value.Get<bool>();
    } else if (key == kConstantRequestNontemporaryAddress &&
      value.IsTypeCompatible<bool>()) {
      request_na_ = value.Get<bool>();
    } else if (key == kConstantRequestPrefixDelegation &&
               value.IsTypeCompatible<bool>()) {
      request_pd_ = value.Get<bool>();
    } else {
      LOG(ERROR) << "Invalid configuration with key: " << key;
    }
  }
}

}  // namespace dhcp_client

