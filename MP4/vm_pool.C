/*
 File: vm_pool.C
 
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

#include "vm_pool.H"
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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table)
{


    base_address = _base_address;
    size = _size;
    frame_pool = _frame_pool;
    page_table = _page_table;
  
    region_no = 0;

    regions_info = (struct regions_info_ *) (base_address);

    page_table->register_pool(this);
//    assert(false);
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {

    unsigned long address;

    if (size == 0) {
	Console::puts("0 size invalid for allocation");
	return 0;
    }

    assert(region_no < MAX_REGIONS);


    unsigned frame_remainder = _size % (Machine::PAGE_SIZE);

    unsigned long frames = _size / (Machine::PAGE_SIZE);

    if (frame_remainder == 0)
	frames++;

    if (region_no == 0)
    {

	address = base_address;
	regions_info[region_no].base_address = address + Machine::PAGE_SIZE;
        regions_info[region_no].size = frames*(Machine::PAGE_SIZE);
        region_no++;
	return (address + Machine::PAGE_SIZE);
    }
    else
    {
	address = regions_info[region_no - 1].base_address + regions_info[region_no - 1].size;
    }


    regions_info[region_no].base_address = address;
    regions_info[region_no].size = frames*(Machine::PAGE_SIZE);

    region_no++;

//    assert(false);
    Console::puts("Allocated region of memory.\n");

    return(address);
}

void VMPool::release(unsigned long _start_address) {


   int current_region_no = -1;

   for (int i = 0; i < MAX_REGIONS; i++) 
   {
	if (regions_info[i].base_address == _start_address) 
	{
		current_region_no = i;
		break;
	}
   }

   assert(!(current_region_no < 0))
  
   unsigned int region_pages = ((regions_info[current_region_no].size) / (Machine::PAGE_SIZE));

   for (int i = 0; i < region_pages; i++)
   {
	page_table->free_page(_start_address);
        _start_address = _start_address + Machine::PAGE_SIZE;
   }

   for (int i = current_region_no; i < region_no - 1; i++)
   {
	regions_info[i] = regions_info[i+1];
   }
   region_no--;

   page_table->load();

   // assert(false);
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {

    unsigned long final_limit = base_address + size;
    
    if ((_address < final_limit) && (_address >= base_address))
	return true;

    return false;

  //  assert(false);
    Console::puts("Checked whether address is part of an allocated region.\n");
}

