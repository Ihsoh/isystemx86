#ifndef	_ALLOCA_H
#define	_ALLOCA_H	1
#include "../features.h"

#define	__need_size_t
#include "../stddef.h"

__BEGIN_DECLS

/* Remove any previous definitions.  */
#undef	__alloca
#undef	alloca

/* Allocate a block that will be freed when the calling function exits.  */
extern __ptr_t __alloca __P ((size_t __size));
extern __ptr_t alloca __P ((size_t __size));

#ifdef	__GNUC__
#define	__alloca(size)	__builtin_alloca(size)
#endif /* GCC.  */

#define	alloca(size)	__alloca(size)

__END_DECLS

#endif /* alloca.h */
