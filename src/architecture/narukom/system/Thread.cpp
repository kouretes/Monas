#include "Thread.h"
#include <iostream>
using std::cout;
using std::endl;
Thread::Thread(bool run_start) : m_running(run_start) 
{
  if(run_start)
    start();
}


void Thread::start()
{
  m_running = true;
  m_Thread = boost::thread(&Thread::thread_run,this);
}
void Thread::join()
{
  m_Thread.join();
//   m_running =false;
}

void Thread::stop()
{
  m_running = false;
}
void Thread::thread_run()
{
	cout << "Starting running " << endl;
  while(m_running)
     run();
  
	cout << "Exiting thread run " << endl;
  
}
void Thread::run()
{
  cout << "Default run function of Thread " << endl;
  cout << "Sleep for 3 seconds " << endl;
  boost::posix_time::seconds sec(3);
  boost::this_thread::sleep(sec);
}



