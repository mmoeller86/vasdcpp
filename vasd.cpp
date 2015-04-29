#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _MSC_VER
#include "win32.h"
#else
#include "posix.h"
#endif
#include "vasd.h"

BOOL vasd_key_cmp (VASD_KEY *, VASD_KEY *);

VASD_C *
vasd_c_alloc ()
{
	return (VASD_C *)calloc (1, sizeof (VASD_C));
}

void
vasd_c_free (VASD_C * c)
{
	free (c);
}

void
vasd_c_set_key (VASD_C * c, VASD_KEY * key)
{
	c->key = key;
	return;
}

BOOL
vasd_is_allocated (VOID * va)
{
	return vmem_page_commited (va);
}

VOID *
vasd_get_virtual (VAS * vas, int i)
{
	return &vas->va [i * 20];
}

BOOL
vasd_alloc (VOID * va)
{
	return vmem_commit (va);
}

VASD_E *
vasd_e_alloc (VAS * vas, int i)
{
	if (vasd_is_allocated (vasd_get_virtual (vas, i)) == 0) {
		if (vasd_alloc (vasd_get_virtual (vas, i)) == FALSE)
			return NULL;
	}
	
	return (VASD_E *)&vas->va [i * 20];
}

void
vasd_e_free (VAS * vas, int i)
{
	VASD_E * e;
	
	e = (VASD_E *)vasd_get_virtual (vas, i);
	if (e->n > 0) {
		int i;
		
		for (i = 0; i < e->n; i++) {
			free (e->collides [i]->key);
			free (e->collides [i]);
		}
	
		free (e->collides);
	}
}

VASD_KEY *
vasd_k_dup (VASD_KEY * key)
{
	VASD_KEY * k;
	
	k = (VASD_KEY *)calloc (1, sizeof (VASD_KEY));
	if (k == NULL)
		return NULL;

	memcpy (k, key, sizeof (VASD_KEY));
	return k;
}

VASD_D
vasd_d_alloc ()
{
	VASD_D d;
	
	d.datas =(VASD_VALUE *)calloc (1, sizeof (VASD_VALUE));
	d.n = 1;
	return d;
}

void
vasd_d_append (VAS * vas, VASD_D * d, VASD_VALUE data)
{
	d->n++;
	d->datas = (VASD_VALUE *)realloc (d->datas, (d->n * sizeof (VASD_VALUE)));
	if (d->datas == NULL)
		return;
		
	d->datas [d->n -1] = data;
	return;
}

BOOL
vasd_e_append (VAS * vas, VASD_E * e, VASD_KEY * key, VASD_VALUE data)
{
	VASD_C * c;
	int i;
	
	if (vasd_is_allocated (e) == FALSE) {
		if (vasd_alloc (e) == FALSE) {
			fprintf (stderr, "failed to allocate page...\n");
			return FALSE;
		}
	}

	for (i = 0; i < e->n; i++)
		if (vasd_key_cmp (e->collides [i]->key, key))
			break;
			
	if (i != e->n) {
		vasd_d_append (vas, &e->collides [i]->datas, data);
		return TRUE;
	}
			
	e->n++;
	e->collides = (VASD_C **)realloc (e->collides, sizeof (VASD_C *) * e->n);
	if (e->collides == NULL) {
		fprintf (stderr, "failed to allocate e->collides...\n");
		return FALSE;
	}
		
	c = vasd_c_alloc ();
	if (c == NULL) {
		fprintf (stderr, "VASD_c_alloc failed...\n");
		return FALSE;
	}

	c->key = vasd_k_dup (key);
	c->datas = vasd_d_alloc ();
	c->datas.datas [0] = data;
	e->collides [e->n -1] = c;
	return TRUE;
}

BOOL
vasd_key_cmp (VASD_KEY * k0, VASD_KEY * k1)
{
	if (k0->dt != k1->dt)
		return FALSE;
		
	switch (k0->dt) {
		case VASD_DT_INT:
			return k0->value.i == k1->value.i;
		case VASD_DT_LL:
			return k0->value.ll == k1->value.ll;
		case VASD_DT_FLOAT:
			return k0->value.fl == k1->value.fl;
		case VASD_DT_DOUBLE:
			return k0->value.dbl == k1->value.dbl;
		case VASD_DT_LDOUBLE:
			return k0->value.ldbl == k1->value.ldbl;
		case VASD_DT_STR:
			return strcmp (k0->value.str, k1->value.str) == 0;
	}
	
	return FALSE;
}

BOOL
vasd_e_remove (VAS * vas, VASD_E * e, VASD_KEY * key)
{
	VASD_C * c;
	int i;
	
	if (vasd_is_allocated (e) == FALSE)
		return FALSE;

	for (i = 0; i < e->n; i++) {
		if (vasd_key_cmp (e->collides [i]->key, key)) {
			memmove (&e->collides [i], e->collides [i +1], (e->n -i -1) * sizeof (VASD_C *));
			e->n--;
			e->collides = (VASD_C **)realloc (e->collides, e->n * sizeof (VASD_C *));
			return TRUE;
		}
	}
	
	return FALSE;
}

VASD_D *
vasd_e_get (VAS * vas, VASD_E * e, VASD_KEY * key)
{
	VASD_C * c;
	int i;

	if (vasd_is_allocated (e) == FALSE)
		return NULL;

	for (i = 0; i < e->n; i++) {
		if (vasd_key_cmp (e->collides [i]->key, key)) {
			return &e->collides [i]->datas;
		}
	}
	
	return NULL;
}

BOOL
vasd_e_replace (VAS * vas, VASD_E * e, VASD_KEY * key, VASD_VALUE data, VASD_VALUE newdata)
{
	VASD_C * c;
	int i, j;

	if (vasd_is_allocated (e) == FALSE)
		return FALSE;

	for (i = 0; i < e->n; i++) {
		if (vasd_key_cmp (e->collides [i]->key, key)) {
			for (j = 0; j < e->collides [i]->datas.n; j++) {
				if (memcmp ((void *)&e->collides [i]->datas.datas [j], (void *)&data, sizeof (VASD_VALUE)) == 0) {
					e->collides [i]->datas.datas [j] = newdata;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL
vasd_init (VAS * vas, long long int n)
{
	VOID * va;
	
	va = vmem_reserve (n);
	if (va == NULL)
		return FALSE;
		
	vas->va = (CHAR *)va;
	vas->n = n;
	return TRUE;
}

VAS *
vasd_alloc (long long int N)
{
	VAS * vas;
	
	vas = (VAS *)calloc (1, sizeof (VAS));
	vasd_init (vas, N);
	return vas;
}

void
vasd_free (VAS * vas)
{
	int i;
	
	for (i = 0; i < vas->n; i++) {
		if (vasd_is_allocated (vasd_get_virtual (vas, i)) == TRUE)
			vasd_e_free (vas, i);
	}
	
	vmem_free (vas->va, vas->n * 20);
}

long long int
vasd_hash_ll (VAS * vas, long long i)
{
	return ((i % vas->n) * 20);
}

long long int
vasd_hash_str (VAS * vas, char * str)
{
	long long int hash;
	
	hash = 0;
	while (*str) {
		hash <<= 1;
		hash += *str++;
		hash ^= 0xABDE4567;
	}
	
	return vasd_hash_ll (vas, hash);
}

long long int
vasd_hash_voidptr (VAS * vas, CHAR * ptr, int len)
{
	long long int hash;
	int i;
	
	hash = 0;
	for (i = 0; i < len; i++) {
		hash <<= 1;
		hash += *ptr++;
		hash ^= 0xABDE4567;
	}
	
	return vasd_hash_ll (vas, hash);
}

long long int
vasd_hash (VAS * vas, VASD_KEY * key)
{
	switch (key->dt) {
		case VASD_DT_STR:
			return vasd_hash_str (vas, (char *)key->value.str);
		case VASD_DT_VOIDPTR:
			return vasd_hash_voidptr (vas, (CHAR *)key->value.ptr, key->len);
		case VASD_DT_LL:
			return vasd_hash_ll (vas, (long long int)key->value.ll);
		case VASD_DT_INT:
			return vasd_hash_ll (vas, (long long int)key->value.i);
		case VASD_DT_FLOAT:
			return vasd_hash_ll (vas, (long long int)key->value.fl);
		case VASD_DT_DOUBLE:
			return vasd_hash_ll (vas, (long long int)key->value.dbl);
		case VASD_DT_LDOUBLE:
			return vasd_hash_ll (vas, (long long int)key->value.ldbl);
	}
	
	return 0;
}

BOOL
vasd_insert (VAS * vas, VASD_KEY * key, VASD_VALUE data)
{
	long long int va;
	VASD_E * e;
	
	va = vasd_hash (vas, key);
	e = (VASD_E *)(va + (long long int)vas->va);
	return vasd_e_append (vas, e, key, data);
}

BOOL
vasd_remove (VAS * vas, VASD_KEY * key)
{
	long long int va;
	VASD_E * e;
	
	va = vasd_hash (vas, key);
	e = (VASD_E *)(va + (long long int)vas->va);
	return vasd_e_remove (vas, e, key);
}

VASD_D *
vasd_get (VAS * vas, VASD_KEY * key)
{
	long long int va;
	VASD_E * e;
	
	va = vasd_hash (vas, key);
	e = (VASD_E *)(va + (long long int)vas->va);
	return vasd_e_get (vas, e, key);
}

BOOL
vasd_replace (VAS * vas, VASD_KEY * key, VASD_VALUE data, VASD_VALUE newdata)
{
	long long int va;
	VASD_E * e;
	
	va = vasd_hash (vas, key);
	e = (VASD_E *)(va + (long long int)vas->va);
	return vasd_e_replace (vas, e, key, data, newdata);
}
