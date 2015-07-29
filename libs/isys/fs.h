#ifndef	_ISYS_FS_H_
#define	_ISYS_FS_H_

#include "types.h"

#define	ISYS_FILE_MODE_WRITE	0x00000001
#define	ISYS_FILE_MODE_READ		0x00000002
#define	ISYS_FILE_MODE_APPEND	0x00000004

typedef struct
{
	BYTE			Name[256];
	UINT32			Length;
	ISYSDateTime 	CreatedDateTime;
	ISYSDateTime 	ChangedDateTime;
	BOOL			locked;
	UINT32			__BlockID;
} FileInfomation, * FileInfomationPtr;

typedef struct
{
	BYTE			Name[256];
	UINT32			Length;
	ISYSDateTime 	CreatedDateTime;
	ISYSDateTime 	ChangedDateTime;
	UINT32 			ItemCount;
	UINT32			__BlockID;
} DirectoryInfomation, * DirectoryInfomationPtr;

typedef VOIDPTR ISYSFilePtr;

extern
ISYSFilePtr
ISYSOpenFile(	IN BYTEPTR path,
				IN UINT32 mode);

extern
BOOL
ISYSCloseFile(IN ISYSFilePtr file);

extern
BOOL
ISYSWriteFile(	IN ISYSFilePtr file,
				IN BYTEPTR data,
				IN UINT32 length);

extern
UINT32
ISYSReadFile(	IN ISYSFilePtr file,
				OUT BYTEPTR data,
				IN UINT32 length);

extern
BOOL
ISYSAppendFile(	IN ISYSFilePtr file,
				IN BYTEPTR data,
				IN UINT32 length);

extern
BOOL
ISYSResetFile(IN ISYSFilePtr file);

extern
BOOL
ISYSFEOF(IN ISYSFilePtr file);

extern
UINT32
ISYSGetFileLength(IN BYTEPTR path);

extern
BOOL
ISYSFileExists(IN BYTEPTR path);

extern
BOOL
ISYSDirectoryExists(IN BYTEPTR path);

extern
BOOL
ISYSExists(IN BYTEPTR path);

extern
BOOL
ISYSCreateFile(IN BYTEPTR path);

extern
BOOL
ISYSCreateDirectory(IN BYTEPTR path);

extern
BOOL
ISYSDeleteFile(IN BYTEPTR path);

extern
BOOL
ISYSDeleteDirectory(IN BYTEPTR path,
					IN BOOL r);

extern
BOOL
ISYSRenameFile(	IN BYTEPTR path,
				IN BYTEPTR name);

extern
BOOL
ISYSRenameDirectory(IN BYTEPTR path,
					IN BYTEPTR newName);

extern
BOOL
ISYSGetFileInfomation(OUT FileInfomationPtr info);

extern
BOOL
ISYSGetDirectoryInfomation(OUT DirectoryInfomationPtr info);

extern
BOOL
ISYSGetDirectoryItemName(	IN DirectoryInfomationPtr info,
							IN UINT32 index,
							OUT BYTEPTR name);

extern
BOOL
ISYSMapPath(IN BYTEPTR rpath,
			OUT BYTEPTR apath);

#endif
