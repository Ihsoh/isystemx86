/**
	@File:			net.c
	@Author:		Ihsoh
	@Date:			2017-02-02
	@Description:
		一些与网络相关的功能。
*/

#include "net.h"
#include "pcnet2.h"
#include "endian.h"
#include "helper.h"

#include "../memory.h"

static NetDevicePtr _devices[MAX_NET_DEVICE];
static uint32 _count = 0;
static NetARPRecord _arp_records[MAX_ARP_RECORD];
static uint32 _arp_record_count = 0;

static
void
_NetFillIPv4Packet(
	IN NetDevicePtr netdev,
	IN OUT NetIPv4PacketPtr pk_ipv4,
	IN uint32 buffer_len,
	IN uint8 dscp,
	IN uint8 ecn,
	IN uint8 ttl,
	IN uint8 protocol,
	IN uint8 * mac_dst,
	IN uint8 * ip_dst)
{
	// 获取本机MAC地址和IP地址。
	uint8 * mac_src = netdev->GetMAC(netdev);
	uint8 * ip_src = netdev->GetIP(netdev);

	// 填充以太网帧。
	MemCopy(mac_dst, pk_ipv4->fr_eth.mac_dst, 6);
	MemCopy(mac_src, pk_ipv4->fr_eth.mac_src, 6);
	pk_ipv4->fr_eth.type = NetToBigEndian16(0x0800);

	// 填充IPv4帧。
	pk_ipv4->fr_ipv4.version = 4;
	pk_ipv4->fr_ipv4.ihl = 5;
	pk_ipv4->fr_ipv4.dscp = dscp;
	pk_ipv4->fr_ipv4.ecn = ecn;
	pk_ipv4->fr_ipv4.total_length = NetToBigEndian16(buffer_len - sizeof(NetEthernetFrame));
	pk_ipv4->fr_ipv4.identification = 0;
	pk_ipv4->fr_ipv4.flags = 0x02;
	pk_ipv4->fr_ipv4.fragment_offset_l8 = 0;
	pk_ipv4->fr_ipv4.fragment_offset_h5 = 0;
	pk_ipv4->fr_ipv4.ttl = ttl;
	pk_ipv4->fr_ipv4.protocol = protocol;
	pk_ipv4->fr_ipv4.checksum = 0;
	MemCopy(ip_src, pk_ipv4->fr_ipv4.ip_src, 4);
	MemCopy(ip_dst, pk_ipv4->fr_ipv4.ip_dst, 4);

	// 计算IPv4帧的校验和。
	int32 i;
	uint32 checksum = 0;
	uint16 * w = (uint16 *)&pk_ipv4->fr_ipv4;
	for (i = 0; i < sizeof(NetIPv4Frame) / 2; i++, w++)
	{
		checksum += *w;
	}
	while ((checksum & 0xffff0000) != 0)
	{
		checksum = (checksum & 0x0000ffff) + ((checksum >> 16) & 0x0000ffff);
	}
	checksum = ~checksum;
	pk_ipv4->fr_ipv4.checksum = checksum;
}

BOOL
NetSendUDP(
	IN NetDevicePtr netdev,
	IN uint16 port_src,
	IN uint8 * ip_dst,
	IN uint16 port_dst,
	IN uint8 * data,
	IN uint16 len)
{
	uint32 i;

	if (netdev == NULL
		|| ip_dst == NULL
		|| data == NULL
		|| len > MAX_UDP_DATA_LEN)
	{
		goto err;
	}

	NetARPRecordPtr record = NetFindARPRecord(ip_dst);
	if (record == NULL)
	{
		goto err;
	}
	uint8 * mac_dst = record->mac;

	uint8 * mac_src = netdev->GetMAC(netdev);
	uint8 * ip_src = netdev->GetIP(netdev);

	uint8 buffer[MAX_NET_PACKET_LEN];
	MemClear(buffer, 0x00, sizeof(buffer));
	NetIPv4UDPPacketPtr pk_udp = (NetIPv4UDPPacketPtr)buffer;
	uint32 pk_len = sizeof(NetIPv4UDPPacket) + len;

	// 以太网帧。
	MemCopy(mac_dst, pk_udp->fr_eth.mac_dst, 6);
	MemCopy(mac_src, pk_udp->fr_eth.mac_src, 6);
	pk_udp->fr_eth.type = NetToBigEndian16(0x0800);

	// IPv4帧。
	pk_udp->fr_ipv4.version = 4;
	pk_udp->fr_ipv4.ihl = 5;
	pk_udp->fr_ipv4.dscp = 0x08;
	pk_udp->fr_ipv4.ecn = 0x00;
	pk_udp->fr_ipv4.total_length = NetToBigEndian16(pk_len - sizeof(NetEthernetFrame));
	pk_udp->fr_ipv4.identification = 0;
	pk_udp->fr_ipv4.flags = 0x02;
	pk_udp->fr_ipv4.fragment_offset_l8 = 0;
	pk_udp->fr_ipv4.fragment_offset_h5 = 0;
	pk_udp->fr_ipv4.ttl = 50;
	pk_udp->fr_ipv4.protocol = 17;
	pk_udp->fr_ipv4.checksum = 0;
	MemCopy(ip_src, pk_udp->fr_ipv4.ip_src, 4);
	MemCopy(ip_dst, pk_udp->fr_ipv4.ip_dst, 4);

	uint32 checksum = 0;
	uint16 * w = (uint16 *)&pk_udp->fr_ipv4;
	for (i = 0; i < sizeof(NetIPv4Frame) / 2; i++, w++)
	{
		checksum += *w;
	}
	while ((checksum & 0xffff0000) != 0)
	{
		checksum = (checksum & 0x0000ffff) + ((checksum >> 16) & 0x0000ffff);
	}
	checksum = ~checksum;
	pk_udp->fr_ipv4.checksum = checksum;

	// UDP帧。
	uint32 udp_length = sizeof(NetUDPFrame) + len;
	NetUDPFrameWithIPv4PseudoHeader udp_ph;
	MemCopy(ip_src, udp_ph.ip_src, 4);
	MemCopy(ip_dst, udp_ph.ip_dst, 4);
	udp_ph.zeroes = 0;
	udp_ph.protocol = 17;
	udp_ph.udp_length = NetToBigEndian16(udp_length);
	udp_ph.udp.port_src = NetToBigEndian16(port_src);
	udp_ph.udp.port_dst = NetToBigEndian16(port_dst);
	udp_ph.udp.length = NetToBigEndian16(udp_length);
	udp_ph.udp.checksum = 0;

	MemCopy(data, buffer + sizeof(NetIPv4UDPPacket), len);

	checksum = 0;
	w = (uint16 *)&udp_ph;
	for (i = 0; i < sizeof(NetUDPFrameWithIPv4PseudoHeader) / 2; i++, w++)
	{
		checksum += *w;
	}
	for (i = 0; i < len; i++)
	{
		uint32 data = buffer[sizeof(NetIPv4UDPPacket) + i];
		if (i % 2 == 1)
		{
			data = data << 8;
		}
		checksum += data;
	}
	while ((checksum & 0xffff0000) != 0)
	{
		checksum = (checksum & 0x0000ffff) + ((checksum >> 16) & 0x0000ffff);
	}
	checksum = ~checksum;
	udp_ph.udp.checksum = checksum;

	MemCopy(&udp_ph.udp, &pk_udp->fr_udp, sizeof(NetUDPFrame));

	return netdev->SendPacket(netdev, buffer, pk_len);
err:
	return FALSE;
}

static
void
_NetProcessPing(
	IN NetDevicePtr netdev,
	IN void * packet,
	IN uint16 len)
{
	uint8 buffer[MAX_NET_PACKET_LEN];
	if (len < sizeof(NetICMPPacket)
		&& len > sizeof(buffer))
	{
		return;
	}
	NetICMPPacketPtr pk_icmp = (NetICMPPacketPtr)packet;
	NetICMPFramePtr fr_icmp = (NetICMPFramePtr)&pk_icmp->fr_icmp;
	NetICMPPacketPtr pk_pong = (NetICMPPacketPtr)buffer;
	_NetFillIPv4Packet(
		netdev,
		pk_pong,
		len,
		0x00,
		0x00,
		56,
		1,
		pk_icmp->fr_eth.mac_src,
		pk_icmp->fr_ipv4.ip_src);
	pk_pong->fr_icmp.type = 0;
	pk_pong->fr_icmp.code = 0;
	pk_pong->fr_icmp.checksum = 0;
	pk_pong->fr_icmp.data = fr_icmp->data;
	MemCopy(packet + sizeof(NetICMPPacket), buffer + sizeof(NetICMPPacket), len - sizeof(NetICMPPacket));

	// 计算ICMP帧校验和。
	int32 i;
	uint32 checksum = 0;
	for (i = 0; i < len - sizeof(NetIPv4Packet); i++)
	{
		uint32 data = buffer[sizeof(NetIPv4Packet) + i];
		if (i % 2 == 1)
		{
			data = data << 8;
		}
		checksum += data;
	}
	while ((checksum & 0xffff0000) != 0)
	{
		checksum = (checksum & 0x0000ffff) + ((checksum >> 16) & 0x0000ffff);
	}
	checksum = ~checksum;
	pk_pong->fr_icmp.checksum = checksum;

	// 发送。
	netdev->SendPacket(netdev, buffer, len);
}

static
void
_NetProcessICMP(
	IN NetDevicePtr netdev,
	IN void * packet,
	IN uint16 len)
{
	if (len < sizeof(NetICMPPacket))
	{
		return;
	}
	NetICMPPacketPtr pk_icmp = (NetICMPPacketPtr)packet;
	NetICMPFramePtr fr_icmp = (NetICMPFramePtr)&pk_icmp->fr_icmp;
	switch (fr_icmp->type)
	{
		case 0:
		{

			break;
		}
		case 8:
		{
			_NetProcessPing(netdev, packet, len);
			break;
		}
	}
}

static
void
_NetProcessUDP(
	IN NetDevicePtr netdev,
	IN void * packet,
	IN uint16 len)
{
	if (len < sizeof(NetIPv4UDPPacket))
	{
		return;
	}

	// 获取本机MAC地址和IP地址。
	uint8 * mac = netdev->GetMAC(netdev);
	uint8 * ip = netdev->GetIP(netdev);

	NetIPv4UDPPacketPtr pk_udp = (NetIPv4UDPPacketPtr)packet;
	if (NetCompareMAC(mac, pk_udp->fr_eth.mac_dst) && NetCompareIP(ip, pk_udp->fr_ipv4.ip_dst))
	{
		uint8 * data = (uint8 *)packet;
		data += sizeof(NetIPv4UDPPacket);
		uint16 data_len = len - sizeof(NetIPv4UDPPacket);
		uint8 * ip_src = pk_udp->fr_ipv4.ip_src;
		uint16 port_src = NetToLittleEndian16(pk_udp->fr_udp.port_src);
		uint8 * ip_dst = pk_udp->fr_ipv4.ip_dst;
		uint16 port_dst = NetToLittleEndian16(pk_udp->fr_udp.port_dst);
		NetProcessUDP f = netdev->ProcessUDP[port_dst];
		if (f != NULL)
		{
			f(netdev, ip_src, port_src, ip_dst, port_dst, data, data_len);
		}
	}
}

static
void
_NetProcessIPv4(
	IN NetDevicePtr netdev,
	IN void * packet,
	IN uint16 len)
{
	if (len < sizeof(NetIPv4Packet))
	{
		return;
	}
	NetIPv4PacketPtr pk_ipv4 = (NetIPv4PacketPtr)packet;
	NetIPv4FramePtr fr_ipv4 = (NetIPv4FramePtr)&pk_ipv4->fr_ipv4;
	switch (fr_ipv4->protocol)
	{
		// ICMP
		case 1:
		{
			_NetProcessICMP(netdev, packet, len);
			break;
		}
		// IGMP
		case 2:
		{

			break;
		}
		// TCP
		case 6:
		{

			break;
		}
		// UDP
		case 17:
		{
			_NetProcessUDP(netdev, packet, len);
			break;
		}
		// OSPF
		case 89:
		{

			break;
		}
		// SCTP
		case 132:
		{

			break;
		}
		default:
		{

			break;
		}
	}
}

static
void
_NetProcessARP(
	IN NetDevicePtr netdev,
	IN void * packet,
	IN uint16 len)
{
	if (len < sizeof(NetARPPacket))
	{
		return;
	}
	uint8 * netdev_mac = netdev->GetMAC(netdev);
	uint8 * netdev_ip = netdev->GetIP(netdev);
	NetARPPacketPtr pk_arp = (NetARPPacketPtr)packet;
	NetARPFramePtr fr_arp = &pk_arp->fr_arp;
	if (fr_arp->htype == NetToBigEndian16(0x0001)
		&& fr_arp->ptype == NetToBigEndian16(0x0800)
		&& fr_arp->hlen == 6
		&& fr_arp->plen == 4
		&& fr_arp->tpa[0] == netdev_ip[0]
		&& fr_arp->tpa[1] == netdev_ip[1]
		&& fr_arp->tpa[2] == netdev_ip[2]
		&& fr_arp->tpa[3] == netdev_ip[3])
	{
		if (fr_arp->oper == NetToBigEndian16(0x0001))
		{
			NetARPPacket pk_arp_reply;
			MemClear(&pk_arp_reply, 0x00, sizeof(pk_arp_reply));

			// 以太网帧。
			MemCopy(fr_arp->sha, pk_arp_reply.fr_eth.mac_dst, 6);
			MemCopy(netdev_mac, pk_arp_reply.fr_eth.mac_src, 6);
			pk_arp_reply.fr_eth.type = NetToBigEndian16(0x0806);

			// ARP帧。
			pk_arp_reply.fr_arp.htype = NetToBigEndian16(0x0001);
			pk_arp_reply.fr_arp.ptype = NetToBigEndian16(0x0800);
			pk_arp_reply.fr_arp.hlen = 6;
			pk_arp_reply.fr_arp.plen = 4;
			pk_arp_reply.fr_arp.oper = NetToBigEndian16(0x0002);
			MemCopy(netdev_mac, pk_arp_reply.fr_arp.sha, 6);
			MemCopy(netdev_ip, pk_arp_reply.fr_arp.spa, 4);
			MemCopy(fr_arp->sha, pk_arp_reply.fr_arp.tha, 6);
			MemCopy(fr_arp->spa, pk_arp_reply.fr_arp.tpa, 4);

			// 发送。
			netdev->SendPacket(netdev, &pk_arp_reply, sizeof(pk_arp_reply));
		}
		else if (fr_arp->oper == NetToBigEndian16(0x0002))
		{
			if (_arp_record_count < MAX_ARP_RECORD)
			{
				NetARPRecordPtr arp_record = &_arp_records[_arp_record_count++];
				MemCopy(fr_arp->sha, arp_record->mac, sizeof(arp_record->mac));
				MemCopy(fr_arp->spa, arp_record->ip, sizeof(arp_record->ip));
			}
		}
	}
}

static void _NetProcessPacket(IN void * device, IN void * packet, IN uint16 len)
{
	if (len < sizeof(NetEthernetFrame))
	{
		return;
	}
	NetEthernetFramePtr fr_eth = (NetEthernetFramePtr)packet;
	uint16 type = NetToLittleEndian16(fr_eth->type);
	switch (type)
	{
		// Internet Protocol version 4 (IPv4)
		case 0x0800:
		{
			_NetProcessIPv4(device, packet, len);
			break;
		}
		// Address Resolution Protocol (ARP)
		case 0x0806:
		{
			_NetProcessARP(device, packet, len);
			break;
		}
		default:
		{

			break;
		}
	}
}

BOOL
NetAdd(
	IN NetDevicePtr device)
{
	uint32 i;
	if (_count >= MAX_NET_DEVICE
		|| device == NULL)
	{
		return;
	}
	device->ProcessPacket = _NetProcessPacket;
	for (i = 0; i < MAX_UDP_PORT; i++)
	{
		device->ProcessUDP[i] = NULL;
	}
	_devices[_count++] = device;
}

uint32
NetGetCount(void)
{
	return _count;
}

NetDevicePtr
NetGet(
	IN uint32 index)
{
	if (index >= _count)
	{
		return NULL;
	}
	return _devices[index];
}

uint32
NetGetARPRecordCount(void)
{
	return _arp_record_count;
}

NetARPRecordPtr
NetGetARPRecord(
	IN uint32 index)
{
	if (index >= _arp_record_count)
	{
		return NULL;
	}
	return &_arp_records[index];
}

NetARPRecordPtr
NetFindARPRecord(
	IN uint8 * ip)
{
	if (ip == NULL)
	{
		return NULL;
	}
	uint32 i;
	for (i = 0; i < _arp_record_count; i++)
	{
		if (*(uint32 *)ip == *(uint32 *)_arp_records[i].ip)
		{
			return &_arp_records[i];
		}
	}
	return NULL;
}

void
NetInit(void)
{
	int32 i;
	for (i = 0; i < MAX_NET_DEVICE; i++)
	{
		_devices[i] = NULL;
	}

	PCNET2Init();
}
