#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

#define MB * (0x1 << 20)

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
  kernel_mem_pool = _kernel_mem_pool;
  process_mem_pool = _process_mem_pool;
  shared_size = _shared_size;
  // assert(false);
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   page_directory = (unsigned long *) (process_mem_pool->get_frames(1) * Machine::PAGE_SIZE);
   unsigned long *page_table =  (unsigned long *) (process_mem_pool->get_frames(1) * Machine::PAGE_SIZE);

   unsigned long address = 0;

   for(int i=0; i<1024; i++)
   {
       
	page_table[i] = address | 3;
        address = address + 4096;
   }
 
//   page_directory[0] = (unsigned long) page_table;
   page_directory[0] = (unsigned long) page_table | 3;

   for(int i=1; i<1024; i++)
   {
	page_directory[i] = 0 | 2;
   }
   page_directory[1023] = (unsigned long) page_directory | 3;

   for(int i=0; i< VM_POOL_SIZE; i++)
   {
	reg_vm_pool[i] = NULL;
   }

   vm_pool_no = 0;
   //assert(false);
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   current_page_table = this;
   write_cr3((unsigned long) current_page_table->page_directory);
   //assert(false);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   write_cr0(read_cr0() | 0x80000000);
   paging_enabled = 1;
   //assert(false);
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{

   unsigned long fault_add = read_cr2();

   unsigned long directory = fault_add >> 22;
   unsigned long page_no = fault_add >> 12;

   unsigned long *page_table = NULL;
   unsigned long error_code = _r->err_code;

   unsigned long * current_page_directory = (unsigned long *) 0xFFFFF000;

   if((error_code & 1) == 0) 
   {
	int index = -1;
	VMPool ** vm_pool = current_page_table->reg_vm_pool;
	for (int i = 0; i < current_page_table->vm_pool_no; i++)
	{
		if(vm_pool[i] != NULL) 
		{
			if (vm_pool[i]->is_legitimate(fault_add))
			{
				index = i;
				break;
			}
		}
	}
   
	assert(!(index < 0));
	

	if(((current_page_directory[directory] & 3) != 3))
	{
  
	//	unsigned long *page_table =  (unsigned long *) (process_mem_pool->get_frames(1) * Machine::PAGE_SIZE);
	        unsigned long *page_table = NULL;
	        current_page_directory[directory] = (unsigned long) ((process_mem_pool->get_frames(1) * Machine::PAGE_SIZE) | 3);
		page_table = (unsigned long *) (0xFFC00000 | (directory << 12));
		unsigned int i;

		for(i=0; i<1024; i++)
		{
			page_table[i] = 0 | 2; 
		}

        	page_table[page_no & 0x3FF] = ((kernel_mem_pool->get_frames(1)) * Machine::PAGE_SIZE) | 3;
		
   	}
   	else
	{   

        	unsigned long *page_table = (unsigned long *) (0xFFC00000 | (directory << 12)) ;

        	page_table[page_no & 0x3FF] = ((process_mem_pool->get_frames(1)) * Machine::PAGE_SIZE) | 3;
		
	}
   }

  //assert(false);
  Console::puts("handled page fault\n");

}



void PageTable::register_pool(VMPool * _vm_pool)
{
    if (vm_pool_no < VM_POOL_SIZE)
    {
	reg_vm_pool[vm_pool_no++] = _vm_pool;
    	Console::puts("registered VM pool\n");
    }
    else
    {
        Console::puts(" VM pool full\n");
    }
	
    //assert(false);
}

void PageTable::free_page(unsigned long _page_no)
{

    unsigned long directory = _page_no >> 22;
    unsigned long page_no = _page_no >> 12;

    unsigned long *page_table = (unsigned long *) (0xFFC00000 | (directory << 12));

    unsigned long frame_no = page_table[page_no & 0x3FF] / (Machine::PAGE_SIZE);
    process_mem_pool->release_frames(frame_no);

    page_table[page_no & 0x3FF] = 0 | 2;

    //assert(false);
    Console::puts("freed page\n");
}

/*
unsigned long* PageTable::PDE_address(unsigned long addr) {


   unsigned long offset = ((addr << 20) >> 22) << 2;

   unsigned long Pde_address = (1023 << 22);
    
   Pde_address | = ((1023 << 22) >> 10);
   Pde_address | = offset; 

   return(Pde_address);
}


unsigned long* PageTable::PTE_address(unsigned long addr) {

   unsigned long offset_plus_pte = ((addr << 10) >> 12) << 2;

   unsigned long Pte_address = (1023 << 22);

   Pte_address |= offset_plus_pte;

   return(pte_address);

}
*/
