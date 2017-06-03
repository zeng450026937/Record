#ifndef THREADRUN_H
#define THREADRUN_H

#include <QRunnable>
#include <QThreadPool>

class ThreadRun : public QRunnable
{
public:
    static void ONCE( void (*fptr)() ){
        QThreadPool::globalInstance()->start(new ThreadRun(fptr));
    }

protected:
    ThreadRun(void (*fptr)()) : funcPtr(fptr){}
    ~ThreadRun(){}

    void run(){
        funcPtr();
    }

private:
    void (*funcPtr)();

};

#endif // THREADRUN_H
