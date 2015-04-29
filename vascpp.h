#pragma once

#include "vasd.h"
class VASCpp {
	public:
		VASCpp (int n);
		~VASCpp ();

		bool insert (VASD_KEY * key, VASD_VALUE data);
		bool replace (VASD_KEY * key, VASD_VALUE data, VASD_VALUE newdata);
		bool remove (VASD_KEY * key);
		VASD_D * get (VASD_KEY * key);
	
	private:
		VAS * vas;
};
