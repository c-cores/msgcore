#pragma once

#include <std/array.h>
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
	virtual void run();
	virtual void begin();
	virtual void end();
};

struct terminate : public std::exception
{
};

void *run(void *proc);

}
