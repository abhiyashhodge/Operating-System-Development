/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

// NOTE: ALONG WITH REQUIRED REGULAR PART FOR MP5, OPTION 1 IS ALSO IMPLEMENTED

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  size_of_queue = 0;
  this->Disk = NULL;
  // assert(false);
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {

  //Machine::disable_interrupts();                             //OPTION 1 IMPLEMENTED: DISABLED INTERRUPTS
    
  if(Disk !=NULL && Disk->is_ready() && Disk->size_disk_queue != 0)
  {
        Thread *disk_queue_thread = Disk->disk_queue->remove_from_queue();
        Disk->size_disk_queue -= 1;
        Thread::dispatch_to(disk_queue_thread);
  }
  else
  {	
  	if (size_of_queue != 0)
  	{
   		Console::puts("yield: "); Console::puti(size_of_queue); Console::puts("\n");
		size_of_queue -= 1;
		Thread* new_thread = ready_queue.remove_from_queue();
   		Console::puts("yield: "); Console::puti(new_thread->ThreadId()); Console::puts("\n");
  		//Machine::enable_interrupts();			    // INTERRUPT ENABLED
	       Console::puts("In yield before dispatch ...\n");

		Thread::dispatch_to(new_thread);
  	}
  	else
  	{    
		Console::puts(" Threads not available to dispatch\n");
  	}

  }
 // assert(false);
}

void Scheduler::resume(Thread * _thread) {
//  Machine::disable_interrupts();              
   ready_queue.add_to_queue(_thread);
   size_of_queue += 1;
//  Machine::enable_interrupts();
   Console::puts("Resume: "); Console::puti(_thread->ThreadId());
 // assert(false);
}

void Scheduler::add(Thread * _thread) { 
//  Machine::disable_interrupts();          
   ready_queue.add_to_queue(_thread);
//  Machine::enable_interrupts();
   size_of_queue += 1;
 // assert(false);
}

void Scheduler::terminate(Thread * _thread) {
  int temp = size_of_queue;

  while(temp > 0)
  {
	Thread *removed_thread = ready_queue.remove_from_queue();
	temp -= 1;
        if(removed_thread->ThreadId() == _thread->ThreadId())
	{
		size_of_queue -= 1;
        }
        else
        {
		ready_queue.add_to_queue(removed_thread);
	}
  }


 // assert(false);
}


void Scheduler::add_disk(BlockingDisk * disk) {
    Disk = disk;
}
