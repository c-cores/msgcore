#pragma once

#include <pthread.h>
#include <exception>

namespace msg
{

struct channel
{
	pthread_mutex_t mutex;
	pthread_cond_t signal;
	enum {
		empty = 0,
		full = 1,
		term = 2
	} status;

	channel();
	~channel();

	void stop();
	void check();
	
	void begin_send();
	void end_send();
	void begin_recv();
	void end_recv();
	bool begin_probe();
	void end_probe();
};

template <typename data_t>
struct port;

template <typename data_t = void>
struct chan : channel
{
	chan(port<data_t> &send, port<data_t> &recv)
	{
		send.C = this;
		recv.C = this;
	}

	~chan() {}

	const data_t *data;

	void send(const data_t &value)
	{
		begin_send();
		data = &value;
		end_send();
	}

	data_t recv()
	{
		data_t temp;
		begin_recv();
		temp = *data;
		end_recv();
		return temp;
	}

	bool probe(data_t *value = NULL)
	{
		if (begin_probe())
		{
			if (value != NULL)
				*value = *data;
			end_probe();
			return true;
		}
		else
			return false;
	}
};

struct disconnected : public std::exception
{
};

template <typename data_t = void>
struct port
{
	port()
	{
		C = NULL;
	}

	~port()
	{
		if (C)
			C->stop();
	}

	chan<data_t> *C;

	void stop()
	{
		if (C)
			C->stop();
	}

	void send(const data_t &value)
	{
		if (C)
			C->send(value);
		else
			throw disconnected();
	}

	data_t recv()
	{
		if (C)
			return C->recv();
		else
			throw disconnected();
	}

	bool probe(data_t *value = NULL)
	{
		if (C)
			return C->probe(value);
		else
			throw disconnected();
	}
};

}
