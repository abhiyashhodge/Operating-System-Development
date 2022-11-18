#include "thread.H"
#include "utils.H"

// NOTE: ALONG WITH REQUIRED REGULAR PART FOR MP5, OPTION 1 IS ALSO IMPLEMENTED

class My_queue                                               //Newly designed class
{

	private:
		Thread* my_thread;                             //Private variables declaration
		My_queue* next;

	public:

		My_queue()				//Constructor
		{ 
			my_thread = NULL;
			next = NULL;
		}

		void set_thread(Thread* _thread)         //Set thread variable in queue object
		{
			my_thread = _thread;
			next = NULL;
		}

		void add_to_queue(Thread *this_thread)   //Add element at the end of the queue
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
				temp->set_thread(this_thread);
			}
			else
			{
				my_thread = this_thread;
			}
					
			
		}

		Thread* remove_from_queue()      //Remove first element from the queue
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

				delete (void *)(remove_next);
			}
			
			return returned_thread;
		}

		
};
