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
 
 #include "srslte/upper/x2ap.h"
 
 
 namespace srsenb {

/****************************************************************************
 * Ref: 3GPP TS36.423 v16.0.0 
 ***************************************************************************/
 
 x2ap(log* log_)
 {
	x2ap_log = log_;
 }
 
 bool x2ap::init(srsenb::pdcp_interface_x2ap* pdcp_, std::string x2ap_myaddr_, std::string x2ap_neiaddr_)
 {
	pdcp = pdcp_;
	x2ap_myaddr = x2ap_myaddr_;
	x2ap_neiaddr = x2ap_neiaddr_;
	
	// Set up socket
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_fd < 0)
	{
		x2ap_log->error("Failed to create x2ap socket\n");
		#if(NUK_JIN_DEBUG)
		{
			x2ap_log->console("Failed to create x2ap socket\n");
		}
		#endif
		return false;
	}
	
	int enable = 1;
#if defined(SO_REUSEADDR)
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    x2ap_log->error("setsockopt(SO_REUSEADDR) failed\n");
#endif
#if defined(SO_REUSEPORT)
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    x2ap_log->error("setsockopt(SO_REUSEPORT) failed\n");
#endif

	bzero(&s_myaddr , sizeof(struct sockaddr_in));
	s_myaddr.sin_family = AF_INET;
	s_myaddr.sin_addr.s_addr = inet_addr(x2ap_myaddr.c_str());
	s_myaddr.sinport = htons(X2AP_PORT);
	
	if(bind(socket_fd , (struct sockaddr*)&s_myaddr, sizeof(struct sockaddr_in)) < 0)
	{
		x2ap_log->error("Failed to bind on address %s, port %d\n", x2ap_myaddr.c_str(), X2AP_PORT);
		x2ap_log->console("Failed to bind on address %s, port %d\n", x2ap_myaddr.c_str(), X2AP_PORT);
		return false;
	}
#if(NUK_JIN_DEBUG)
{
	x2ap_log->console("X2AP socket_fd = %d\n", socket_fd);
	x2ap_log->console("X2AP IP = %s  , port = %d \n", x2ap_myaddr, X2AP_PORT);
	
	x2ap_log->info("X2AP socket_fd = %d\n", socket_fd);
	x2ap_log->info("X2AP IP = %s  , port = %d \n", x2ap_myaddr, X2AP_PORT);
}
#endif
	
	
	return true;
 }
 
 void x2ap::stop()
 {
	 if(socket_fd)
	 {
		close(socket_fd);
	 }
 }
 
 void x2ap::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
 {
	x2ap_log->info_hex(pdu->msg, pdu->N_bytes, "TX PDU, RNTI: 0x%x, LCID: %d, n_bytes=%d", rnti, lcid, pdu->N_bytes);
	
	bzero(&s_neighaddr , sizeof(struct sockaddr_in));
	s_neighaddr.sin_family = AF_INET;
	inet_aton(x2ap_neiaddr.c_str(), &s_neighaddr.sin_addr);
	s_neighaddr.sinport = htons(X2AP_PORT);
	
	if(sento(socket_fd, pdu->msg, pdu->N_bytes, MSG_EOR, (struct sockaddr*)&s_neighaddr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("x2ap write_pdu sento error");
	}
 }
 
 void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)
 {
	// select() for prevent blocking / thread control
	s_length = sizeof(s_neighaddr);
	if((recv_bytes = recvfrom(socket_fd, buffer, SRSENB_MAX_BUFFER_SIZE_BYTES - SRSENB_BUFFER_HEADER_OFFSET ,0 , &s_neighaddr, s_length)) < 0 )
	{
		perror("could not read datagram! \n");
	}
	
	pdcp->wirte_sdu(rnti, lcid, buffer);
 }
 
 } // namespace srsenb