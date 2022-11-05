#include "thread.H"
//#include <cstddef>
#include "utils.H"

class My_queue
{

	private:
		Thread* my_thread;
		My_queue* next;

	public:

		My_queue()
		{
			my_thread = NULL;
			next = NULL;
		}

		void set_thread(Thread* current_thread)
		{
			my_thread = current_thread;
			next = NULL;
		}

		void add_to_queue(Thread *current_thread)   //Add at the end of the queue
		{
			if(my_thread != NULL)
			{
				while(next != NULL)
				{
					next->add_to_queue(current_thread);
				}
			        
				My_queue* new_queue = new My_queue();
				new_queue->set_thread(current_thread);
				next = new_queue;
				delete new_queue;
			}
			else
			{
				my_thread = current_thread;
			}
		}

		Thread *remove_from_queue()
		{
			if(my_thread == NULL)
				return NULL;
			Thread* returned_thread = my_thread;
			if(next == NULL)
				my_thread = NULL;
			else
			{
				my_thread = next->my_thread;
				My_queue* remove_next = next;
				next = next->next;

				delete remove_next;
			}
			
			return returned_thread;
		}

		
};
	







