#pragma once
#include "types.h"

typedef enum {
	VASD_DT_INVALID,
	VASD_DT_VOIDPTR,
	VASD_DT_STR,
	VASD_DT_INT,
	VASD_DT_LL,
	VASD_DT_FLOAT,
	VASD_DT_DOUBLE,
	VASD_DT_LDOUBLE
} VASD_DT;

typedef union {
	char *	str;
	int		i;
	long long ll;
	float	fl;
	double	dbl;
	long double ldbl;
	void *		ptr;
} VASD_VAL;

typedef struct {
	BOOL	invalid;
	int		len;
	VASD_VAL	val;
} VASD_VALUE;

typedef struct {
	VASD_DT		dt;
	int			len;
	VASD_VAL		value;
} VASD_KEY;

typedef struct {
	VASD_VALUE *	datas;
	int n;
} VASD_D;

typedef struct {
	VASD_KEY *	key;
	VASD_D		datas;
} VASD_C;

typedef struct {
	long long int		n;
	VASD_C **	collides;
} VASD_E;

typedef struct {
	char *		va;
	long long int n;
} VAS;

VAS *		vasd_alloc (long long int N);
BOOL		vasd_init (VAS * vas, long long int n);
void		vasd_free (VAS * vas);
BOOL		vasd_insert (VAS * vas, VASD_KEY * key, VASD_VALUE data);
BOOL		vasd_remove (VAS * vas, VASD_KEY * key);
BOOL		vasd_replace (VAS * vas, VASD_KEY * key, VASD_VALUE data, VASD_VALUE newdata);
VASD_D *	vasd_get (VAS * vas, VASD_KEY * key);

BOOL		vasd_is_allocated (long long int va);
VOID *		vasd_get_virtual (VAS * vas, int i);
