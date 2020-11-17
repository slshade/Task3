#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#define  NEEDLES_NUMBER 100
#define BUF_SIZE 5

using namespace std;

struct count {
	int buf[BUF_SIZE]; // the buffer
	size_t len;
    int counter;
    pthread_mutex_t mutex;
	pthread_cond_t can_produce; // signaled when items are removed
	pthread_cond_t can_consume;
	pthread_cond_t can_check_quality;
};

void *doit(void *);
void *check_curvature(void *vptr);
void *sharpen(void *vptr);
void *check_quality(void *vptr);

int main()
{
    pthread_t worker_one, worker_two, worker_three;
    struct count c;
	c.len = 0;
    pthread_mutex_init(&c.mutex, NULL);
    pthread_cond_init(&c.can_produce, NULL);
	pthread_cond_init(&c.can_consume, NULL);

    c.counter = 0;
    srandom(1000);

	for (int i = 0; i < NEEDLES_NUMBER; i++)
	{
		printf("\033[32;1m NEEDLE %d: \033[0m\n", i + 1);
        pthread_create(&worker_one, NULL, &check_curvature, &c);
        pthread_create(&worker_two, NULL, &sharpen, &c);
		pthread_create(&worker_three, NULL, &check_quality, &c);

		pthread_join(worker_one, NULL);
		pthread_join(worker_two, NULL);
		pthread_join(worker_three, NULL);
	}
  
    exit(0);
}

void *check_curvature(void *vptr)
{
	struct count* c;

	c = (struct count*) vptr;
	pthread_mutex_lock(&c->mutex);
	if(c->len == BUF_SIZE) {
		pthread_cond_wait(&c->can_produce, &c->mutex);
	}
	int t = rand();
	cout << "Curvature is fine" << endl;
	c->buf[c->len] = t;
	++c->len;
	pthread_cond_signal(&c->can_consume);

	pthread_mutex_unlock(&c->mutex);
}

void *sharpen(void *vptr)
{
	struct count* c;

	c = (struct count*) vptr;
	pthread_mutex_lock(&c->mutex);
	if(c->len == 0) {
		pthread_cond_wait(&c->can_consume, &c->mutex);
	}
	--c->len;
	cout << "Sharped!" << endl;
	pthread_cond_signal(&c->can_produce);
	pthread_mutex_unlock(&c->mutex);
}

void *check_quality(void *vptr)
{
	struct count* c;

	c = (struct count*) vptr;

	pthread_mutex_lock(&c->mutex);
	cout << "Good quality!" << endl;
	pthread_mutex_unlock(&c->mutex);

}
