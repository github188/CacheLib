#ifndef CACHEQUEUE_H
#define CACHEQUEUE_H

#include <list>
using namespace std;
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/pool/object_pool.hpp>

#include "log4cpp/log4cppwraper.h"

#define QEUUE_MAX 300
template<typename T>
class CacheQueue
{
public:
    enum FlagCim
    {
        CIM_FAILED = 0,
        CIM_SUCCESS
    };
    CacheQueue();
    ~CacheQueue();
    T* getBuf();
    void pushBuf(T* buf);
    bool isRun();

    T* getBufForFree();

    void setSuccess(int flag)
    {
        _flag = flag;
    }
    const int &isSuccess() const
    {
        return _flag;
    }

    void signalReturn(const int& flag);

    T* getPoolArray(int size)
    {
        return new T[size];
    }
    void freePoolArray(T* tmp)
    {
        delete []tmp;
    }

    T* getPool()
    {
        T* tmpPool = NULL;
        tmpPool =  new T;
        return tmpPool;
    }
    void freePool(T* tmp)
    {
        delete tmp;
        tmp = NULL;
    }

private:
    list<T*> _queue;
    boost::mutex _lock;
    boost::condition _condFull;
    boost::condition _condEmpty;
    bool _run;
    int _flag;

    //boost::object_pool<T> _cimPool;
    //boost::mutex _mutexPool;
};

template<typename T>
CacheQueue<T>::CacheQueue() : _run(true),_flag(CIM_SUCCESS)
{
}

template<typename T>
CacheQueue<T>::~CacheQueue()
{
    typename list<T*>::iterator iterQue = _queue.begin();
    for(;iterQue != _queue.end(); iterQue++)
    {
        T* curQue = *iterQue;
        if(curQue)
        {
            freePool(curQue);
        }
    }
}

template<typename T>
bool CacheQueue<T>::isRun()
{
    return _run;
}

template<typename T>
void CacheQueue<T>::signalReturn(const int& flag)
{
    _lock.lock();
    _flag = flag;
    _run = false;
    _condFull.notify_all();
    _condEmpty.notify_all();
    _lock.unlock();
}

template<typename T>
T* CacheQueue<T>::getBufForFree()
{
    T*  tmp = NULL;
    _lock.lock();
    if(!_queue.empty())
    {
        tmp = _queue.front();
        _queue.pop_front();
    }
    _lock.unlock();
    return tmp;
}

template<typename T>
T* CacheQueue<T>::getBuf()
{
    _lock.lock();
    while(_queue.empty())
    {
        if(_flag == CIM_FAILED)
        {
            _lock.unlock();
            LOG_DEBUG("man get buf CIM_FAILED");
            return NULL;
        }
        if(_run)
        {
            _condEmpty.wait(_lock);
        }
        else
        {
            _lock.unlock();
            LOG_DEBUG("man get buf is not run");
            return NULL;
        }
    }
    T*  tmp = _queue.front();
    _queue.pop_front();
    if(_queue.size() < QEUUE_MAX)
    {
        _condFull.notify_all();
    }
    _lock.unlock();
    return tmp;
}

template<typename T>
void CacheQueue<T>::pushBuf(T* buf)
{
    _lock.lock();
    while(_queue.size() > QEUUE_MAX)
    {
        if(!_run)
        {
            _lock.unlock();
            return;
        }
        _condFull.wait(_lock);
    }
    _queue.push_back(buf);
    _condEmpty.notify_all();
    _lock.unlock();
}
#endif // CACHEQUEUE_H
