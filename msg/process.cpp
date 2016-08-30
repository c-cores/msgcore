#include "process.h"

namespace msg
{

process::process()
{
}

process::~process()
{
}

void process::start()
{
	pthread_create(&thread, NULL, ::msg::run, this);
}

void process::stop()
{
	pthread_join(thread, NULL);
}

void *run(void *proc)
{
	if (proc != NULL)
	{
		try
		{
			((process*)proc)->run();
		}
		catch (terminate &term)
		{
		}
		((process*)proc)->end();
	}
	return NULL;
}

}
