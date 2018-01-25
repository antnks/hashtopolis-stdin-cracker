#include "corethread.h"
using namespace std;

CoreThread::CoreThread(atomic_llong *lengthCounter, atomic_llong *crackedCounter, atomic_llong *hashingCounter, int timeout, int startTime, long long length, RunThread *parent, QList<BcryptHash*> *hashes, QMutex *mutex){
    this->lengthCounter = lengthCounter;
    this->crackedCounter = crackedCounter;
    this->hashingCounter = hashingCounter;
    this->timeout = timeout;
    this->startTime = startTime;
    this->length = length;
    this->parent = parent;
    this->hashes = hashes;
    this->mutex = mutex;
}

void CoreThread::run(){
    QString combo;
    bool work = true;
    int m;
    BcryptHash *h;
    while(work){
        mutex->lock();
        if(!parent->getNext(combo, (long long int)lengthCounter)){
            mutex->unlock();
            break;
        }
        mutex->unlock();

        // try to verify every hash of combo
        for(int n=0;n<this->hashes->length();n++){
            h = this->hashes->at(n);
            m = BCrypt::validatePassword(combo.toStdString(), h->original.toStdString());
            hashingCounter++;
            if(m == 1){
                // found
                cout << h->original.toStdString() << ":" << combo.toStdString() << endl;
                h->found = true;
                this->hashes->replace(n, h);
                crackedCounter++;
            }
        }

        lengthCounter++;
        if((long long int)lengthCounter >= this->length){
            break; // we reached length limit
        }
        else if(timeout > 0 && time(NULL) - startTime > timeout){
            return;
        }
    }
}
