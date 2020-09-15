#ifndef c_ceg_worker_h
#define c_ceg_worker_h

#include <c_eg/queue.h>
#include <pthread.h>

struct Worker_s;
typedef struct Worker_s Worker, *WorkerRef;

WorkerRef Worker_new(QueueRef qref, int _id);
void Worker_free(WorkerRef wref);
void Worker_start(WorkerRef wref);

pthread_t* Worker_pthread(WorkerRef);

#endif