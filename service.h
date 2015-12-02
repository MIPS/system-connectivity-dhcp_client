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

#ifndef DHCP_CLIENT_SERVICE_H_
#define DHCP_CLIENT_SERVICE_H_

#include <string>

#include <base/macros.h>
#include <base/memory/ref_counted.h>
#include <brillo/variant_dictionary.h>

#include "dhcp_client/event_dispatcher_interface.h"

namespace dhcp_client {

class Manager;

class Service : public base::RefCounted<Service> {
 public:
  Service(Manager* manager,
          int service_identifier,
          EventDispatcherInterface* event_dispatcher,
          const brillo::VariantDictionary& configs);

  virtual ~Service();
  void Start();
  void Stop();

 private:
  Manager* manager_;
  int identifier_;
  EventDispatcherInterface* event_dispatcher_;

  std::string interface_name_;
  int type_;
  std::string network_id_;

  // DHCP IPv4 configurations:
  // Request hostname from server
  bool request_hostname_;
  // ARP for default gateway
  bool arp_gateway_;
  // Enable unicast ARP on renew
  bool unicast_arp_;

  // DHCP IPv6 configurations:
  // Request non-temporary address
  bool request_na_;
  // Request prefix delegation
  bool request_pd_;

  void ParseConfigs(const brillo::VariantDictionary& configs);

  DISALLOW_COPY_AND_ASSIGN(Service);
};

}  // namespace dhcp_client

#endif  // DHCP_CLIENT_SERVICE_H_
