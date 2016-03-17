#include "threadpool.h"
#include <errno.h>
#include <string.h>
#include <assert.h>



struct threadpool *threadpool_init(int thread_num, int queue_max)
{
     struct threadpool *pool = NULL;
     do
     {
         pool = (struct threadpool*)malloc(sizeof(struct threadpool));
         if(pool == NULL)
         {
             perror("malloc");
             break;
         }
         pool->thread_num = thread_num;
         pool->queue_max_num = queue_max;
         pool->queue_cur_num = 0;
         pool->head = NULL;
         pool->tail = NULL;
         if(pthread_mutex_init(&(pool->mutex),NULL))
         {
             perror("Mutex Init:");
             break;
         }
         if(pthread_cond_init(&(pool->queue_empty),NULL))
         {
             perror("Cond Init:");
             break;
         }
         if(pthread_cond_init(&(pool->queue_not_empty),NULL))
         {
             perror("Cond Init:");
             break;
         }
         if(pthread_cond_init(&(pool->queue_not_full),NULL))
         {
             perror("Cond Init:");
             break;
         }

         pool->pthreads = malloc(sizeof(pthread_t)*thread_num);
         if(NULL == pool->pthreads)
         {
             perror("Malloc thread.");
             break;
         }
         pool->queue_close = 0;
         pool->pool_close = 0;

         int i;
         for(i = 0;i < pool->thread_num;++i)
         {
             pthread_create(&(pool->pthreads[i]),NULL,threadpool_function,(void*)pool);
         }
         return pool;

     }while(0);

     /*
     if(pool!= NULL)
         free(pool);
      */
     return NULL;
}


int threadpool_add_job(struct threadpool *pool, void *(*callback_function)(void *), void *arg)
{
    assert(pool != NULL);
    assert(callback_function != NULL);
    assert(arg != NULL);

    pthread_mutex_lock(&(pool->mutex));

    while((pool->queue_cur_num == pool->queue_max_num) && !(pool->queue_close || pool->pool_close))
    {
        pthread_cond_wait(&(pool->queue_not_full),&(pool->mutex));
    }
    if(pool->queue_close || pool->pool_close)
    {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }

    struct job *pjob = (struct job*)malloc(sizeof(struct job));
    if(NULL == pjob)
    {
        perror("Malloc job");
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }

    pjob->callbacl_function = callback_function;
    pjob->arg = arg;
    pjob->next = NULL;
    if(pool->head == NULL)
    {
        pool->head = pool->tail = pjob;
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }
    else
    {
        pool->tail->next = pjob;
        pool->tail = pjob;
    }
    pool->queue_cur_num++;
    pthread_mutex_unlock(&(pool->mutex));
    return 0;
}



int threadpool_destroy(struct threadpool *pool)
{
    assert(pool != NULL);
    pthread_mutex_lock(&(pool->mutex));

    if(pool->queue_close || pool->pool_close)
    {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }
    pool->queue_close = 1;
    while(pool->queue_cur_num !=0)
    {
        pthread_cond_wait(&(pool->queue_empty),&(pool->mutex));
    }

    pool->pool_close =1 ;
    pthread_mutex_unlock(&(pool->mutex));

    pthread_cond_broadcast(&(pool->queue_not_empty));
    pthread_cond_broadcast(&(pool->queue_not_full));

    int i;
    for(i =0 ;i < pool->thread_num;++i)
    {
        pthread_join(pool->pthreads[i],NULL);
    }

    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->queue_empty));
    pthread_cond_destroy(&(pool->queue_not_empty));
    pthread_cond_destroy(&(pool->queue_not_full));
    free(pool->pthreads);
    struct job *p;
    while(pool->head != NULL)
    {
        p = pool->head;
        pool->head = p->next;
        free(p);
    }
    free(pool);
    return 0;
}

void *threadpool_function(void *arg)
{
    struct threadpool *pool = (struct threadpool*)arg;
    struct job *pjob = NULL;
    while(1)
    {
        pthread_mutex_lock(&pool->mutex);
        while((pool->queue_cur_num == 0) && !pool->pool_close)
        {
            pthread_cond_wait(&(pool->queue_not_empty),&(pool->mutex));
        }
        if(pool->pool_close)
        {
            pthread_mutex_unlock(&(pool->mutex));
            pthread_exit(NULL);
        }
        pool->queue_cur_num--;
        pjob = pool->head;
        if(pool->queue_cur_num == 0)
        {
            pool->head = pool->tail = NULL;
        }else
        {
            pool->head = pjob->next;
        }

        if(pool->queue_cur_num == 0)
        {
            pthread_cond_signal(&(pool->queue_empty));
        }

        if(pool->queue_cur_num == pool->queue_max_num -1)
        {
            pthread_cond_broadcast(&(pool->queue_not_full));
        }
        pthread_mutex_unlock(&(pool->mutex));

        (*(pjob->callbacl_function))(pjob->arg);
        free(pjob);
        pjob = NULL;

    }
}















