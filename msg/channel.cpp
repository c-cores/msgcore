#include "channel.h"
#include "process.h"

namespace msg
{

channel::channel()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&signal, NULL);
	status = empty;
}

channel::~channel()
{
}

void channel::stop()
{
	pthread_mutex_lock(&mutex);
	status = term;
	pthread_cond_signal(&signal);
	pthread_mutex_unlock(&mutex);
}

void channel::check()
{
	if (status == term)
	{
		pthread_cond_signal(&signal);
		pthread_mutex_unlock(&mutex);
		throw ::msg::terminate();
	}
}

void channel::begin_send()
{
	pthread_mutex_lock(&mutex);
	while (status == full)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
}

void channel::end_send()
{
	pthread_mutex_lock(&mutex);
	check();
	status = full;
	pthread_cond_signal(&signal);
	while (status == full)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
}

void channel::begin_recv()
{
	pthread_mutex_lock(&mutex);
	while (status == empty)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
}

void channel::end_recv()
{
	pthread_mutex_lock(&mutex);
	check();
	status = empty;
	pthread_cond_signal(&signal);
	pthread_mutex_unlock(&mutex);
}

void channel::begin_data_probe()
{
	pthread_mutex_lock(&mutex);
	while (status == empty)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
}

void channel::end_data_probe()
{
	pthread_mutex_lock(&mutex);
	check();
	pthread_cond_signal(&signal);
	pthread_mutex_unlock(&mutex);
}

bool channel::probe()
{
	bool temp;
	pthread_mutex_lock(&mutex);
	temp = (status == full);
	pthread_mutex_unlock(&mutex);
	return temp;
}

}
