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
	//printf("entering begin_send()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  begin_send()\n");
	while (status == full || status == ready)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
	//printf("exiting  begin_send()\n");
}

void channel::mid_send()
{
	//printf("entering mid_send()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  mid_send()\n");
	check();
	status = ready;
	pthread_cond_signal(&signal);
	pthread_mutex_unlock(&mutex);
	//printf("exiting  mid_send()\n");
}

void channel::end_send()
{
	//printf("entering end_send()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  end_send()\n");
	check();
	status = full;
	pthread_cond_signal(&signal);
	while (status == full)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
	//printf("exiting  end_send()\n");
}

void channel::begin_recv()
{
	//printf("entering begin_recv()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  begin_recv()\n");
	while (status == empty || status == ready)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
	//printf("exiting  begin_recv()\n");
}

void channel::end_recv()
{
	//printf("entering end_recv()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  end_recv()\n");
	check();
	status = empty;
	pthread_cond_signal(&signal);
	pthread_mutex_unlock(&mutex);
	//printf("exiting  end_recv()\n");
}

void channel::begin_data_probe()
{
	//printf("entering begin_data_probe()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  begin_data_probe()\n");
	while (status == empty)
		pthread_cond_wait(&signal, &mutex);
	check();
	pthread_mutex_unlock(&mutex);
	//printf("exiting  begin_data_probe()\n");
}

void channel::end_data_probe()
{
	//printf("entering end_data_probe()\n");
	pthread_mutex_lock(&mutex);
	//printf("locking  end_data_probe()\n");
	check();
	pthread_cond_signal(&signal);
	pthread_mutex_unlock(&mutex);
	//printf("exiting  end_data_probe()\n");
}

bool channel::probe()
{
	bool temp;
	pthread_mutex_lock(&mutex);
	temp = (status == full || status == ready);
	pthread_mutex_unlock(&mutex);
	return temp;
}

}
