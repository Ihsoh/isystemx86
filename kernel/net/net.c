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

static NetDevicePtr _devices[MAX_NET_DEVICE];
static uint32 _count = 0;
static NetARPRecord _arp_records[MAX_ARP_RECORD];
static uint32 _arp_record_count = 0;

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

			break;
		}
		// Address Resolution Protocol (ARP)
		case 0x0806:
		{
			_NetProcessARP(device, packet, len);
			break;
		}
	}
}

BOOL
NetAdd(
	IN NetDevicePtr device)
{
	if (_count >= MAX_NET_DEVICE
		|| device == NULL)
	{
		return;
	}
	device->ProcessPacket = _NetProcessPacket;
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
