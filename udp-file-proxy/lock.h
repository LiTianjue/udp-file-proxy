#ifndef __LOCK_H__
#define __LOCK_H__



#define MUTEX_TYPE  pthread_mutex_t
#define MUTEX_SETUP(x)  pthread_mutex_init(&(x),NULL)
#define MUTEX_CLEANUP(x)    pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x)   pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x) pthread_mutex_unlock(&(x))

#define THREAD_ID   pthread_self()

int MUTEX_SETUP_RECURSUVE(MUTEX_TYPE *);




#endif
