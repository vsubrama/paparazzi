#ifndef SYNCH_QUEUE_H
#define SYNCH_QUEUE_H
#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include "common.h"

template<class T1>
class QSynchQueue{
 public:
  QSynchQueue();
  ~QSynchQueue();
 public:
  void enqueue(const T1& data);
  int get_size();
  T1 dequeue();
  /*  T& head();
      const T& head()const;*/
  void makequeuenonblocking();
 private:
  QQueue<T1>container;
  static const int MAX_SIZE; 
  QWaitCondition cv;
  QMutex mutex;
  bool canBlock;
};
template<class T1>
const int QSynchQueue<T1>::MAX_SIZE = 256;

template<class T1>
QSynchQueue<T1>::QSynchQueue(){
  canBlock=true;
}

template<class T1>
QSynchQueue<T1>::~QSynchQueue(){
  cv.wakeAll();
}
template<class T1>
int QSynchQueue<T1>::get_size(){
  int size=0;
  mutex.lock();
  size = container.size();
  mutex.unlock();
  return size;
}

template<class T1>
void QSynchQueue<T1>::makequeuenonblocking(){
  /*check if someone is already blocked on the queue. if
   that's the case wakep up all thread*/
  mutex.lock();
  canBlock=false;
  cv.wakeAll();
  mutex.unlock();
}



template<class T1>
void QSynchQueue<T1>::enqueue(const T1& data){
  mutex.lock();
  while(container.size()==MAX_SIZE && canBlock){
    cv.wait(&mutex);
  }

  if(container.size()==MAX_SIZE){
    /*I woke up and containes is still full.BBYe */
    mutex.unlock();
    return;
  }

  container.enqueue(data);
  if(container.size()==1)//if contanier has atleast one item
    cv.wakeAll();
  mutex.unlock();
}

template<class T1>
T1 QSynchQueue<T1>::dequeue(){
  T1 data;
  mutex.lock();
  while(container.size()==0 && canBlock){
    cv.wait(&mutex);
  }
  if(container.size()==0){
    /*I woke up and containes is still empty.
      Therefore, can't do much.*/
    mutex.unlock();
    return data;
  }
  data = container.dequeue();
  if(container.size()==MAX_SIZE-1)//if contanier has atleast one item
    cv.wakeAll();
  mutex.unlock();
  return data;
}
#endif
