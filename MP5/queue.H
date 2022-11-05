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

		void set_thread(Thread* new_thread)
		{
			my_thread = new_thread;
			next = NULL;
		}

		void add_to_queue(Thread *new_thread)   //Add at the end of the queue
		{

			if(my_thread != NULL)
			{
				My_queue *temp = this;
				while((temp->next) != NULL)
				{
					temp=temp->next;
				}
			        
				temp->next = new My_queue();
				temp = temp->next;
				temp->set_thread(new_thread);
				//next = new_queue;
				//delete new_queue;
			}
			else
			{
				my_thread = new_thread;
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
	







