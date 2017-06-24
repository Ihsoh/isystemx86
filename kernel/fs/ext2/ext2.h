/**
	@File:			ext2.h
	@Author:		Ihsoh
	@Date:			2017-6-3
	@Description:
		EXT2文件系统。
*/

#ifndef	_FS_EXT2_EXT2_H_
#define	_FS_EXT2_EXT2_H_

#include "../../types.h"

#define	EXT2_VALID_FS	1	// Unmounted cleanly
#define	EXT2_ERROR_FS	2	// Errors detected

#define	EXT2_ERRORS_CONTINUE	1	// continue as if nothing happened
#define	EXT2_ERRORS_RO			2	// remount read-only
#define	EXT2_ERRORS_PANIC		3	// cause a kernel panic

#define	EXT2_OS_LINUX		0	// Linux
#define	EXT2_OS_HURD		1	// GNU HURD
#define	EXT2_OS_MASIX		2	// MASIX
#define	EXT2_OS_FREEBSD		3	// FreeBSD
#define	EXT2_OS_LITES		4	// Lites

#define	EXT2_GOOD_OLD_REV	0	// Revision 0
#define	EXT2_DYNAMIC_REV	1	// Revision 1 with variable inode sizes, extended attributes, etc.

#define	EXT2_DEF_RESUID		0

#define	EXT2_DEF_RESGID		0

#define	EXT2_GOOD_OLD_FIRST_INO		11

#define	EXT2_GOOD_OLD_INODE_SIZE	128

#define	EXT2_FEATURE_COMPAT_DIR_PREALLOC		0x0001	// Block pre-allocation for new directories
#define	EXT2_FEATURE_COMPAT_IMAGIC_INODES		0x0002
#define	EXT3_FEATURE_COMPAT_HAS_JOURNAL			0x0004	// An Ext3 journal exists
#define	EXT2_FEATURE_COMPAT_EXT_ATTR			0x0008	// Extended inode attributes are present
#define	EXT2_FEATURE_COMPAT_RESIZE_INO			0x0010	// Non-standard inode size used
#define	EXT2_FEATURE_COMPAT_DIR_INDEX			0x0020	// Directory indexing (HTree)

#define	EXT2_FEATURE_INCOMPAT_COMPRESSION		0x0001	// Disk/File compression is used
#define	EXT2_FEATURE_INCOMPAT_FILETYPE			0x0002
#define	EXT3_FEATURE_INCOMPAT_RECOVER			0x0004
#define	EXT3_FEATURE_INCOMPAT_JOURNAL_DEV		0x0008
#define	EXT2_FEATURE_INCOMPAT_META_BG			0x0010

#define	EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER		0x0001	// Sparse Superblock
#define	EXT2_FEATURE_RO_COMPAT_LARGE_FILE		0x0002	// Large file support, 64-bit file size
#define	EXT2_FEATURE_RO_COMPAT_BTREE_DIR		0x0004	// Binary tree sorted directory files

#define	EXT2_LZV1_ALG		0x00000001
#define	EXT2_LZRW3A_ALG		0x00000002
#define	EXT2_GZIP_ALG		0x00000004
#define	EXT2_BZIP2_ALG		0x00000008
#define	EXT2_LZO_ALG		0x00000010






typedef struct
{
	/*
		32bit value indicating the total number of inodes, both used and free, in the file system. This value must
		be lower or equal to (s_inodes_per_group * number of block groups). It must be equal to the sum of the
		inodes defined in each block group.
	*/
	uint32		s_inodes_count;

	/*
		32bit value indicating the total number of blocks in the system including all used, free and reserved. This
		value must be lower or equal to (s_blocks_per_group * number of block groups). It must be equal to the
		sum of the blocks defined in each block group.
	*/
	uint32		s_blocks_count;

	/*
		32bit value indicating the total number of blocks reserved for the usage of the super user. This is most
		useful if for some reason a user, maliciously or not, fill the file system to capacity; the super user will
		have this specified amount of free blocks at his disposal so he can edit and save configuration files.
	*/
	uint32		s_r_blocks_count;

	/*
		32bit value indicating the total number of free blocks, including the number of reserved blocks (see
		s_r_blocks_count). This is a sum of all free blocks of all the block groups.
	*/
	uint32		s_free_blocks_count;

	/*
		32bit value indicating the total number of free inodes. This is a sum of all free inodes of all the block
		groups.
	*/
	uint32		s_free_inodes_count;

	/*
		32bit value identifying the first data block, in other word the id of the block containing the superblock
		structure.

		Note that this value is always 0 for file systems with a block size larger than 1KB, and always 1 for file
		systems with a block size of 1KB. The superblock is always starting at the 1024th byte of the disk, which
		normally happens to be the first byte of the 3rd sector.
	*/
	uint32		s_first_data_block;

	/*
		The block size is computed using this 32bit value as the number of bits to shift left the value 1024. This
		value may only be positive.

		block size = 1024 << s_log_block_size;

		Common block sizes include 1KiB, 2KiB, 4KiB and 8Kib. For information about the impact of selecting
		a block size, see Impact of Block Sizes.

			Note: In Linux, at least up to 2.6.28, the block size must be at least as large as the sector size of the
			block device, and cannot be larger than the supported memory page of the architecture.
	*/
	uint32		s_log_block_size;

	/*
		The fragment size is computed using this 32bit value as the number of bits to shift left the value 1024.
		Note that a negative value would shift the bit right rather than left.

		if( positive )
			fragmnet size = 1024 << s_log_frag_size;
		else
			framgnet size = 1024 >> -s_log_frag_size;

			Note: As of Linux 2.6.28 no support exists for an Ext2 partition with fragment size smaller than the
			block size, as this feature seems to not be available.
	*/
	uint32		s_log_frag_size;

	/*
		32bit value indicating the total number of blocks per group. This value in combination with
		s_first_data_block can be used to determine the block groups boundaries.
	*/
	uint32		s_blocks_per_group;

	/*
		32bit value indicating the total number of fragments per group. It is also used to determine the size of the
		block bitmap of each block group.
	*/
	uint32		s_frags_per_group;

	/*
		32bit value indicating the total number of inodes per group. This is also used to determine the size of the
		inode bitmap of each block group. Note that you cannot have more than (block size in bytes * 8) inodes
		per group as the inode bitmap must fit within a single block. This value must be a perfect multiple of the
		number of inodes that can fit in a block ((1024<<s_log_block_size)/s_inode_size).
	*/
	uint32		s_inodes_per_group;

	/*
		Unix time, as defined by POSIX, of the last time the file system was mounted.
	*/
	uint32		s_mtime;

	/*
		Unix time, as defined by POSIX, of the last write access to the file system.
	*/
	uint32		s_wtime;

	/*
		32bit value indicating how many time the file system was mounted since the last time it was fully
		verified.
	*/
	uint16		s_mnt_count;

	/*
		32bit value indicating the maximum number of times that the file system may be mounted before a full
		check is performed.
	*/
	uint16		s_max_mnt_count;

	/*
		16bit value identifying the file system as Ext2. The value is currently fixed to EXT2_SUPER_MAGIC of
		value 0xEF53.
	*/
	uint16		s_magic;

	/*
		16bit value indicating the file system state. When the file system is mounted, this state is set to
		EXT2_ERROR_FS. After the file system was cleanly unmounted, this value is set to EXT2_VALID_FS.

		When mounting the file system, if a valid of EXT2_ERROR_FS is encountered it means the file system
		was not cleanly unmounted and most likely contain errors that will need to be fixed. Typically under
		Linux this means running fsck.

		Defined s_state Values
		Constant Name		Value		Description
		-------------------------------------------------
		EXT2_VALID_FS		1			Unmounted cleanly
		EXT2_ERROR_FS 		2			Errors detected
	*/
	uint16		s_state;

	/*
		16bit value indicating what the file system driver should do when an error is detected. The following
		values have been defined:

		Defined s_errors Values
		Constant Name				Value		Description
		-----------------------------------------------------------------------
		EXT2_ERRORS_CONTINUE		1			continue as if nothing happened
		EXT2_ERRORS_RO				2			remount read-only
		EXT2_ERRORS_PANIC			3			cause a kernel panic
	*/
	uint16		s_errors;

	/*
		16bit value identifying the minor revision level within its revision level.
	*/
	uint16		s_minor_rev_level;

	/*
		Unix time, as defined by POSIX, of the last file system check.
	*/
	uint32		s_lastcheck;

	/*
		Maximum Unix time interval, as defined by POSIX, allowed between file system checks.
	*/
	uint32		s_checkinterval;

	/*
		32bit identifier of the os that created the file system. Defined values are:

		Defined s_creator_os Values
		Constant Name				Value		Description
		---------------------------------------------------
		EXT2_OS_LINUX				0			Linux
		EXT2_OS_HURD				1			GNU HURD
		EXT2_OS_MASIX				2			MASIX
		EXT2_OS_FREEBSD				3			FreeBSD
		EXT2_OS_LITES				4			Lites
	*/
	uint32		s_creator_os;

	/*
		32bit revision level value.

		Defined s_rev_level Values
		Constant Name				Value		Description
		---------------------------------------------------
		EXT2_GOOD_OLD_REV			0			Revision 0
		EXT2_DYNAMIC_REV			1			Revision 1 with variable inode sizes,
												extended attributes, etc.
	*/
	uint32		s_rev_level;

	/*
		16bit value used as the default user id for reserved blocks.

			Note: In Linux this defaults to EXT2_DEF_RESUID of 0.
	*/
	uint16		s_def_resuid;

	/*
		16bit value used as the default group id for reserved blocks.

			Note: In Linux this defaults to EXT2_DEF_RESGID of 0.
	*/
	uint16		s_def_resgid;

	/*
		-- EXT2_DYNAMIC_REV Specific --
	*/

	/*
		32bit value used as index to the first inode useable for standard files. In revision 0, the first non-reserved
		inode is fixed to 11 (EXT2_GOOD_OLD_FIRST_INO). In revision 1 and later this value may be set to any
		value.
	*/
	uint32		s_first_ino;

	/*
		16bit value indicating the size of the inode structure. In revision 0, this value is always 128
		(EXT2_GOOD_OLD_INODE_SIZE). In revision 1 and later, this value must be a perfect power of 2 and
		must be smaller or equal to the block size (1<<s_log_block_size).
	*/
	uint16		s_inode_size;

	/*
		16bit value used to indicate the block group number hosting this superblock structure. This can be used
		to rebuild the file system from any superblock backup.
	*/
	uint16		s_block_group_nr;

	/*
		32bit bitmask of compatible features. The file system implementation is free to support them or not
		without risk of damaging the meta-data.

		Defined s_feature_compat Values
		Constant Name							Value		Description
		EXT2_FEATURE_COMPAT_DIR_PREALLOC		0x0001		Block pre-allocation for new directories
		EXT2_FEATURE_COMPAT_IMAGIC_INODES		0x0002
		EXT3_FEATURE_COMPAT_HAS_JOURNAL			0x0004		An Ext3 journal exists
		EXT2_FEATURE_COMPAT_EXT_ATTR			0x0008		Extended inode attributes are present
		EXT2_FEATURE_COMPAT_RESIZE_INO			0x0010		Non-standard inode size used
		EXT2_FEATURE_COMPAT_DIR_INDEX			0x0020		Directory indexing (HTree)
	*/
	uint32		s_feature_compat;

	/*
		32bit bitmask of incompatible features. The file system implementation should refuse to mount the file
		system if any of the indicated feature is unsupported.

		An implementation not supporting these features would be unable to properly use the file system. For
		example, if compression is being used and an executable file would be unusable after being read from the
		disk if the system does not know how to uncompress it.

		Defined s_feature_incompat Values
		Constant Name							Value		Description
		---------------------------------------------------------------------------------
		EXT2_FEATURE_INCOMPAT_COMPRESSION		0x0001		Disk/File compression is used
		EXT2_FEATURE_INCOMPAT_FILETYPE			0x0002
		EXT3_FEATURE_INCOMPAT_RECOVER			0x0004
		EXT3_FEATURE_INCOMPAT_JOURNAL_DEV		0x0008
		EXT2_FEATURE_INCOMPAT_META_BG			0x0010
	*/
	uint32		s_feature_incompat;

	/*
		32bit bitmask of “read-only” features. The file system implementation should mount as read-only if any
		of the indicated feature is unsupported.

		Defined s_feature_ro_compat Values
		Constant Name							Value		Description
		---------------------------------------------------------------------
		EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER		0x0001		Sparse Superblock
		EXT2_FEATURE_RO_COMPAT_LARGE_FILE		0x0002		Large file support, 64-bit file size
		EXT2_FEATURE_RO_COMPAT_BTREE_DIR		0x0004		Binary tree sorted directory files
	*/
	uint32		s_feature_ro_compat;

	/*
		128bit value used as the volume id. This should, as much as possible, be unique for each file system
		formatted.
	*/
	uint8		s_uuid[16];

	/*
		16 bytes volume name, mostly unusued. A valid volume name would consist of only ISO-Latin-1
		characters and be 0 terminated.
	*/
	uint8		s_volume_name[16];

	/*
		64 bytes directory path where the file system was last mounted. While not normally used, it could serve
		for auto-finding the mountpoint when not indicated on the command line. Again the path should be zero
		terminated for compatibility reasons. Valid path is constructed from ISO-Latin-1 characters.
	*/
	uint8		s_last_mounted[64];

	/*
		32bit value used by compression algorithms to determine the compression method(s) used.

			Note: Compression is supported in Linux 2.4 and 2.6 via the e2compr patch. For more information,
			visit http://e2compr.sourceforge.net/

		Defined s_algo_bitmap Values
		Constant Name							Value		Description
		---------------------------------------------------------------------
		EXT2_LZV1_ALG							0			Binary value of 0x00000001
		EXT2_LZRW3A_ALG							1			Binary value of 0x00000002
		EXT2_GZIP_ALG							2			Binary value of 0x00000004
		EXT2_BZIP2_ALG							3			Binary value of 0x00000008
		EXT2_LZO_ALG							4			Binary value of 0x00000010
	*/
	uint32		s_algo_bitmap;

	/*
		-- Performance Hints --
	*/

	/*
		8-bit value representing the number of blocks the implementation should attempt to pre-allocate when
		creating a new regular file.

		Linux 2.6.28 will only perform pre-allocation using Ext4 although no problem is expected if any version
		of Linux encounters a file with more blocks present than required.
	*/
	uint8		s_prealloc_blocks;

	/*
		8-bit value representing the number of blocks the implementation should attempt to pre-allocate when
		creating a new directory.

		Linux 2.6.28 will only perform pre-allocation using Ext4 and only if the
		EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is present. Since Linux does not de-allocate blocks from
		directories after they were allocated, it should be safe to perform pre-allocation and maintain
		compatibility with Linux.
	*/
	uint8		s_prealloc_dir_blocks;

	uint16		_alignment;

	/*
		-- Journaling Support --
	*/

	/*
		16-byte value containing the uuid of the journal superblock. See Ext3 Journaling for more information.
	*/
	uint8		s_journal_uuid[16];

	/*
		32-bit inode number of the journal file. See Ext3 Journaling for more information.
	*/
	uint32		s_journal_inum;

	/*
		32-bit device number of the journal file. See Ext3 Journaling for more information.
	*/
	uint32		s_journal_dev;

	/*
		32-bit inode number, pointing to the first inode in the list of inodes to delete. See Ext3 Journaling for
		more information.
	*/
	uint32		s_last_orphan;

	/*
		-- Directory Indexing Support --
	*/

	/*
		An array of 4 32bit values containing the seeds used for the hash algorithm for directory indexing.
	*/
	uint32		s_hash_seed[4];

	/*
		An 8bit value containing the default hash version used for directory indexing.
	*/
	uint8		s_def_hash_version;

	uint8		_padding[3];

	/*
		-- Other options --
	*/

	/*
		A 32bit value containing the default mount options for this file system. TODO: Add more information
		here!
	*/
	uint32		s_default_mount_options;

	/*
		A 32bit value indicating the block group ID of the first meta block group. TODO: Research if this is an
		Ext3-only extension.
	*/
	uint32		s_first_meta_bg;

	uint8		_unused[760];

} __attribute__((packed)) Ext2SuperBlock, * Ext2SuperBlockPtr;

typedef struct
{
	/*
		32bit block id of the first block of the “block bitmap” for the group represented.

		The actual block bitmap is located within its own allocated blocks starting at the block ID specified by
		this value.
	*/
	uint32		bg_block_bitmap;

	/*
		32bit block id of the first block of the “inode bitmap” for the group represented.
	*/
	uint32		bg_inode_bitmap;

	/*
		32bit block id of the first block of the “inode table” for the group represented.
	*/
	uint32		bg_inode_table;

	/*
		16bit value indicating the total number of free blocks for the represented group.
	*/
	uint16		bg_free_blocks_count;

	/*
		16bit value indicating the total number of free inodes for the represented group.
	*/
	uint16		bg_free_inodes_count;

	/*
		16bit value indicating the number of inodes allocated to directories for the represented group.
	*/
	uint16		bg_used_dirs_count;

	/*
		16bit value used for padding the structure on a 32bit boundary.
	*/
	uint16		bg_pad;

	/*
		12 bytes of reserved space for future revisions.
	*/
	uint8		bg_reserved[12];
} __attribute__((packed)) Ext2BlockGroupDescriptor, * Ext2BlockGroupDescriptorPtr;

typedef struct
{
	/*
		16bit value used to indicate the format of the described file and the access rights. Here are the possible
		values, which can be combined in various ways:

		Defined i_mode Values
		Constant Name							Value		Description
		---------------------------------------------------------------------
		-- file format --
		EXT2_S_IFSOCK							0xC000		socket
		EXT2_S_IFLNK							0xA000		symbolic link
		EXT2_S_IFREG							0x8000		regular file
		EXT2_S_IFBLK							0x6000		block device
		EXT2_S_IFDIR							0x4000		directory
		EXT2_S_IFCHR							0x2000		character device
		EXT2_S_IFIFO							0x1000		fifo
		-- process execution user/group override --
		EXT2_S_ISUID							0x0800		Set process User ID
		EXT2_S_ISGID							0x0400		Set process Group ID
		EXT2_S_ISVTX							0x0200		sticky bit
		-- access rights --
		EXT2_S_IRUSR							0x0100		user read
		EXT2_S_IWUSR							0x0080		user write
		EXT2_S_IXUSR							0x0040		user execute
		EXT2_S_IRGRP							0x0020		group read
		EXT2_S_IWGRP							0x0010		group write
		EXT2_S_IXGRP							0x0008		group execute
		EXT2_S_IROTH							0x0004		others read
		EXT2_S_IWOTH							0x0002		others write
		EXT2_S_IXOTH							0x0001		others execute
	*/
	uint16		i_mode;

	/*
		16bit user id associated with the file.
	*/
	uint16		i_uid;

	/*
		In revision 0, (signed) 32bit value indicating the size of the file in bytes. In revision 1 and later revisions,
		and only for regular files, this represents the lower 32-bit of the file size; the upper 32-bit is located in the
		i_dir_acl.
	*/
	uint32		i_size;

	/*
		32bit value representing the number of seconds since january 1st 1970 of the last time this inode was
		accessed.
	*/
	uint32		i_atime;

	/*
		32bit value representing the number of seconds since january 1st 1970, of when the inode was created.
	*/
	uint32		i_ctime;

	/*
		32bit value representing the number of seconds since january 1st 1970, of the last time this inode was
		modified.
	*/
	uint32		i_mtime;

	/*
		32bit value representing the number of seconds since january 1st 1970, of when the inode was deleted.
	*/
	uint32		i_dtime;

	/*
		16bit value of the POSIX group having access to this file.
	*/
	uint16		i_gid;

	/*
		16bit value indicating how many times this particular inode is linked (referred to). Most files will have a
		link count of 1. Files with hard links pointing to them will have an additional count for each hard link.
	
		Symbolic links do not affect the link count of an inode. When the link count reaches 0 the inode and all
		its associated blocks are freed.
	*/
	uint16		i_links_count;

	/*
		32-bit value representing the total number of 512-bytes blocks reserved to contain the data of this inode,
		regardless if these blocks are used or not. The block numbers of these reserved blocks are contained in
		the i_block array.

		Since this value represents 512-byte blocks and not file system blocks, this value should not be directly
		used as an index to the i_block array. Rather, the maximum index of the i_block array should be
		computed from i_blocks / ((1024<<s_log_block_size)/512), or once simplified,
		i_blocks/(2<<s_log_block_size).
	*/
	uint32		i_blocks;

	/*
		32bit value indicating how the ext2 implementation should behave when accessing the data for this inode.

		Defined i_flags Values
		Constant Name							Value		Description
		---------------------------------------------------------------------
		EXT2_SECRM_FL							0x00000001	secure deletion
		EXT2_UNRM_FL							0x00000002	record for undelete
		EXT2_COMPR_FL							0x00000004	compressed file
		EXT2_SYNC_FL							0x00000008	synchronous updates
		EXT2_IMMUTABLE_FL						0x00000010	immutable file
		EXT2_APPEND_FL							0x00000020	append only
		EXT2_NODUMP_FL							0x00000040	do not dump/delete file
		EXT2_NOATIME_FL							0x00000080	do not update .i_atime
		-- Reserved for compression usage --
		EXT2_DIRTY_FL							0x00000100	Dirty (modified)
		EXT2_COMPRBLK_FL						0x00000200	compressed blocks
		EXT2_NOCOMPR_FL							0x00000400	access raw compressed data
		EXT2_ECOMPR_FL							0x00000800	compression error
		-- End of compression flags --
		EXT2_BTREE_FL							0x00001000	b-tree format directory
		EXT2_INDEX_FL							0x00001000	hash indexed directory
		EXT2_IMAGIC_FL							0x00002000	AFS directory
		EXT3_JOURNAL_DATA_FL					0x00004000	journal file data
		EXT2_RESERVED_FL						0x80000000	reserved for ext2 library
	*/
	uint32		i_flags;

	/*
		32bit OS dependant value.

		Hurd: 32bit value labeled as “translator”.
		Linux: 32bit value currently reserved.
		Masix: 32bit value currently reserved.
	*/
	uint32		i_osd1;

	/*
		15 x 32bit block numbers pointing to the blocks containing the data for this inode. The first 12 blocks are
		direct blocks. The 13th entry in this array is the block number of the first indirect block; which is a block
		containing an array of block ID containing the data. Therefore, the 13th block of the file will be the first
		block ID contained in the indirect block. With a 1KiB block size, blocks 13 to 268 of the file data are
		contained in this indirect block.

		The 14th entry in this array is the block number of the first doubly-indirect block; which is a block
		containing an array of indirect block IDs, with each of those indirect blocks containing an array of blocks
		containing the data. In a 1KiB block size, there would be 256 indirect blocks per doubly-indirect block,
		with 256 direct blocks per indirect block for a total of 65536 blocks per doubly-indirect block.
	
		The 15th entry in this array is the block number of the triply-indirect block; which is a block containing
		an array of doubly-indrect block IDs, with each of those doubly-indrect block containing an array of
		indrect block, and each of those indirect block containing an array of direct block. In a 1KiB file system,
		this would be a total of 16777216 blocks per triply-indirect block.

		A value of 0 in this array effectively terminates it with no further block being defined. All the remaining
		entries of the array should still be set to 0.
	*/
	uint32		i_block[15];

	/*
		32bit value used to indicate the file version (used by NFS).
	*/
	uint32		i_generation;

	/*
		32bit value indicating the block number containing the extended attributes. In revision 0 this value is
		always 0.

			Note: Patches and implementation status of ACL under Linux can generally be found at
			http://acl.bestbits.at/
	*/
	uint32		i_file_acl;

	/*
		In revision 0 this 32bit value is always 0. In revision 1, for regular files this 32bit value contains the high
		32 bits of the 64bit file size.

			Note: Linux sets this value to 0 if the file is not a regular file (i.e. block devices, directories, etc). In
			theory, this value could be set to point to a block containing extended attributes of the directory or
			special file.
	*/
	uint32		i_dir_acl;

	/*
		32bit value indicating the location of the file fragment.

			Note: In Linux and GNU HURD, since fragments are unsupported this value is always 0. In Ext4 this
			value is now marked as obsolete.

			In theory, this should contain the block number which hosts the actual fragment. The fragment
			number and its size would be contained in the i_osd2 structure.
	*/
	uint32		i_faddr;

	/*
		96bit OS dependant structure.
	*/
	uint8		i_osd2[12];
} __attribute__((packed)) Ext2Inode, * Ext2InodePtr;

typedef struct
{
	/*
		8bit fragment number. Always 0 GNU HURD since fragments are not supported. Obsolete with Ext4
	*/
	uint8		h_i_frag;

	/*
		8bit fragment size. Always 0 in GNU HURD since fragments are not supported. Obsolete with Ext4.
	*/
	uint8		h_i_fsize;

	/*
		High 16bit of the 32bit mode.
	*/
	uint16		h_i_mode_high;

	/*
		High 16bit of user id.
	*/
	uint16		h_i_uid_high;

	/*
		High 16bit of group id.
	*/
	uint16		h_i_gid_high;

	/*
		32bit user id of the assigned file author. If this value is set to -1, the POSIX user id will be used.
	*/
	uint32		h_i_author;
} __attribute__((packed)) Ext2HurdOsd2, * Ext2HurdOsd2Ptr;

typedef struct
{
	/*
		8bit fragment number.

			Note: Always 0 in Linux since fragments are not supported.

			Important: A new implementation of Ext2 should completely disregard this field if the i_faddr value is
			0; in Ext4 this field is combined with l_i_fsize to become the high 16bit of the 48bit blocks count for
			the inode data.
	*/
	uint8		l_i_frag;

	/*
		8bit fragment size.

			Note: Always 0 in Linux since fragments are not supported.

			Important: A new implementation of Ext2 should completely disregard this field if the i_faddr value is
			0; in Ext4 this field is combined with l_i_frag to become the high 16bit of the 48bit blocks count for the
			inode data.
	*/
	uint8		l_i_fsize;

	uint16		reserved1;

	/*
		High 16bit of user id.
	*/
	uint16		l_i_uid_high;

	/*
		High 16bit of group id.
	*/
	uint16		l_i_gid_high;


	uint32		reserved2;
} __attribute__((packed)) Ext2LinuxOsd2, * Ext2LinuxOsd2Ptr;

typedef struct
{
	/*
		8bit fragment number. Always 0 in Masix as framgents are not supported. Obsolete with Ext4
	*/
	uint8		m_i_frag;

	/*
		8bit fragment size. Always 0 in Masix as fragments are not supported. Obsolete with Ext4
	*/
	uint8		m_i_fsize;

	uint8		reserved[10];
} __attribute__((packed)) Ext2MasixOsd2, * Ext2MasixOsd2Ptr;



#endif
