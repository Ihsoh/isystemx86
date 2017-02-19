/**
	@File:			net.h
	@Author:		Ihsoh
	@Date:			2017-02-02
	@Description:
		一些与网络相关的结构体和常量。
*/

#ifndef	_NET_NET_H_
#define	_NET_NET_H_

#include "../types.h"

#define	MAX_NET_DEVICE	4
#define	MAX_ARP_RECORD	64
#define	MAX_UDP_PORT	65536

typedef BOOL (* NetSetIP)(IN void * device, IN uint8 * ip);
typedef uint8 * (* NetGetIP)(IN void * device);
typedef BOOL (* NetSetMAC)(IN void * device, IN uint8 * mac);
typedef uint8 * (* NetGetMAC)(IN void * device);
typedef CASCTEXT (* NetGetName)(IN void * device);
typedef BOOL (* NetSendPacket)(IN void * device, IN void * packet, IN uint16 len);
typedef void (* NetProcessPacket)(IN void * device, IN void * packet, IN uint16 len);

typedef void (* NetProcessUDP)(
	IN void * device,
	IN uint8 * ip_src,
	IN uint16 port_src,
	IN uint8 * ip_dst,
	IN uint16 port_dst,
	IN uint8 * data,
	IN uint16 len);

typedef struct
{
	uint32				id;
	NetSetIP			SetIP;
	NetGetIP			GetIP;
	NetSetMAC			SetMAC;
	NetGetMAC			GetMAC;
	NetGetName			GetName;
	NetSendPacket		SendPacket;
	NetProcessPacket	ProcessPacket;

	NetProcessUDP		ProcessUDP[MAX_UDP_PORT];
} NetDevice, * NetDevicePtr;

typedef struct
{
	uint8	ip[4];
	uint8	mac[6];
} NetARPRecord, * NetARPRecordPtr;

typedef struct
{
	// 目标MAC地址。
	uint8		mac_dst[6];

	// 源MAC地址。
	uint8		mac_src[6];

	// 协议类型。
	//
	// 0x0800	Internet Protocol version 4 (IPv4)
	// 0x0806	Address Resolution Protocol (ARP)
	// 0x0842	Wake-on-LAN[4]
	// 0x22F3	IETF TRILL Protocol
	// 0x6003	DECnet Phase IV
	// 0x8035	Reverse Address Resolution Protocol
	// 0x809B	AppleTalk (Ethertalk)
	// 0x80F3	AppleTalk Address Resolution Protocol (AARP)
	// 0x8100	VLAN-tagged frame (IEEE 802.1Q) and Shortest Path Bridging IEEE 802.1aq[5]
	// 0x8137	IPX
	// 0x8204	QNX Qnet
	// 0x86DD	Internet Protocol Version 6 (IPv6)
	// 0x8808	Ethernet flow control
	// 0x8819	CobraNet
	// 0x8847	MPLS unicast
	// 0x8848	MPLS multicast
	// 0x8863	PPPoE Discovery Stage
	// 0x8864	PPPoE Session Stage
	// 0x887B	HomePlug 1.0 MME
	// 0x888E	EAP over LAN (IEEE 802.1X)
	// 0x8892	PROFINET Protocol
	// 0x889A	HyperSCSI (SCSI over Ethernet)
	// 0x88A2	ATA over Ethernet
	// 0x88A4	EtherCAT Protocol
	// 0x88A8	Provider Bridging (IEEE 802.1ad) & Shortest Path Bridging IEEE 802.1aq[5]
	// 0x88AB	Ethernet Powerlink[citation needed]
	// 0x88B8	GOOSE (Generic Object Oriented Substation event)
	// 0x88B9	GSE (Generic Substation Events) Management Services
	// 0x88BA	SV (Sampled Value Transmission)
	// 0x88CC	Link Layer Discovery Protocol (LLDP)
	// 0x88CD	SERCOS III
	// 0x88E1	HomePlug AV MME[citation needed]
	// 0x88E3	Media Redundancy Protocol (IEC62439-2)
	// 0x88E5	MAC security (IEEE 802.1AE)
	// 0x88E7	Provider Backbone Bridges (PBB) (IEEE 802.1ah)
	// 0x88F7	Precision Time Protocol (PTP) over Ethernet (IEEE 1588)
	// 0x88FB	Parallel Redundancy Protocol (PRP)
	// 0x8902	IEEE 802.1ag Connectivity Fault Management (CFM) Protocol / ITU-T Recommendation Y.1731 (OAM)
	// 0x8906	Fibre Channel over Ethernet (FCoE)
	// 0x8914	FCoE Initialization Protocol
	// 0x8915	RDMA over Converged Ethernet (RoCE)
	// 0x891D	TTEthernet Protocol Control Frame (TTE)
	// 0x892F	High-availability Seamless Redundancy (HSR)
	// 0x9000	Ethernet Configuration Testing Protocol[6]
	// 0x9100	VLAN-tagged (IEEE 802.1Q) frame with double tagging
	uint16		type;
} __attribute__((packed)) NetEthernetFrame, * NetEthernetFramePtr;

typedef struct
{
	// 第二个字段是4位首部长度，说明首部有多少32位字长。由于IPv4首部可能包含数目不定的选项，
	// 这个字段也用来确定数据的偏移量。这个字段的最小值是5（RFC 791），最大值是15。
	uint8		ihl:4;

	// IP报文首部的第一个字段是4位版本字段。对IPv4来说，这个字段的值是4。
	uint8		version:4;
	
	// 在RFC 3168中定义，允许在不丢弃报文的同时通知对方网络拥塞的发生。
	// ECN是一种可选的功能，仅当两端都支持并希望使用，且底层网络支持时才被使用。
	uint8		ecn:2;
	
	// 最初被定义为服务类型字段，但被RFC 2474重定义为DiffServ。
	// 新的需要实时数据流的技术会应用这个字段，一个例子是VoIP。
	uint8		dscp:6;

	// 这个16位字段定义了报文总长，包含首部和数据，单位为字节。
	// 这个字段的最小值是20（20字节首部+0字节数据），最大值是65,535。
	// 所有主机都必须支持最小576字节的报文，但大多数现代主机支持更大的报文。
	// 有时候子网会限制报文的大小，这时报文就必须被分片。
	uint16		total_length;

	// 这个字段主要被用来唯一地标识一个报文的所有分片。一些实验性的工作建议将此字段用于其它目的，
	// 例如增加报文跟踪信息以协助探测伪造的源地址。
	uint16		identification;

	// 这个5位字段指明了每个分片相对于原始报文开头的偏移量（偏移量总共13位，这里为高5位），以8字节作单位。
	uint8		fragment_offset_h5:5;

	// 这个3位字段用于控制和识别分片，它们是：
	// 		位0：保留，必须为0。
	// 		位1：禁止分片（DF）。
	// 		位2：更多分片（MF）。
	// 如果DF标志被设置但路由要求必须分片报文，此报文会被丢弃。这个标志可被用于发往没有能力组装分片的主机。
	// 当一个报文被分片，除了最后一片外的所有分片都设置MF标志。不被分片的报文不设置MF标志：它是它自己的最后一片。
	uint8		flags:3;

	// 这个8位字段指明了每个分片相对于原始报文开头的偏移量（偏移量总共13位，这里为低8位），以8字节作单位。
	uint8		fragment_offset_l8;

	// 这个8位字段避免报文在互联网中永远存在（例如陷入路由环路）。
	// 存活时间以秒为单位，但小于一秒的时间均向上取整到一秒。在现实中，
	// 这实际上成了一个跳数计数器：报文经过的每个路由器都将此字段减一，当此字段等于0时，
	// 报文不再向下一跳传送并被丢弃。常规地，一份ICMP报文被发回报文发送端说明其发送的报文已被丢弃。
	// 这也是traceroute的核心原理。
	uint8		ttl;

	// 这个字段定义了该报文数据区使用的协议。IANA维护着一份协议列表（最初由RFC 790定义）。
	// 
	// 协议字段值		协议名				缩写
	// =====================================
	// 1			互联网控制消息协议		ICMP
	// 2			互联网组管理协议		IGMP
	// 6			传输控制协议			TCP
	// 17			用户数据报协议		UDP
	// 41			IPv6封装				-
	// 89			开放式最短路径优先		OSPF
	// 132			流控制传输协议		SCTP
	uint8		protocol;

	// 这个16位检验和字段用于对首部查错。在每一跳，计算出的首部检验和必须与此字段进行比对，
	// 如果不一致，此报文被丢弃。值得注意的是，数据区的错误留待上层协议处理——用户数据报协
	// 议和传输控制协议都有检验和字段。
	uint16		checksum;

	// 源IP地址。
	uint8		ip_src[4];

	// 目IP标地址。
	uint8		ip_dst[4];
} __attribute__((packed)) NetIPv4Frame, * NetIPv4FramePtr;

typedef struct
{
	NetEthernetFrame	fr_eth;

	NetIPv4Frame		fr_ipv4;
} __attribute__((packed)) NetIPv4Packet, * NetIPv4PacketPtr;

typedef struct
{
	// 源端口。
	uint16		port_src;

	// 目标端口。
	uint16		port_dst;

	// 报文长度。
	uint16		length;

	// 检验和。
	uint16		checksum;
} __attribute__((packed)) NetUDPFrame, * NetUDPFramePtr;

typedef struct
{
	// 源IP地址。
	uint8			ip_src[4];

	// 目IP标地址。
	uint8			ip_dst[4];

	// 填充0。
	uint8			zeroes;

	// 协议。
	uint8			protocol;

	// UDP报文长度。
	uint16			udp_length;

	// UDP报文。
	NetUDPFrame		udp;
} __attribute__((packed)) NetUDPFrameWithIPv4PseudoHeader, * NetUDPFrameWithIPv4PseudoHeaderPtr;

typedef struct
{
	NetEthernetFrame	fr_eth;

	NetIPv4Frame		fr_ipv4;

	NetUDPFrame			fr_udp;
} __attribute__((packed)) NetIPv4UDPPacket, * NetIPv4UDPPacketPtr;

typedef struct
{
	uint16			htype;

	uint16			ptype;

	uint8			hlen;

	uint8			plen;

	uint16			oper;

	uint8			sha[6];

	uint8			spa[4];

	uint8			tha[6];

	uint8			tpa[4];
} __attribute__((packed)) NetARPFrame, * NetARPFramePtr;


typedef struct
{
	NetEthernetFrame	fr_eth;

	NetARPFrame			fr_arp;
} __attribute__((packed)) NetARPPacket, * NetARPPacketPtr;

typedef struct
{
	uint8				type;

	uint8				code;

	uint16				checksum;

	uint32				data;
} __attribute__((packed)) NetICMPFrame, * NetICMPFramePtr;

typedef struct
{
	NetEthernetFrame	fr_eth;

	NetIPv4Frame		fr_ipv4;

	NetICMPFrame		fr_icmp;
} __attribute__((packed)) NetICMPPacket, * NetICMPPacketPtr;

extern
BOOL
NetAdd(
	IN NetDevicePtr device);

extern
uint32
NetGetCount(void);

extern
NetDevicePtr
NetGet(
	IN uint32 index);

extern
uint32
NetGetARPRecordCount(void);

extern
NetARPRecordPtr
NetGetARPRecord(
	IN uint32 index);

extern
NetARPRecordPtr
NetFindARPRecord(
	IN uint8 * ip);

extern
void
NetInit(void);

#endif
