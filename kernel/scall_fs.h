/**
	@File:			scall_fs.h
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
*/

#ifndef	_SCALL_FS_H_
#define	_SCALL_FS_H_

#include "types.h"
#include "sparams.h"

#define	SCALL_FOPEN			0
#define	SCALL_FCLOSE		1
#define	SCALL_FWRITE		2
#define	SCALL_FREAD			3
#define	SCALL_FAPPEND		4
#define	SCALL_FRESET		5
#define	SCALL_EXISTS_DF		6
#define	SCALL_CREATE_DIR	7
#define	SCALL_CREATE_FILE	8
#define	SCALL_DEL_DIR		9
#define	SCALL_DEL_FILE		10
#define	SCALL_DEL_DIRS		11
#define	SCALL_RENAME_DIR	12
#define	SCALL_RENAME_FILE	13
#define	SCALL_FLEN			14
#define	SCALL_FCREATE_DT	15
#define	SCALL_FCHANGE_DT	16
#define	SCALL_DF_COUNT		17
#define	SCALL_DF			18
#define	SCALL_FIX_PATH		19
#define	SCALL_LOCK_FS		20
#define	SCALL_UNLOCK_FS		21
#define	SCALL_FS_LOCK_STATE	22

extern
void
system_call_fs(	IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams);

extern
void
system_call_fs_unlock_fs(void);

#endif
