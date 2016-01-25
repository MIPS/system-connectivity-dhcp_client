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

#include <linux/filter.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include <base/bind.h>
#include <base/logging.h>

#include "dhcp_client/dhcp_message.h"

using base::Bind;
using base::Unretained;
using shill::ByteString;
using shill::IOHandlerFactoryContainer;

namespace dhcp_client {

namespace {
// UDP port numbers for DHCP.
const uint16_t kDHCPServerPort = 67;
const uint16_t kDHCPClientPort = 68;

const int kInvalidSocketDescriptor = -1;

// Socket filter for dhcp packet.
const sock_filter dhcp_bpf_filter[] = {
  BPF_STMT(BPF_LD + BPF_B + BPF_ABS, 23 - ETH_HLEN),
  BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, IPPROTO_UDP, 0, 6),
  BPF_STMT(BPF_LD + BPF_H + BPF_ABS, 20 - ETH_HLEN),
  BPF_JUMP(BPF_JMP + BPF_JSET + BPF_K, 0x1fff, 4, 0),
  BPF_STMT(BPF_LDX + BPF_B + BPF_MSH, 14 - ETH_HLEN),
  BPF_STMT(BPF_LD + BPF_H + BPF_IND, 16 - ETH_HLEN),
  BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, kDHCPClientPort, 0, 1),
  BPF_STMT(BPF_RET + BPF_K, 0x0fffffff),
  BPF_STMT(BPF_RET + BPF_K, 0),
};
const int dhcp_bpf_filter_len =
    sizeof(dhcp_bpf_filter) / sizeof(dhcp_bpf_filter[0]);
}  // namespace

DHCPV4::DHCPV4(const std::string& interface_name,
               const ByteString& hardware_address,
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
      io_handler_factory_(
          IOHandlerFactoryContainer::GetInstance()->GetIOHandlerFactory()),
      state_(State::INIT),
      socket_(kInvalidSocketDescriptor),
      sockets_(new shill::Sockets()),
      from_(INADDR_ANY),
      to_(INADDR_BROADCAST) {
}

DHCPV4::~DHCPV4() {
  Stop();
}

void DHCPV4::ParseMessage(shill::InputData* data) {
  LOG(INFO) << __func__;
}

void DHCPV4::OnReadError(const std::string& error_msg) {
  LOG(INFO) << __func__;
}

bool DHCPV4::Start() {
  if (!CreateRawSocket()) {
    return false;
  }

  input_handler_.reset(io_handler_factory_->CreateIOInputHandler(
      socket_,
      Bind(&DHCPV4::ParseMessage, Unretained(this)),
      Bind(&DHCPV4::OnReadError, Unretained(this))));
  return true;
}

void DHCPV4::Stop() {
  input_handler_.reset();
  if (socket_ != kInvalidSocketDescriptor) {
    sockets_->Close(socket_);
  }
}

bool DHCPV4::CreateRawSocket() {
  int fd = sockets_->Socket(PF_PACKET,
                            SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK,
                            htons(ETHERTYPE_IP));
  if (fd == kInvalidSocketDescriptor) {
    PLOG(ERROR) << "Failed to create socket";
    return false;
  }
  shill::ScopedSocketCloser socket_closer(sockets_.get(), fd);

  // Apply the socket filter.
  sock_fprog pf;
  memset(&pf, 0, sizeof(pf));
  pf.filter = const_cast<sock_filter*>(dhcp_bpf_filter);
  pf.len = dhcp_bpf_filter_len;

  if (sockets_->AttachFilter(fd, &pf) != 0) {
    PLOG(ERROR) << "Failed to attach filter";
    return false;
  }

  if (sockets_->ReuseAddress(fd) == -1) {
    PLOG(ERROR) << "Failed to reuse socket address";
    return false;
  }

  if (sockets_->BindToDevice(fd, interface_name_) < 0) {
    PLOG(ERROR) << "Failed to bind socket to device";
    return false;
  }

  struct sockaddr_ll local;
  memset(&local, 0, sizeof(local));
  local.sll_family = PF_PACKET;
  local.sll_protocol = htons(ETHERTYPE_IP);
  local.sll_ifindex = static_cast<int>(interface_index_);

  if (sockets_->Bind(fd,
                     reinterpret_cast<struct sockaddr*>(&local),
                     sizeof(local)) < 0) {
    PLOG(ERROR) << "Failed to bind to address";
    return false;
  }

  socket_ = socket_closer.Release();
  return true;
}

bool DHCPV4::MakeRawPacket(const DHCPMessage& message, ByteString* output) {
  ByteString payload;
  if (!message.Serialize(&payload)) {
    LOG(ERROR) << "Failed to serialzie dhcp message";
    return false;
  }
  const size_t header_len = sizeof(struct iphdr) + sizeof(struct udphdr);
  const size_t payload_len = payload.GetLength();

  char buffer[header_len + payload_len];
  memset(buffer, 0, header_len + payload_len);
  struct iphdr* ip = reinterpret_cast<struct iphdr*>(buffer);
  struct udphdr* udp = reinterpret_cast<struct udphdr*>(buffer + sizeof(*ip));

  if (!payload.CopyData(payload_len, buffer + header_len)) {
    LOG(ERROR) << "Failed to copy data from payload";
    return false;
  }
  udp->uh_sport = htons(kDHCPClientPort);
  udp->uh_dport = htons(kDHCPServerPort);
  udp->uh_ulen =
      htons(static_cast<uint16_t>(sizeof(*udp) + payload.GetLength()));

  // Fill pseudo header (for UDP checksum computing):
  // Protocol.
  ip->protocol = IPPROTO_UDP;
  // Source IP address.
  ip->saddr = htonl(from_);
  // Destination IP address.
  ip->daddr = htonl(to_);
  // Total length, use udp packet length for pseudo header.
  ip->tot_len = udp->uh_ulen;
  // Calculate udp checksum based on:
  // IPV4 pseudo header, UDP header, and payload.
  udp->uh_sum = htons(DHCPMessage::ComputeChecksum(
      reinterpret_cast<const uint8_t*>(buffer),
      header_len + payload_len));

  // IP version.
  ip->version = IPVERSION;
  // IP header length.
  ip->ihl = sizeof(*ip) >> 2;
  // Fragment offset field.
  // The DHCP packet is always smaller than MTU,
  // so fragmentation is not needed.
  ip->frag_off = 0;
  // Identification.
  // TODO(nywang) Use arc4 random number.
  ip->id = static_cast<uint16_t>(rand());
  // Time to live.
  ip->ttl = IPDEFTTL;
  // Total length.
  ip->tot_len = htons(static_cast<uint16_t>(header_len+ payload.GetLength()));
  // Calculate IP Checksum only based on IP header.
  ip->check = htons(DHCPMessage::ComputeChecksum(
      reinterpret_cast<const uint8_t*>(ip),
      sizeof(*ip)));

  *output = ByteString(buffer, header_len + payload_len);
  return true;
}

bool DHCPV4::SendRawPacket(const ByteString& packet) {
  struct sockaddr_ll remote;
  memset(&remote, 0, sizeof(remote));
  remote.sll_family = AF_PACKET;
  remote.sll_protocol = htons(ETHERTYPE_IP);
  remote.sll_ifindex = interface_index_;
  remote.sll_hatype = htons(ARPHRD_ETHER);
  // Use broadcast hardware address.
  remote.sll_halen = IFHWADDRLEN;
  memset(remote.sll_addr, 0xff, IFHWADDRLEN);

  size_t result = sockets_->SendTo(socket_,
                                   packet.GetConstData(),
                                   packet.GetLength(),
                                   0,
                                   reinterpret_cast<struct sockaddr *>(&remote),
                                   sizeof(remote));

  if (result != packet.GetLength()) {
    PLOG(ERROR) << "Socket sento failed";
    return false;
  }
  return true;
}

}  // namespace dhcp_client

