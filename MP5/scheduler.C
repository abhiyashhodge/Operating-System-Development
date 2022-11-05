/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

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
  // assert(false);
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  if (size_of_queue != 0)
  {
	Thread::dispatch_to(ready_queue.remove_from_queue());
	size_of_queue -= 1;
  }
  else
  {    
	Console::puts(" Threads not available to dispatch\n");
  }
 // assert(false);
}

void Scheduler::resume(Thread * _thread) {
   ready_queue.add_to_queue(_thread);
   size_of_queue += 1;
 // assert(false);
}

void Scheduler::add(Thread * _thread) { 
   ready_queue.add_to_queue(_thread);
   size_of_queue += 1;
 // assert(false);
}

void Scheduler::terminate(Thread * _thread) {
 // assert(false);
}
