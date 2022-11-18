/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "thread.H"

extern Scheduler* SYSTEM_SCHEDULER;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {

    size_disk_queue = 0;
    this->disk_queue =  new My_queue();
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */


void BlockingDisk::wait_until_ready() {
    if (!is_ready()) {
	Console::puts(" In wait until ready...\n");

        Thread * thread_running = Thread::CurrentThread();
	Console::puts(" In wait until ready before add to queue...\n");
        this->disk_queue->add_to_queue(thread_running);
    	size_disk_queue += 1;
	Console::puts(" In wait until ready just before yield...\n");
        SYSTEM_SCHEDULER->yield();
    }

}


bool BlockingDisk::is_ready() {
    return SimpleDisk::is_ready();
}

/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::read(_block_no, _buf);

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::write(_block_no, _buf);
}
