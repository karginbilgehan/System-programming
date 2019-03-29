#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"


class GTUOS{
	public:
		int counter;
		GTUOS(){counter=1;}
		uint64_t handleCall( CPU8080 & cpu);
		
};

#endif
