/**
	@File:			pcnet2.c
	@Author:		Ihsoh
	@Date:			2017-01-23
	@Description:
		AMD PCnet-PCI II 驱动。
*/

#include "pcnet2.h"
#include "net.h"
#include "endian.h"

#include "../types.h"
#include "../memory.h"
#include "../kernel.h"
#include "../screen.h"

#include "../pci/pci.h"

#define	_NAME		"AMD PCnet-PCI II"

#define	_MAX_COUNT	4
#define	_ID			0x20001022	// Vendor ID = 0x1022，Device ID = 0x2000。

#define	_RX_BUFFER_COUNT		32		// “接收缓冲区”的数量。
#define	_TX_BUFFER_COUNT		8		// “发送缓冲区”的数量。

#define	_BUFFER_SIZE			1548	// 每个“接收缓冲区”和“发送缓冲区”的大小。

#define	_CSR0_INIT		(1 << 0)
#define	_CSR0_STRT		(1 << 1)
#define	_CSR0_STOP		(1 << 2)
#define	_CSR0_TDMD		(1 << 3)
#define	_CSR0_TXON		(1 << 4)
#define	_CSR0_RXON		(1 << 5)
#define	_CSR0_IENA		(1 << 6)
#define	_CSR0_INTR		(1 << 7)
#define	_CSR0_IDON		(1 << 8)
#define	_CSR0_TINT		(1 << 9)
#define	_CSR0_RINT		(1 << 10)
#define	_CSR0_MERR		(1 << 11)
#define	_CSR0_MISS		(1 << 12)
#define	_CSR0_CERR		(1 << 13)
#define	_CSR0_BABL		(1 << 14)
#define	_CSR0_ERR		(1 << 15)

#define	_CSR3_BSWP		(1 << 2)
#define	_CSR3_EMBA		(1 << 3)
#define	_CSR3_DXMT2PD	(1 << 4)
#define	_CSR3_LAPPEN	(1 << 5)
#define	_CSR3_DXSUFLO	(1 << 6)
#define	_CSR3_IDONM		(1 << 8)
#define	_CSR3_TINTM		(1 << 9)
#define	_CSR3_RINTM		(1 << 10)
#define	_CSR3_MERRM		(1 << 11)
#define	_CSR3_MISSM		(1 << 12)
#define	_CSR3_BABLM		(1 << 14)

#define	_CSR15_DRX		(1 << 0)
#define	_CSR15_DTX		(1 << 1)
#define	_CSR15_LOOP		(1 << 2)
#define	_CSR15_DXMTFCS	(1 << 3)
#define	_CSR15_FCOLL	(1 << 4)
#define	_CSR15_DRTY		(1 << 5)
#define	_CSR15_INTL		(1 << 6)
#define	_CSR15_PORTSEL0	(1 << 7)
#define	_CSR15_PORTSEL1	(1 << 8)
#define	_CSR15_LRT		(1 << 9)
#define	_CSR15_TSEL		(1 << 9)
#define	_CSR15_MENDECL	(1 << 10)
#define	_CSR15_DAPC		(1 << 11)
#define	_CSR15_DLNKTST	(1 << 12)
#define	_CSR15_DRCVPA	(1 << 13)
#define	_CSR15_DRCVBC	(1 << 14)
#define	_CSR15_PROM		(1 << 15)

// Initialization Block (SSIZE32 = 1)
typedef struct
{
	// The mode register field of the initialization block is copied into CSR15
	// and interpreted according to the de- scription of CSR15.
	uint16		mode;

	// When SSIZE32 (BCR20, bit 8) is set to ZERO, the software structures are
	// defined to be 16 bits wide, and the RLEN and TLEN fields in the initialization
	// block are each 3 bits wide. The values in these fields determine the number of
	// transmit and receive Descriptor Ring En- tries (DRE) which are used in the
	// descriptor rings. Their meaning is as follows:
	// 
	//		R/TLEN Decoding (SSIZE32 = 0)
	//		R/TLEN		No. of DREs
	//		=============================
	//		000			1
	//		001			2
	//		010			4
	//		011			8
	//		100			16
	//		101			32
	//		110			64
	//		111			128
	//
	// If a value other than those listed in the above table is desired, CSR76 and
	// CSR78 can be written after initialization is complete.
	//
	// When SSIZE32 (BCR20, bit 8) is set to ONE, the software structures are
	// defined to be 32 bits wide, and the RLEN and TLEN fields in the initialization
	// block are each 4 bits wide. The values in these fields determine the number of
	// transmit and receive Descriptor Ring Entries (DRE) which are used in the descriptor
	// rings. Their meaning is as follows:
	//
	//		R/TLEN Decoding (SSIZE32 = 1)
	//		R/TLEN		No. of DREs
	//		=============================
	//		0000		1
	//		0001		2
	//		0010		4
	//		0011		8
	//		0100		16
	//		0101		32
	//		0110		64
	//		0111		128
	//		1000		256
	//		1001		512
	//		11XX		512
	//		1X1X		512
	//
	// If a value other than those listed in the above table is desired, CSR76 and CSR78
	// can be written after initialization is complete.
	uint8		rlen;
	uint8		tlen;

	// This 48-bit value represents the unique node address assigned by the ISO 8802-3 (IEEE/ANSI 802.3)
	// and used for internal address comparison. PADR[0] is compared with the first bit n the destination
	// address of the incoming frame. It must be ZERO since only the destination address of a unicast
	// frames is compared to PADR. The six hex-digit nomenclature used by the ISO 8802-3 (IEEE/ANSI 802.3)
	// maps to the PCnet-PCI II controller PADR register as follows: the first byte is compared with PADR[7:0],
	// with PADR[0] being the least significant bit of the byte. The second ISO 8802-3 (IEEE/ANSI 802.3)
	// byte is compared with PADR[15:8], again from the least significant bit to the most significant bit,
	// and so on. The sixth byte is compared with PADR[47:40], the least significant bit being PADR[40].
	uint8		mac[6];

	// reserved.
	uint16		reserved;

	// The Logical Address Filter (LADRF) is a 64-bit mask that is used to accept incoming Logical Addresses.
	// If the first bit in the incoming address (as transmitted on the wire) is a ONE, it indicates a logical address.
	// If the first bit is a ZERO, it is a physical address and is compared against the physical address that was
	// loaded through the initialization block.
	uint8		ladr[8];

	// TDRA and RDRA indicate where the transmit and receive descriptor rings begin. Each DRE must be
	// located at a 16-byte address boundary when SSIZE32 is set to ONE (BCR20, bit 8). Each DRE must be
	// located at an 8-byte address boundary when SSIZE32 is set to ZERO (BCR20, bit 8).
	uint32		rdes;
	uint32		tdes;
} __attribute__((packed)) PCNet2InitializationBlock, * PCNet2InitializationBlockPtr;

// Receive Descriptor (SWSTYLE = 2)
typedef struct
{
	// Receive Buffer address. This field contains the address of the receive buffer that is associated with this descriptor.
	uint32		rbadr;

	// Buffer Byte Count is the length of the buffer pointed to by this descriptor, expressed as the two’s complement of
	// the length of the buffer. This field is written by the host and unchanged by the PCnet-PCI II controller.
	uint32		bcnt:12;

	// These four bits must be written as ONEs. They are written by the host and unchanged by the PC- net-PCI II controller.
	uint32		ones:4;

	// Reserved locations. These locations should be read and written as ZEROs.
	uint32		res:4;

	// Broadcast Address Match is set by the PCnet-PCI II controller when it accepts the received frame because the frame’s
	// destination address is of the type ‘‘Broadcast’’. BAM is valid only when ENP is set. BAM is set by the PCnet-PCI II
	// controller and cleared by the host.
	//
	// This bit does not exist when the PCnet-PCI II controller is programmed to use 16-bit software structures for the
	// descriptor ring entries (BCR20, bits 7–0, SWSTYLE is cleared to ZERO).
	uint32		bam:1;

	// Logical Address Filter Match is set by the PCnet-PCI II controller when it accepts the received frame based on the
	// value in the logical address filter register. LAFM is valid only when ENP is set. LAFM is set by the PCnet-PCI II
	// controller and cleared by the host.
	//
	// Note that if DRCVBC (CSR15, bit 14) is cleared to ZERO, only BAM, but not LAFM will be set when a Broadcast frame
	// is received, even if the Logical Address Filter is programmed in such a way that a Broadcast frame would pass the
	// hash filter. If DRCVBC is set to ONE and the Logical Address Filter is programmed in such a way that a Broadcast
	// frame would pass the hash filter, LAFM will be set on the reception of a Broadcast frame.
	//
	// This bit does not exist when the PCnet-PCI II controller is programmed to use 16-bit software structures for the
	// descriptor ring entries (BCR20, bits 7–0, SWSTYLE is cleared to ZERO).
	uint32		lafm:1;

	// Physical Address Match is set by the PCnet-PCI II controller when it accepts the received frame due to a match of
	// the frame’s destination address with the content of the physical address register. PAM is valid only when ENP is
	// set. PAM is set by the PCnet-PCI II controller and cleared by the host.
	//
	// This bit does not exist when the PCnet-PCI II controller is programmed to use 16-bit software structures for the
	// descriptor ring entries (BCR20, bits 7–0, SWSTYLE is cleared to ZERO).
	uint32		pam:1;

	// Bus Parity Error is set by the PCnet-PCI II controller when a parity error occurred on the bus interface during
	// a data transfers to a receive buffer. BPE is valid only when ENP, OFLO or BUFF are set. The PCnet-PCI II controller
	// will only set BPE when the advanced parity error handling is enabled by setting APERREN (BCR20, bit 10) to ONE.
	// BPE is set the PCnet-PCI II controller and cleared by the host.
	//
	// This bit does not exist, when the PCnet-PCI II controller is programmed to use 16-bit software structures for the
	// descriptor ring entries (BCR20, bits 7–0, SW- STYLE is cleared to ZERO).
	uint32		bpe:1;

	// End of Packet indicates that this is the last buffer used by the PCnet-PCI II controller for this frame. It is
	// used for data chaining buffers. If both STP and ENP are set, the frame fits into one buffer and there is no data
	// chaining. ENP is set by the PCnet-PCI II controller and cleared by the host.
	uint32		enp:1;

	// Start of Packet indicates that this is the first buffer used by the PCnet-PCI II controller for this frame.
	// If STP and ENP are both set to ONE, the frame fits into a single buffer. Otherwise, the frame is spread over
	// more than one buffer. When LAPPEN (CSR3, bit 5) is cleared to ZERO, STP is set by the PCnet-PCI II controller
	// and cleared by the host. When LAPPEN is set to ONE, STP must be set by the host.
	uint32		stp:1;

	// Buffer error is set any time the PCnet-PCI II controller does not own the next buffer while data chaining a received frame.
	// This can occur in either of two ways:
	//
	//		1. The OWN bit of the next buffer is ZERO.
	//		2. FIFO overflow occurred before the PCnet-PCI II controller was able to read the OWN bit of the next descriptor.
	//
	// If a Buffer Error occurs, an Overflow Error may also occur internally in the FIFO, but will not be reported in the descriptor
	// status entry unless both BUFF and OFLO errors occur at the same time. BUFF is set by the PCnet-PCI II controller and cleared
	// by the host.
	uint32		buff:1;

	// CRC indicates that the receiver has detected a CRC (FCS) error on the incoming frame. CRC is valid only when ENP is set and
	// OFLO is not. CRC is set by the PCnet-PCI II controller and cleared by the host.
	uint32		crc:1;

	// Overflow error indicates that the receiver has lost all or part of the incoming frame, due to an inability to move data
	// from the receive FIFO into a memory buffer before the internal FIFO overflowed. OFLO is valid only when ENP is not set.
	// OFLO is set by the PCnet-PCI II controller and cleared by the host.
	uint32		oflo:1;

	// Framing error indicates that the incoming frame contains a non-integer multiple of eight bits and there was an FCS error.
	// If there was no FCS error on the incoming frame, then FRAM will not be set even if there was a non-integer multiple of
	// eight bits in the frame. FRAM is not valid in internal loopback mode. FRAM is valid only when ENP is set and OFLO is not.
	// FRAM is set by the PCnet-PCI II controller and cleared by the host.
	uint32		fram:1;

	// ERR is the OR of FRAM, OFLO, CRC, BUFF or BPE. ERR is set by the PCnet-PCI II controller and cleared by the host.
	uint32		err:1;

	// This bit indicates whether the descriptor entry is owned by the host (OWN = 0) or by the PCnet-PCI II controller (OWN = 1).
	// The PCnet-PCI II controller clears the OWN bit after filling the buffer that the descriptor points to. The host sets the OWN bit
	// after emptying the buffer. Once the PCnet-PCI II controller or host has relinquished ownership of a buffer, it must not change
	// any field in the descriptor entry.
	uint32		own:1;

	// Message Byte Count is the length in bytes of the received message, expressed as an unsigned binary integer. MCNT is valid only
	// when ERR is clear and ENP is set. MCNT is written by the PCnet-PCI II controller and cleared by the host.
	uint32		mcnt:12;

	// This field is reserved. PCnet-PCI II controller will write ZEROs to these locations.
	uint32		zeros:4;

	// Runt Packet Count. Indicates the accumulated number of runts that were addressed to this node since the last time that a receive
	// packet was successfully received and its corresponding RMD2 ring entry was written to by the PCnet-PCI II controller. In order
	// to be included in the RPC value, a runt must be long enough to meet the minimum requirement of the internal address matching logic.
	// The minimum requirement for a runt to pass the internal address matching mechanism is: 18 bits of valid preamble plus a valid SFD
	// detected, followed by 7 bytes of frame data. This requirement is unvarying, regardless of the address matching mechanisms in
	// force at the time of reception. (I.e. physical, logical, broadcast or promiscuous). The PCnet-PCI II controller implementation
	// of this counter may not be compatible with the ILACC RPC definition.
	uint32		rpc:8;

	// Receive Collision Count. Indi- cates the accumulated number of collisions detected on the net- work since the last packet was
	// received, excluding collisions that occurred during transmissions from this node. The PCnet-PCI II controller implementation of this counter may not be compatible with the ILACC RCC definition. If network statistics are to be moni- tored, then CSR114 should be used for the purpose of monitoring receive collisions instead of these bits.
	uint32		rcc:8;

	// Reserved.
	uint32		reserved;
} __attribute__((packed)) PCNet2ReceiveDescriptorSWS2, * PCNet2ReceiveDescriptorSWS2Ptr;

// Transmit Descriptor (SWSTYLE = 2)
typedef struct
{
	// Transmit Buffer address. This field contains the address of the transmit buffer that is associated with this descriptor.
	uint32		tbadr;

	// Buffer Byte Count is the usable length of the buffer pointed to by this descriptor, expressed as the two’s complement of
	// the length of the buffer. This is the number of bytes from this buffer that will be transmitted by the PCnet-PCI II controller.
	// This field is written by the host and is not changed by the PCnet-PCI II controller. There are no minimum buffer size restrictions.
	uint32		bcnt:12;

	// These four bits must be written as ONEs. This field is written by the host and unchanged by the PCnet-PCI II controller.
	uint32		ones:4;

	// Reserved locations.
	uint32		res0:7;

	// Bus Parity Error is set by the PCnet-PCI II controller when a parity error occurred on the bus interface during a data
	// transfers from the transmit buffer associated with this descriptor. The PCnet-PCI II controller will only set BPE when
	// the advanced parity error handling is enabled by setting APERREN (BCR20, bit 10) to ONE. BPE is set by the PCnet-PCI II
	// controller and cleared by the host.
	//
	// This bit does not exist, when the PCnet-PCI II controller is programmed to use 16-bit software structures for the
	// descriptor ring entries (BCR20, bits 7–0, SWSTYLE is cleared to ZERO).
	uint32		bpe:1;

	// End of Packet indicates that this is the last buffer to be used by the PCnet-PCI II controller for this frame. It is
	// used for data chaining buffers. If both STP and ENP are set, the frame fits into one buffer and there is no data chaining.
	// ENP is set by the host and is not changed by the PCnet-PCI II controller.
	uint32		enp:1;

	// Start of Packet indicates that this is the first buffer to be used by the PCnet-PCI II controller for this frame.
	// It is used for data chaining buffers. The STP bit must be set in the first buffer of the frame, or the PCnet-PCI II
	// controller will skip over the descriptor and poll the next descriptor(s) until the OWN and STP bits are set.
	// STP is set by the host and is not changed by the PCnet-PCI II controller.
	uint32		stp:1;

	// Deferred indicates that the PCnet-PCI II controller had to defer while trying to transmit a frame. This condition
	// occurs if the channel is busy when the PCnet-PCI II controller is ready to transmit. DEF is set by the PCnet-PCI II
	// controller and cleared by the host.
	uint32		def:1;

	// ONE indicates that exactly one retry was needed to transmit a frame. ONE flag is not valid when LCOL is set.
	// The value of the ONE bit is written by the PCnet-PCI II controller. This bit has meaning only if the ENP bit is set.
	uint32		one:1;

	// Bit 28 always function as MORE. The value of MORE is written by the PCnet-PCI II controller and is read by the host.
	// When LTINTEN is cleared to ZERO (CSR5, bit 14), the PCnet-PCI II controller will never look at the content of bit 28,
	// write operations by the host have no effect. When LTINTEN is set to ONE bit 28 changes its function to LTINT on host
	// write operations and on PCnet-PCI II controller read operations.
	//
	//		MORE
	//		MORE indicates that more than one retry was needed to transmit a frame. The value of MORE is written by the
	//		PCnet-PCI II controller. This bit has meaning only if the ENP bit is set.	
	//
	//		LTINT
	//		LTINT is used to suppress interrupts after successful transmission on selected frames. When LTINT is
	//		cleared to ZERO and ENP is set to ONE, the PCnet-PCI II controller will not set TINT (CSR0, bit 9) after
	//		a successful transmission. TINT will only be set when the last descriptor of a frame has both LTINT and
	//		ENP set to ONE. When LTINT is cleared to ZERO, it will only cause the suppression of interrupts for successful
	//		transmission. TINT will always be set if the transmission has an error. The LTINTEN overrides the function of
	//		TOKINTD (CSR5, bit 15).
	uint32		more_ltint:1;

	// Bit 29 functions NO_FCSSWSTYLE as when (BCR20, bits 7–0) is set to ONE (ILACC style). Otherwise bit 29 functions as ADD_FCS.
	//
	//		ADD_FCS
	//		ADD_FCS dynamically controls the generation of FCS on a frame by frame basis. It is valid only if the STP bit is set.
	//		When ADD_FCS is set, the state of DXMTFCS is ignored and transmitter FCS generation is activated. When ADD_FCS is
	//		cleared to ZERO, FCS generation is controlled by DXMTFCS. When APAD_XMT (CSR4, bit 11) is set to ONE, the setting
	//		of ADD_FCS has no effect. ADD_FCS is set by the host, and is not changed by the PCnet-PCI II controller. This is a
	//		reserved bit in the C-LANCE(Am79C90). This function differs from the corresponding ILACC function.
	//
	//		NO_FCS
	//		NO_FCS dynamically controls the generation of FCS on a frame by frame basis. It is valid only if the ENP bit is set.
	//		When NO_FCS is set, the state of DXMTFCS is ignored and transmitter FCS generation is deactivated. When NO_FCS is
	//		cleared to ZERO, FCS generation is controlled by DXMTFCS. When APAD_XMT (CSR4, bit 11) is set to ONE, the setting of
	//		NO_FCS has no effect. NO_FCS is set by the host, and is not changed by the PCnet-PCI II controller. This is a reserved
	//		bit in the C-LANCE (Am79C90). This function is identical to the corresponding ILACC function.
	uint32		add_no_fcs:1;

	// ERR is the OR of UFLO, LCOL, LCAR, RTRY or BPE. ERR is set by the PCnet-PCI II controller and cleared by the host.
	// This bit is set in the current descriptor when the error occurs, and therefore may be set in any descriptor of a
	// chained buffer transmission.
	uint32		err:1;

	// This bit indicates whether the descriptor entry is owned by the host (OWN = 0) or by the PCnet-PCI II controller (OWN = 1).
	// The host sets the OWN bit after filling the buffer pointed to by the descriptor entry. The PCnet-PCI II controller clears the
	// OWN bit after transmitting the contents of the buffer. Both the PCnet-PCI II controller and the host must not alter a descriptor
	// entry after it has relinquished ownership.
	uint32		own:1;

	// Transmit Retry Count. Indicates the number of transmit retries of the associated packet. The maximum count is 15.
	// However, if a RETRY error occurs, the count will roll over to ZERO. In this case only, the Transmit Retry Count value
	// of ZERO should be interpreted as meaning 16. TRC is written by the PCnet-PCI II controller into the last transmit
	// descriptor of a frame, or when an error terminates a frame. Valid only when OWN is cleared to ZERO.
	uint32		trc:4;

	// Reserved locations.
	uint32		res1:8;

	// Time Domain Reflectometer reflects the state of an internal PCnet-PCI II controller counter that counts at a 10 MHz
	// rate from the start of a transmission to the occurrence of a collision or loss of carrier. This value is useful
	// in determining the approximate distance to a cable fault. The TDR value is written by the PCnet-PCI II controller
	// and is valid only if RTRY is set.
	//
	// Note that 10 MHz gives very low resolution and in general has not been found to be particularly useful.
	// This feature is here primarily to maintain full compatibility with the C-LANCE device (Am79C90).
	uint32		tdr:14;

	// Retry error indicates that the transmitter has failed after 16 attempts to successfully transmit a message,
	// due to repeated collisions on the medium. If DRTY is set to ONE in the MODE register, RTRY will set after
	// 1 failed transmission attempt. RTRY is set by the PCnet-PCI II controller and cleared by the host.
	uint32		rtry:1;

	// Loss of Carrier is set when the carrier is lost during a PCnet-PCI II controller-initiated transmission when
	// in AUI mode and the device is operating in half-duplex mode. The PCnet-PCI II controller does not retry upon
	// loss of carrier. It will continue to transmit the whole frame until done. LCAR will not be set when the device
	// is operating in full-duplex mode and the AUI port is active. LCAR is not valid in Internal Loopback Mode.
	// LCAR is set by the PCnet-PCI II controller and cleared by the host.
	//
	// In 10BASE-T mode, LCAR will be set when the T-MAU was in Link Fail state during the transmission.
	uint32		lcar:1;

	// Late Collision indicates that a collision has occurred after the first slot time of the channel has elapsed.
	// The PCnet-PCI II controller does not retry on late collisions. LCOL is set by the PCnet-PCI II controller
	// and cleared by the host.
	uint32		lcol:1;

	// Excessive Deferral. Indicates that the transmitter has experienced Excessive Deferral on this transmit frame,
	// where Excessive Deferral is defined in ISO 8802-3 (IEEE/ANSI 802.3). Excessive Deferral will also set the interrupt
	// bit EXDINT (CSR5, bit 7).
	uint32		exdef:1;

	// Underflow error indicates that the transmitter has truncated a message because it could not read data from memory fast enough.
	// UFLO indicates that the FIFO has emptied before the end of the frame was reached.
	//
	// When DXSUFLO (CSR3, bit 6) is cleared to ZERO, the transmitter is turned off when an UFLO error occurs (CSR0, TXON = 0).
	//
	// When DXSUFLO is set to ONE, the PCnet-PCI II controller gracefully recovers from an UFLO error. It scans the transmit
	// descriptor ring until it finds the start of a new frame and starts a new transmission.
	//
	// UFLO is set by the PCnet-PCI II controller and cleared by the host.
	uint32		uflo:1;

	// Buffer error is set by the PCnet-PCI II controller during transmission when the PCnet-PCI II controller does not find
	// the ENP flag in the current descriptor and does not own the next descriptor. This can occur in either of two ways:
	//
	//		1. The OWN bit of the next de- scriptor is ZERO.
	//		2. FIFO underflow occurred before the PCnet-PCI II controller obtained the ST A- TUS byte (TMD1[31:24]) of the
	//			next descriptor. BUFF is set by the PCnet-PCI II con- troller and cleared by the host.
	//
	// If a Buffer Error occurs, an Underflow Error will also occur. BUFF is not valid when LCOL or RTRY error is set
	// during transmit data chaining. BUFF is set by the PCnet-PCI II controller and cleared by the host.
	uint32		buff:1;

	// Reserved.
	uint32		reserved;
} __attribute__((packed)) PCNet2TransmitDescriptorSWS2, * PCNet2TransmitDescriptorSWS2Ptr;

typedef struct
{
	uint32		index;				// 当为0xffffffff代表设备不存在。

	uint32		iobase;				// NIC的IO BASE。

	uint8		mac[6];				// MAC地址。

	int32		rx_buffer_idx;		// 储存着下一个可用的“接收缓冲区”的索引。
	int32		tx_buffer_idx;		// 储存着下一个可用的“发送缓冲区”的索引。

	uint8 *		rdes;				// 指向第一个“接收缓冲区”的Descriptor Entry。
	uint8 *		tdes;				// 指向第一个“发送缓冲区”的Descriptor Entry。

	uint8 *		rx_buffer;			// 储存着“接收缓冲区”的地址，所有“接收缓冲区”均连续地分布在一块内存块中。
	uint8 *		tx_buffer;			// 储存着“发送缓冲区”的地址，所有“发送缓冲区”均连续地分布在一块内存块中。

	uint8		ip[4];				// IP地址，[0]=最高字节；...；[3]=最低字节。

	PCNet2InitializationBlock	iniblk;
	NetDevicePtr				netdev;
} PCNet2Device, * PCNet2DevicePtr;

static PCNet2Device _devices[_MAX_COUNT];
static uint32 _count = 0;
static uint32 _pidx2didx[MAX_PERIPHERAL_COUNT];

/**
	@Function:		_PCNET2Free
	@Access:		Private
	@Description:
		释放设备所占用的资源。
	@Parameters:
		device, PCNet2DevicePtr, IN OUT
			指向设备实例的指针。
	@Return:
*/
static
void
_PCNET2Free(
	IN OUT PCNet2DevicePtr device)
{
	device->index = 0xffffffff;

	if (device->rdes != NULL)
	{
		DELETE(device->rdes);
		device->rdes = NULL;
	}
	if (device->tdes != NULL)
	{
		DELETE(device->tdes);
		device->tdes = NULL;
	}

	if (device->rx_buffer != NULL)
	{
		DELETE(device->rx_buffer);
		device->rx_buffer = NULL;
	}
	if (device->tx_buffer != NULL)
	{
		DELETE(device->tx_buffer);
		device->tx_buffer = NULL;
	}
}

/**
	@Function:		_PCNET2GetDevice
	@Access:		Private
	@Description:
		获取设备实例。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
		PCNet2DevicePtr
			指向设备实例的指针，返回NULL则设备不存在。
*/
static
PCNet2DevicePtr
_PCNET2GetDevice(
	IN uint32 index)
{
	if (index >= _MAX_COUNT
		|| _devices[index].index == 0xffffffff)
	{
		return NULL;
	}
	return &_devices[index];
}

/**
	@Function:		_PCNET2WriteRAP
	@Access:		Private
	@Description:
		写RAP寄存器。
	@Parameters:
		index, uint32, IN
			设备索引。
		value, uint32, IN
			值。
	@Return:
*/
static
void
_PCNET2WriteRAP(
	IN uint32 index,
	IN uint32 value)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	KnlOutLong(device->iobase + 0x14, value);
}

/**
	@Function:		_PCNET2ReadCSR
	@Access:		Private
	@Description:
		读CSR寄存器。
	@Parameters:
		index, uint32, IN
			设备索引。
		csr, uint32, IN
			CSR寄存器索引。
	@Return:
		uint32
			值。
*/
static
uint32
_PCNET2ReadCSR(
	IN uint32 index,
	IN uint32 csr)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return 0;
	}
	_PCNET2WriteRAP(index, csr);
	return KnlInLong(device->iobase + 0x10);
}

/**
	@Function:		_PCNET2WriteCSR
	@Access:		Private
	@Description:
		写CSR寄存器。
	@Parameters:
		index, uint32, IN
			设备索引。
		csr, uint32, IN
			CSR寄存器索引。
		value, uint32, IN
			值。
	@Return:
*/
static
void
_PCNET2WriteCSR(
	IN uint32 index,
	IN uint32 csr,
	IN uint32 value)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	_PCNET2WriteRAP(index, csr);
	KnlOutLong(device->iobase + 0x10, value);
}

/**
	@Function:		_PCNET2ReadBCR
	@Access:		Private
	@Description:
		读BCR寄存器。
	@Parameters:
		index, uint32, IN
			设备索引。
		bcr, uint32, IN
			BCR寄存器索引。
	@Return:
		uint32
			值。
*/
static
uint32
_PCNET2ReadBCR(
	IN uint32 index,
	IN uint32 bcr)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return 0;
	}
	_PCNET2WriteRAP(index, bcr);
	return KnlInLong(device->iobase + 0x1c);
}

/**
	@Function:		_PCNET2WriteBCR
	@Access:		Private
	@Description:
		写BCR寄存器。
	@Parameters:
		index, uint32, IN
			设备索引。
		bcr, uint32, IN
			BCR寄存器索引。
		value, uint32, IN
			值。
	@Return:
*/
static
void
_PCNET2WriteBCR(
	IN uint32 index,
	IN uint32 bcr,
	IN uint32 value)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	_PCNET2WriteRAP(index, bcr);
	KnlOutLong(device->iobase + 0x1c, value);
}

/**
	@Function:		_PCNET2EncodeRTLen
	@Access:		Private
	@Description:
		根据下表编码：
		IN		OUT
		============
		1		0b0000	
		2		0b0001
		4		0b0010
		8		0b0011
		16		0b0100
		32		0b0101
		64		0b0110
		128		0b0111
		256		0b1000
		512		0b1001
	@Parameters:
		count, uint32, IN
			接收/传输描述符数量。
	@Return:
		uint8
			数量被编码之后的值。
*/
static
uint8
_PCNET2EncodeRTLen(
	IN uint32 count)
{
	switch (count)
	{
		case 1:
			return 0x00;
		case 2:
			return 0x01;
		case 4:
			return 0x02;
		case 8:
			return 0x03;
		case 16:
			return 0x04;
		case 32:
			return 0x05;
		case 64:
			return 0x06;
		case 128:
			return 0x07;
		case 256:
			return 0x08;
		case 512:
			return 0x09;
		default:
			return 0xff;
	}
}

/**
	@Function:		_PCNET2Interrupt
	@Access:		Private
	@Description:
		设备的中断处理程序。
	@Parameters:
		peripheral, uint32, IN
			外部设备索引。
		irq, uint32, IN
			IRQ。
	@Return:
*/
static
void
_PCNET2Interrupt(
	IN uint32 peripheral,
	IN uint32 irq)
{
	uint32 devidx = _pidx2didx[peripheral];
	if (devidx != 0xffffffff)
	{
		PCNet2DevicePtr dev = _PCNET2GetDevice(devidx);
		if (dev != NULL)
		{
			PCNet2ReceiveDescriptorSWS2Ptr rdes = NULL;
			while (!(rdes = (PCNet2ReceiveDescriptorSWS2Ptr)&dev->rdes[sizeof(PCNet2ReceiveDescriptorSWS2) * dev->rx_buffer_idx])->own)
			{
				uint16 len = rdes->mcnt;

				uint8 * buffer = dev->rx_buffer + dev->rx_buffer_idx * _BUFFER_SIZE;

				NetDevicePtr netdev = dev->netdev;
				if (netdev->ProcessPacket != NULL)
				{
					netdev->ProcessPacket(netdev, buffer, len - 4);
				}

				rdes->own = 1;

				if (++dev->rx_buffer_idx == _RX_BUFFER_COUNT)
				{
					dev->rx_buffer_idx = 0;	
				}	
			}
		}

		_PCNET2WriteCSR(devidx, 0, _PCNET2ReadCSR(devidx, 0) | 0x0400);
	}
}

/**
	@Function:		PCNET2GetDeviceCount
	@Access:		Public
	@Description:
		获取设备的数量。
	@Parameters:
	@Return:
		uint32
			设备的数量。
*/
uint32
PCNET2GetDeviceCount(void)
{
	return _count;
}

/**
	@Function:		PCNET2SetMAC
	@Access:		Public
	@Description:
		设置设备的MAC地址。
	@Parameters:
		index, uint32, IN
			设备索引。
		mac, uint8 *, IN
			设备的MAC地址，长度为6字节。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
PCNET2SetMAC(
	IN uint32 index,
	IN uint8 * mac)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	MemCopy(mac, device->mac, sizeof(device->mac));

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		PCNET2GetMAC
	@Access:		Public
	@Description:
		获取设备的MAC地址。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
		uint8 *
			设备的MAC地址，长度为6字节。
*/
uint8 *
PCNET2GetMAC(
	IN uint32 index)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	return device->mac;
err:
	return NULL;
}

/**
	@Function:		PCNET2SetIP
	@Access:		Public
	@Description:
		设置设备的IP地址。
	@Parameters:
		index, uint32, IN
			设备索引。
		ip, uint8 *, IN
			设备的IP地址，长度为4字节。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
PCNET2SetIP(
	IN uint32 index,
	IN uint8 * ip)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	MemCopy(ip, device->ip, sizeof(device->ip));

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		PCNET2GetIP
	@Access:		Public
	@Description:
		获取设备的IP地址。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
		uint8 *
			设备的IP地址，长度为4字节。
*/
uint8 *
PCNET2GetIP(
	IN uint32 index)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	return device->ip;
err:
	return NULL;
}

/**
	@Function:		PCNET2GetName
	@Access:		Public
	@Description:
		获取设备的名称。
	@Parameters:
	@Return:
		CASCTEXT
			设备的名称。
*/
CASCTEXT
PCNET2GetName(void)
{
	return _NAME;
}

/**
	@Function:		PCNET2SendPacket
	@Access:		Public
	@Description:
		发送一个数据包。
	@Parameters:
		index, uint32, IN
			设备索引。
		packet, void *, IN
			指向数据包的指针。
		len, uint16, IN
			要发送的数据包的长度，单位：字节。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
PCNET2SendPacket(
	IN uint32 index,
	IN void * packet,
	IN uint16 len)
{
	// 通过索引获取设备实例。
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	// 获取一个可用的（own = 0）传输描述符（Transmit Descriptor）。
	uint32 idx = device->tx_buffer_idx;
	PCNet2TransmitDescriptorSWS2Ptr tdes = (PCNet2TransmitDescriptorSWS2Ptr)&device->tdes[idx * sizeof(PCNet2TransmitDescriptorSWS2)];
	if (tdes->own)
	{
		goto err;
	}

	// 把需要传输的数据复制到传输缓冲区。
	if (len > _BUFFER_SIZE)
	{
		goto err;
	}
	MemCopy(packet, device->tx_buffer + _BUFFER_SIZE * idx, len);

	// Start of Packet，指定该帧为第一个帧（不支持多帧）。
	tdes->stp = 1;

	// End of Packet，指定该帧为最后一个帧（不支持多帧）。
	tdes->enp = 1;

	// bcnt储存着要传输的数据的长度的补码。
	uint16 bcnt = ~len + 1;
	bcnt &= 0x0fff;
	tdes->bcnt = bcnt;
	tdes->ones = 0x0f;	// 必须全为1。

	// 把传输描述符的所有权归还给设备。
	tdes->own = 1;

	// 把传输缓冲区索引移到下一个可用的索引。
	if (++idx == _TX_BUFFER_COUNT)
	{
		idx = 0;
	}
	device->tx_buffer_idx = idx;

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		PCNET2PrintStatus
	@Access:		Public
	@Description:
		打印设备的状态信息。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
*/
void
PCNET2PrintStatus(
	IN uint32 index)
{
	if (_PCNET2GetDevice(index) == NULL)
	{
		ScrPrintStringToStderrWithProperty(
			"PCNET2 ERROR: The device dose not exists.",
			CC_RED);
		return;
	}
	ScrPrintString("PCNET2:\n");
	#define	_COLOR(n, f) (csr##n & _CSR##n##_##f ? CC_GREEN : CC_RED)
	uint32 csr0 = _PCNET2ReadCSR(index, 0);
	ScrPrintString("CSR0: ");
	ScrPrintStringWithProperty("INIT ", _COLOR(0, INIT));
	ScrPrintStringWithProperty("STRT ", _COLOR(0, STRT));
	ScrPrintStringWithProperty("STOP ", _COLOR(0, STOP));
	ScrPrintStringWithProperty("TDMD ", _COLOR(0, TDMD));
	ScrPrintStringWithProperty("TXON ", _COLOR(0, TXON));
	ScrPrintStringWithProperty("RXON ", _COLOR(0, RXON));
	ScrPrintStringWithProperty("IENA ", _COLOR(0, IENA));
	ScrPrintStringWithProperty("INTR ", _COLOR(0, INTR));
	ScrPrintStringWithProperty("IDON ", _COLOR(0, IDON));
	ScrPrintStringWithProperty("TINT ", _COLOR(0, TINT));
	ScrPrintStringWithProperty("RINT ", _COLOR(0, RINT));
	ScrPrintStringWithProperty("MERR ", _COLOR(0, MERR));
	ScrPrintStringWithProperty("MISS ", _COLOR(0, MISS));
	ScrPrintStringWithProperty("CERR ", _COLOR(0, CERR));
	ScrPrintStringWithProperty("BABL ", _COLOR(0, BABL));
	ScrPrintStringWithProperty("ERR ", _COLOR(0, ERR));
	ScrPrintString("\n");

	uint32 csr3 = _PCNET2ReadCSR(index, 3);
	ScrPrintString("CSR3: ");
	ScrPrintStringWithProperty("BSWP ", _COLOR(3, BSWP));
	ScrPrintStringWithProperty("EMBA ", _COLOR(3, EMBA));
	ScrPrintStringWithProperty("DXMT2PD ", _COLOR(3, DXMT2PD));
	ScrPrintStringWithProperty("LAPPEN ", _COLOR(3, LAPPEN));
	ScrPrintStringWithProperty("DXSUFLO ", _COLOR(3, DXSUFLO));
	ScrPrintStringWithProperty("IDONM ", _COLOR(3, IDONM));
	ScrPrintStringWithProperty("TINTM ", _COLOR(3, TINTM));
	ScrPrintStringWithProperty("RINTM ", _COLOR(3, RINTM));
	ScrPrintStringWithProperty("MERRM ", _COLOR(3, MERRM));
	ScrPrintStringWithProperty("MISSM ", _COLOR(3, MISSM));
	ScrPrintStringWithProperty("BABLM ", _COLOR(3, BABLM));
	ScrPrintString("\n");

	uint32 csr15 = _PCNET2ReadCSR(index, 15);
	ScrPrintString("CSR15: ");
	ScrPrintStringWithProperty("DRX ", _COLOR(15, DRX));
	ScrPrintStringWithProperty("DTX ", _COLOR(15, DTX));
	ScrPrintStringWithProperty("LOOP ", _COLOR(15, LOOP));
	ScrPrintStringWithProperty("DXMTFCS ", _COLOR(15, DXMTFCS));
	ScrPrintStringWithProperty("FCOLL ", _COLOR(15, FCOLL));
	ScrPrintStringWithProperty("DRTY ", _COLOR(15, DRTY));
	ScrPrintStringWithProperty("INTL ", _COLOR(15, INTL));
	ScrPrintStringWithProperty("PORTSEL0 ", _COLOR(15, PORTSEL0));
	ScrPrintStringWithProperty("PORTSEL1 ", _COLOR(15, PORTSEL1));
	ScrPrintStringWithProperty("LRT ", _COLOR(15, LRT));
	ScrPrintStringWithProperty("TSEL ", _COLOR(15, TSEL));
	ScrPrintStringWithProperty("MENDECL ", _COLOR(15, MENDECL));
	ScrPrintStringWithProperty("DAPC ", _COLOR(15, DAPC));
	ScrPrintStringWithProperty("DLNKTST ", _COLOR(15, DLNKTST));
	ScrPrintStringWithProperty("DRCVPA ", _COLOR(15, DRCVPA));
	ScrPrintStringWithProperty("DRCVBC ", _COLOR(15, DRCVBC));
	ScrPrintStringWithProperty("PROM ", _COLOR(15, PROM));
	ScrPrintString("\n");
	#undef	_COLOR
}

/**
	@Function:		_NetSetIP
	@Access:		Private
	@Description:
		PCNET2SetIP的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
		ip, uint8 *, IN
			见PCNET2SetIP。
	@Return:
		BOOL
			见PCNET2SetIP。
*/
static BOOL _NetSetIP(IN void * device, IN uint8 * ip)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2SetIP(netdev->id, ip);
}

/**
	@Function:		_NetGetIP
	@Access:		Private
	@Description:
		PCNET2GetIP的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
	@Return:
		uint8 *
			见PCNET2GetIP。
*/
static uint8 * _NetGetIP(IN void * device)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2GetIP(netdev->id);
}

/**
	@Function:		_NetSetMAC
	@Access:		Private
	@Description:
		PCNET2SetMAC的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
		mac, uint8 *, IN
			见PCNET2SetMAC。
	@Return:
		BOOL
			见PCNET2SetMAC。
*/
static BOOL _NetSetMAC(IN void * device, IN uint8 * mac)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2SetMAC(netdev->id, mac);
}

/**
	@Function:		_NetGetMAC
	@Access:		Private
	@Description:
		PCNET2GetMAC的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
	@Return:
		uint8 *
			见PCNET2GetMAC。
*/
static uint8 * _NetGetMAC(IN void * device)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2GetMAC(netdev->id);
}

/**
	@Function:		_NetGetName
	@Access:		Private
	@Description:
		PCNET2GetName的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
	@Return:
		CASCTEXT
			见PCNET2GetName。
*/
static CASCTEXT _NetGetName(IN void * device)
{
	return PCNET2GetName();
}

/**
	@Function:		_NetSendPacket
	@Access:		Private
	@Description:
		PCNET2SendPacket的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
		packet, void *, IN
			见PCNET2SendPacket。
		len, uint16, IN
			见PCNET2SendPacket。
	@Return:
		BOOL
			见PCNET2SendPacket。
*/
static BOOL _NetSendPacket(IN void * device, IN void * packet, IN uint16 len)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2SendPacket(netdev->id, packet, len);
}

/**
	@Function:		PCNET2Init
	@Access:		Public
	@Description:
		初始化所有AMD PCnet-PCI II设备。
	@Parameters:
	@Return:
*/
void
PCNET2Init(void)
{
	uint32 i;
	for (i = 0; i < _MAX_COUNT; i++)
	{
		_devices[i].index = 0xffffffff;
	}
	for (i = 0; i < MAX_PERIPHERAL_COUNT; i++)
	{
		_pidx2didx[i] = 0xffffffff;
	}
	uint32 c = PciGetDeviceCount();
	for (i = 0; i < c && _count < _MAX_COUNT; i++)
	{
		PCIDeviceInfoPtr dev = PciGetDevice(i);
		if (dev != NULL
			&& dev->header->cfg_hdr.id == _ID)
		{
			uint32 devidx = _count++;
			PCNet2DevicePtr device = &_devices[devidx];
			device->index = i;
			device->iobase = dev->header->cfg_hdr.bars[0] & 0xfffffffe;

			// 构造NetDevice对象。
			NetDevicePtr netdev = NEW(NetDevice);
			netdev->id = devidx;
			netdev->SetIP = _NetSetIP;
			netdev->GetIP = _NetGetIP;
			netdev->SetMAC = _NetSetMAC;
			netdev->GetMAC = _NetGetMAC;
			netdev->GetName = _NetGetName;
			netdev->SendPacket = _NetSendPacket;
			NetAdd(netdev);
			device->netdev = netdev;

			// 开启IO端口以及Bus Mastering。
			uint32 conf = PciReadCfgRegValue(devidx, 0x04);
			conf &= 0xffff0000;
			conf |= 0x5;
			PciWriteCfgRegValue(devidx, 0x04, conf);

			// 获取MAC地址。
			device->mac[5] = KnlInByte(device->iobase + 5);
			device->mac[4] = KnlInByte(device->iobase + 4);
			device->mac[3] = KnlInByte(device->iobase + 3);
			device->mac[2] = KnlInByte(device->iobase + 2);
			device->mac[1] = KnlInByte(device->iobase + 1);
			device->mac[0] = KnlInByte(device->iobase + 0);

			// 设置初始IP地址。
			MemClear(device->ip, 0xff, sizeof(device->ip));

			// 启用32位模式。
			KnlOutLong(device->iobase + 0x10, 0);

			// SWSTYLE。
			uint32 csr58 = _PCNET2ReadCSR(devidx, 58);
			csr58 &= 0xfff0;
			csr58 |= 2;
			_PCNET2WriteCSR(devidx, 58, csr58);

			// ASEL。
			uint32 bcr2 = _PCNET2ReadBCR(devidx, 2);
			bcr2 |= 0x2;
			_PCNET2WriteBCR(devidx, 2, bcr2);

			int32 j;
			uint16 bcnt = (uint16) -_BUFFER_SIZE;
			bcnt &= 0x0fff;
			bcnt |= 0xf000;

			// 初始化“接收缓冲区”。
			device->rx_buffer_idx = 0;
			device->rx_buffer = (uint8 *) MemAlloc(_BUFFER_SIZE * _RX_BUFFER_COUNT);
			device->rdes = (uint8 *) MemAlloc(sizeof(PCNet2ReceiveDescriptorSWS2) * _RX_BUFFER_COUNT);
			MemClear(device->rdes, 0, sizeof(PCNet2ReceiveDescriptorSWS2) * _RX_BUFFER_COUNT);
			for (j = 0; j < _RX_BUFFER_COUNT; j++)
			{
				PCNet2ReceiveDescriptorSWS2Ptr rdes = (PCNet2ReceiveDescriptorSWS2Ptr)&device->rdes[j * sizeof(PCNet2ReceiveDescriptorSWS2)];
				rdes->rbadr = (uint32) (device->rx_buffer + j * _BUFFER_SIZE);
				rdes->bcnt = bcnt;
				rdes->ones = 0x0f;	// 必须全为1。
				rdes->own = 1;
			}

			// 初始化“发送缓冲区”。
			device->tx_buffer_idx = 0;
			device->tx_buffer = (uint8 *) MemAlloc(_BUFFER_SIZE * _TX_BUFFER_COUNT);
			device->tdes = (uint8 *) MemAlloc(sizeof(PCNet2TransmitDescriptorSWS2) * _TX_BUFFER_COUNT);
			MemClear(device->tdes, 0, sizeof(PCNet2TransmitDescriptorSWS2) * _TX_BUFFER_COUNT);
			for (j = 0; j < _TX_BUFFER_COUNT; j++)
			{
				PCNet2TransmitDescriptorSWS2Ptr tdes = (PCNet2TransmitDescriptorSWS2Ptr)&device->tdes[j * sizeof(PCNet2TransmitDescriptorSWS2)];
				tdes->tbadr = (uint32) (device->tx_buffer + j * _BUFFER_SIZE);
				tdes->bcnt = bcnt;
				tdes->ones = 0x0f;	// 必须全为1。
				tdes->own = 0;
			}

			// 设置中断处理程序。
			uint32 irq = dev->header->cfg_hdr.intr & 0xff;
			uint32 pindex = KnlConvertIrqToPeripheralIndex(irq);
			_pidx2didx[pindex] = devidx;
			KnlSetPeripheralInterrupt(pindex, _PCNET2Interrupt);

			// 设置NIC。
			uint32 csr3 = _PCNET2ReadCSR(devidx, 3);
			csr3 &= 0xfffff8fb;
			_PCNET2WriteCSR(devidx, 3, csr3);

			uint32 csr4 = _PCNET2ReadCSR(devidx, 4);
			csr4 |= (1 << 11);
			_PCNET2WriteCSR(devidx, 4, csr4);

			PCNet2InitializationBlockPtr iniblk = &device->iniblk;
			iniblk->mode = 0;
			iniblk->rlen = _PCNET2EncodeRTLen(_RX_BUFFER_COUNT) << 4;
			iniblk->tlen = _PCNET2EncodeRTLen(_TX_BUFFER_COUNT) << 4;
			MemCopy(device->mac, iniblk->mac, 6);
			iniblk->reserved = 0;
			MemClear(iniblk->ladr, 0, 8);
			iniblk->rdes = (uint32) device->rdes;
			iniblk->tdes = (uint32) device->tdes;
			uint32 iadr = (uint32) iniblk;
			_PCNET2WriteCSR(devidx, 1, iadr & 0x0000ffff);
			_PCNET2WriteCSR(devidx, 2, (iadr >> 16) & 0x0000ffff);
			uint32 csr0 = _PCNET2ReadCSR(devidx, 0);
			csr0 &= 0xfffffff8;
			csr0 |= 0x00000043;
			_PCNET2WriteCSR(devidx, 0, csr0);
		}
	}
}
