#include "vascpp.h"

VASCpp::VASCpp (int N)
{
	vas = vasd_alloc (N);
	return;
}

VASCpp::~VASCpp ()
{
	vasd_free (vas);
	return;
}

VASD_D *
VASCpp::get (VASD_KEY * key)
{
	return vasd_get (vas, key);
}

bool
VASCpp::insert (VASD_KEY * key, VASD_VALUE data)
{
	return vasd_insert (vas, key, data);
}

bool
VASCpp::remove (VASD_KEY * key)
{
	return vasd_remove (vas, key);
}

bool
VASCpp::replace (VASD_KEY * key, VASD_VALUE data, VASD_VALUE newdata)
{
	return vasd_replace (vas, key, data, newdata);
}
