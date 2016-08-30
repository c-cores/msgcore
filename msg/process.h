#pragma once

#include <core/array.h>
#include <msg/channel.h>
#include <exception>

namespace msg
{

struct process
{
	pthread_t thread;

	process();
	virtual ~process();

	void start();
	void stop();
	virtual void run() = 0;
	virtual void end() = 0;
};

struct terminate : public std::exception
{
};

void *run(void *proc);

}
