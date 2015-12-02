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

#include "dhcp_client/manager.h"
#include "dhcp_client/service.h"

namespace dhcp_client {

Service::Service(Manager* manager,
                 int service_identifier,
                 EventDispatcherInterface* event_dispatcher,
                 const brillo::VariantDictionary& configs)
    : manager_(manager),
      identifier_(service_identifier),
      event_dispatcher_(event_dispatcher),
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
}

}  // namespace dhcp_client

