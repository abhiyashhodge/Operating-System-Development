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
#include "thread.H"
#include "scheduler.H"

extern Scheduler * SYSTEM_SCHEDULER;   // Declared extern to use the scheduler which is already declared in other file
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::wait_until_ready() {
       
	 if(!SimpleDisk::is_ready())
	 {
		SYSTEM_SCHEDULER->resume(Thread::CurrentThread());         // Thread resumes until Disk is ready
		SYSTEM_SCHEDULER->yield();			// Yields to other thread until Disk is ready to read/write
	 }
}



void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  SimpleDisk::read(_block_no, _buf);

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  SimpleDisk::write(_block_no, _buf);
}
