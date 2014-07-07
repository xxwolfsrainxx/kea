// Copyright (C) 2014 Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef PKT_FILTER_BPF_H
#define PKT_FILTER_BPF_H

#include <dhcp/pkt_filter.h>

#include <util/buffer.h>

namespace isc {
namespace dhcp {

/// @brief Packet handling class using Berkeley Packet Filtering
///
/// This class provides methods to send and recive DHCPv4 messages using raw
/// sockets and Berkeley Packet Filtering. It is used by @c isc::dhcp::IfaceMgr
/// to send DHCPv4 messages to the hosts which don't have an IPv4 address
/// assigned yet.
class PktFilterBPF : public PktFilter {
public:

    /// @brief Check if packet can be sent to the host without address directly.
    ///
    /// This class supports direct responses to the host without address.
    ///
    /// @return true always.
    virtual bool isDirectResponseSupported() const {
        return (true);
    }

    /// @brief Open primary and fallback socket.
    ///
    /// @param iface Interface descriptor.
    /// @param addr Address on the interface to be used to send packets.
    /// @param port Port number.
    /// @param receive_bcast Configure socket to receive broadcast messages
    /// @param send_bcast Configure socket to send broadcast messages.
    ///
    /// @return A structure describing a primary and fallback socket.
    virtual SocketInfo openSocket(Iface& iface,
                                  const isc::asiolink::IOAddress& addr,
                                  const uint16_t port,
                                  const bool receive_bcast,
                                  const bool send_bcast);

    /// @brief Receive packet over specified socket.
    ///
    /// @param iface interface
    /// @param socket_info structure holding socket information
    ///
    /// @return Received packet
    virtual Pkt4Ptr receive(const Iface& iface, const SocketInfo& socket_info);

    /// @brief Send packet over specified socket.
    ///
    /// @param iface interface to be used to send packet
    /// @param sockfd socket descriptor
    /// @param pkt packet to be sent
    ///
    /// @return result of sending a packet. It is 0 if successful.
    virtual int send(const Iface& iface, uint16_t sockfd,
                     const Pkt4Ptr& pkt);

private:

    /// @brief Writes pseudo header containing an address family into a buffer.
    ///
    /// BPF utilizes the pseudo headers to pass the ancillary data between the
    /// kernel and the application. For example, when the packet is to be sent
    /// over the local loopback interface the pseudo header must be added before
    /// the network layer header to indicate the address family. Other link
    /// layer header (e.g. ethernet) is not used for local loopback interface.
    ///
    /// The header written by this method consists of 4 bytes and contains the
    /// address family value in host byte order. See sys/socket.h for the
    /// address family values. Typically it will be AF_INET.
    ///
    /// This function doesn't throw.
    ///
    /// @param address_family Address family (e.g. AF_INET).
    /// @param [out] out_buf buffer where a header is written.
    void writeAFPseudoHeader(const uint32_t address_family,
                             util::OutputBuffer& out_buf);

};

} // namespace isc::dhcp
} // namespace isc

#endif // PKT_FILTER_BPF_H
