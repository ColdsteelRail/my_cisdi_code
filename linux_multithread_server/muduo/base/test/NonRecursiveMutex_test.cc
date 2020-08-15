#include "../Mutex.h"
#include "../Thread"
#include <stdio.h>
#include <vector>

using namespace muduo;

class Foo
{
public:
    void doit() const;
};

MutexLock mutex;
std::vector<Foo> foos;

void post(const Foo& f)
{
    MutexLockGuard lock(mutex);
    foos.push_back(f);
}

void traverse()
{
    MutexLockGuard lock(mutex);
    for (std::vector<Foo>::const_iterator it = foos.begin();
        it != foos.end(); ++it)
    {
        it->doit();
    }
}

void Foo::doit()
{
    Foo f;
    past(f);
}

int main() 
{
    Foo f;
    post(f);
    traverse();
}