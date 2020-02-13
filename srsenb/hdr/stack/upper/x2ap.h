/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
 
 /******************************************************************************
 * File:        x2ap.h
 * Description: Provide an interface for eNBs connect.
 *****************************************************************************/
 
#ifndef X2AP_H
#define X2AP_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srsenb/hdr/stack/upper/common_enb.h"

#include <sys/socket.h>
#include <sys/types.h>

namespace srsenb {

class x2ap : public srsenb::x2ap_interface_pdcp
{
public:
	x2ap(srslte::log* x2ap_log_);
	virtual ~x2ap();
	bool init(srsenb::pdcp_interface_x2ap* pdcp_, std::string x2ap_myaddr_, std::string x2ap_neiaddr_);
	void stop();
	
	// x2ap_interface_pdcp
	void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu);
	
	// X2-C/U each connect interface
	void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu);

	
private:
	srsenb::pdcp_interface_x2ap* pdcp = nullptr;
	srslte::log* x2ap_log = nullptr;
	
	static const int X2AP_PORT = 8888;
	std::string x2ap_myaddr;
	std::string x2ap_neiaddr;
	
	srslte::byte_buffer_pool* pool = nullptr;
	
	
	// Socket
	int socket_fd = -1;
	int receive_fd = -1;
	socklen_t addrlen;
	struct sockaddr_in s_myaddr;
	struct sockaddr_in s_neighaddr;
};
	
} // namespace srsenb

#endif // SRSENB_X2AP_H