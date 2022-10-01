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
   page_directory = (unsigned long *) (kernel_mem_pool->get_frames(1) * Machine::PAGE_SIZE);
   unsigned long *page_table =  (unsigned long *) (kernel_mem_pool->get_frames(1) * Machine::PAGE_SIZE);

   unsigned long address = 0;
   unsigned int i;

   for(i=0; i<1024; i++)
   {
	// page_table[i] = (address << 12) | 3;
	page_table[i] = address | 3;
	address = address + 4096; 
   }
 
   page_directory[0] = (unsigned long) page_table;
   // page_directory[0] = (page_directory[0] << 12) | 3;
   page_directory[0] = page_directory[0] | 3;

   for(i=1; i<1024; i++)
   {
	page_directory[i] = 0 | 2;
   }

   //assert(false);
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   current_page_table = this;
   write_cr3((unsigned long) current_page_table->page_directory);
   paging_enabled = 0;
   //assert(false);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   write_cr0(read_cr0() | 0x80000000);
   //assert(false);
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{

   unsigned long fault_add = read_cr2();
   Console::puts(" In handle fault, fault_add = "); Console::puti(fault_add); Console::puts("\n");

   unsigned long directory = (unsigned long) (fault_add/(4 MB));

   Console::puts(" In handle fault, directory = "); Console::puti(directory); Console::puts("\n");

   unsigned long page_no = (unsigned long) (fault_add/4096);
   page_no = (int) (page_no % 1024);

   Console::puts(" In handle fault, page_no = "); Console::puti(page_no); Console::puts("\n");
   Console::puts(" In handle fault, in outside current_page_table->page_directory[directory] = "); Console::puti(current_page_table->page_directory[directory]); Console::puts("\n");

   if(((current_page_table->page_directory[directory] & 3) != 3))
   {  
	unsigned long *page_table =  (unsigned long *) (kernel_mem_pool->get_frames(1) * Machine::PAGE_SIZE);
	unsigned int i;

	Console::puts(" In handle fault, in if page_table = "); Console::puti((unsigned long) page_table); Console::puts("\n");

	for(i=0; i<1024; i++)
	{
		page_table[i] = 0 | 2; 
	}

        //current_page_table->page_directory[directory] = ((unsigned long) page_table << 12) | 3;
        //page_table[page_no] = (((process_mem_pool->get_frames(1)) * Machine::PAGE_SIZE) << 12) | 3;

        current_page_table->page_directory[directory] = ((unsigned long) page_table) | 3;
	Console::puts(" In handle fault, in if current_page_table->page_directory[directory] = "); Console::puti(current_page_table->page_directory[directory]); Console::puts("\n");
        page_table[page_no] = ((process_mem_pool->get_frames(1)) * Machine::PAGE_SIZE) | 3;
   }
   else
   {   
	Console::puts(" In handle fault, in else = "); Console::puts("\n");
        //unsigned long *page_table = (unsigned long *) ((current_page_table->page_directory[directory]) >> 12);
        //page_table[page_no] = (((process_mem_pool->get_frames(1)) * Machine::PAGE_SIZE) << 12) | 3;
	Console::puts(" In handle fault, in else current_page_table->page_directory[directory] = "); Console::puti(current_page_table->page_directory[directory]); Console::puts("\n");

        unsigned long *page_table = (unsigned long *) ((current_page_table->page_directory[directory] >> 12) << 12);
	Console::puts(" In handle fault, in else page_table_address read = "); Console::puti((unsigned long) page_table); Console::puts("\n");
        page_table[page_no] = ((process_mem_pool->get_frames(1)) * Machine::PAGE_SIZE) | 3;
   }

   Console::puts(" In handle fault, page_directory value = "); Console::puti((current_page_table->page_directory[directory] & 3)); Console::puts("\n");

  //assert(false);
  Console::puts("handled page fault...................here it is\n");

}

