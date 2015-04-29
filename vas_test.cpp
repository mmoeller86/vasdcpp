#include <stdio.h>
#include "vasd.h"

int main (int c, char ** args)
{
	VAS vas;
	long long int val;
	VASD_VALUE value;
	VASD_VALUE newvalue;
	VASD_D * d;
	VASD_KEY key;
	
	setbuf (stdout, NULL);
	if (vasd_init (&vas, 1024 * 1024 * 16) == FALSE) {
		fprintf (stderr, "%s: vas_init failed...\n", args [0]);
		return 1;
	}

	value.invalid = FALSE;
	value.val.ll = 1000;
	key.dt = VASD_DT_LL;
	key.value.ll = 1000;
	if (vasd_insert (&vas, &key, value) == FALSE) {
		fprintf (stderr, "%s: vas_insert failed...\n", args [0]);
		vasd_free (&vas);
		return 2;
	}
	
	d = vasd_get (&vas, &key);
	printf ("val = %d\n", d->datas [0].val.ll);
	
	newvalue.invalid = FALSE;
	newvalue.val.ll = 2000;
	vasd_replace (&vas, &key, value, newvalue);
	
	d = vasd_get (&vas, &key);
	printf ("val = %d\n", d->datas [0].val.ll);

	vasd_free (&vas);
	return 0;
}