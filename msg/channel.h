#pragma once

#include <pthread.h>
#include <exception>
#include <std/ascii_stream.h>

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
	void begin_data_probe();
	void end_data_probe();
	bool probe();
};

template <typename data_t>
struct port;

template <typename data_t = void>
struct chan : channel
{
	typedef channel super;
	
	chan(port<data_t> &send, port<data_t> &recv)
	{
		send.C = this;
		recv.C = this;
	}

	virtual ~chan() {}

	const data_t *data;

	virtual void send(const data_t &value)
	{
		begin_send();
		data = &value;
		end_send();
	}

	virtual data_t recv()
	{
		data_t temp;
		begin_recv();
		temp = *data;
		end_recv();
		return temp;
	}

	virtual bool probe()
	{
		return super::probe();
	}

	virtual data_t data_probe()
	{
		data_t temp;
		begin_data_probe();
		temp = *data;
		end_data_probe();
		return temp;
	}
};


template <typename data_t = void>
struct log_chan : chan<data_t>
{
	typedef chan<data_t> super;

	log_chan(port<data_t> &send, port<data_t> &recv, const char *path = NULL) : super(send, recv)
	{
		if (path)
			dbg.open(path);
		else
			dbg.ptr = stdout;
	}

	~log_chan() {
		if (dbg.ptr != stdout && dbg.ptr != NULL)
			dbg.close();
	}

	core::ascii_stream dbg;

	using super::send;

	data_t recv()
	{
		data_t temp = super::recv();
		dbg << temp << endl;
		return temp;
	}

	using super::probe;
	using super::data_probe;
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

	bool probe()
	{
		if (C)
			return C->probe();
		else
			throw disconnected();
	}

	data_t data_probe()
	{
		if (C)
			return C->data_probe();
		else
			throw disconnected();
	}

};

}
