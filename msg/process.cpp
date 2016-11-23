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

void process::run()
{
}

void process::begin()
{
}

void process::end()
{
}

void *run(void *proc)
{
	if (proc != NULL)
	{
		((process*)proc)->begin();
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
