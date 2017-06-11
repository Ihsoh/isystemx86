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
} __attribute__((packed)) BlockGroupDescriptor, * BlockGroupDescriptorPtr;




#endif
