/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/




unsigned int ContFramePool::frame_traversal(unsigned int frame_no, unsigned int _n_frames) {

    unsigned int frame_number = frame_no;
    while(((get_state(frame_number) == FrameState::Used) || (get_state(frame_number) == FrameState::HoS)) && (frame_number < base_frame_no + this->nframes))    {
        frame_number++;
    }

    int count = 1;
    frame_no = frame_number;
    while((count<_n_frames) && (frame_number < base_frame_no + this->nframes))
    {
        if(get_state(frame_number) == FrameState::Free)
        {
                count++;
                frame_number++;
        }
        else
        {
                break;
        }

    }


    if (count == _n_frames)
    {
        return(frame_no);
    }

    if(frame_number < (base_frame_no + this->nframes))
    {

        frame_no = frame_traversal(frame_number, _n_frames);
        return(frame_no);
    }
    else
    {
        return(0);
    }


}

ContFramePool* ContFramePool::frame_pool_manager;    

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _nframes,
                             unsigned long _info_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Bitmap must fit in a single frame!
    assert(_nframes <= FRAME_SIZE * 4);

    base_frame_no = _base_frame_no;
    nframes = _nframes;
    nFreeFrames = _nframes-1;
    info_frame_no = _info_frame_no;

    frame_pool_manager_next = NULL;

    if(frame_pool_manager == NULL)
    {
    	frame_pool_manager = this;
    }
    else
    {
       ContFramePool *temp = frame_pool_manager->frame_pool_manager_next;
       while(temp != NULL)
       {
          temp = temp->frame_pool_manager_next;
       }
       if(temp == NULL)
       {
		temp = this;
       }

    }    

    // If _info_frame_no is zero then we keep management info in the first
    //frame, else we use the provided frame to keep management info
    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }
   
    // Everything ok. Proceed to mark all frame as free.
    for(unsigned int fno = base_frame_no; fno < base_frame_no + _nframes; fno++) {
        set_state(fno, FrameState::Free);
    }
   
    // Mark the first frame as being used if it is being used
    if(_info_frame_no == 0) {
        set_state(base_frame_no, FrameState::HoS);
        nFreeFrames--;
    }

    //Console::puts("ContframePool::Constructor not implemented!\n");
    //assert(false);
}

ContFramePool::FrameState ContFramePool::get_state(unsigned long _frame_no) {
    _frame_no = _frame_no - base_frame_no;
    unsigned int bitmap_index = _frame_no / 4;
    int bit_manipulation = _frame_no % 4;

    unsigned char mask = 0x1 << (bit_manipulation*2);
    unsigned char two_bit_mask = mask | 0x1 << ((bit_manipulation*2)+1);
 

    if(countSetBits(bitmap[bitmap_index] & two_bit_mask) == 0)
    {
	return(FrameState::Free);
    }
    else if(countSetBits(bitmap[bitmap_index] & two_bit_mask) == 1)
    {
 	return(FrameState::Used);
    }
    else
    {
	return(FrameState::HoS);
    }    

}


short int ContFramePool::countSetBits(unsigned int n)
{
    unsigned int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

void ContFramePool::set_state(unsigned long _frame_no, FrameState _state) {
    _frame_no = _frame_no - base_frame_no;
    unsigned int bitmap_index = _frame_no / 4;
    int bit_manipulation = _frame_no % 4;
    unsigned char mask;
    unsigned char two_bit_mask; 

    switch(_state) {
    case FrameState::HoS:
      mask = 0x1 << (bit_manipulation*2);
      two_bit_mask = mask | 0x1 << ((bit_manipulation*2)+1);
      bitmap[bitmap_index] |= two_bit_mask;
      break;
    case FrameState::Used:
      mask = 0x1 << ((bit_manipulation*2)+1);
      bitmap[bitmap_index] |= mask;
      two_bit_mask = 0x1 << (bit_manipulation*2);
      bitmap[bitmap_index] &= ~(two_bit_mask);
      break;
    case FrameState::Free:
      mask = 0x1 << (bit_manipulation*2);
      two_bit_mask = mask | 0x1 << ((bit_manipulation*2)+1);
      bitmap[bitmap_index] &= ~(two_bit_mask);
      break;
    }

}


unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!


    // Any frames left to allocate?
    assert(nFreeFrames > _n_frames);

    // Find a frame that is not being used and return its frame index.
    // Mark that frame as being used in the bitmap.
    unsigned int frame_final_number = this->base_frame_no;

    frame_final_number = frame_traversal(frame_final_number, _n_frames);

    int count = 0;
    if(frame_final_number > 0)
    {
                while(count != _n_frames)
                {
                        // We don't need to check whether we overrun. This is handled by assert(nFreeFrame>0) above.
                        if(count == 0)
			{
                        	set_state(frame_final_number+count, FrameState::HoS);
                        	nFreeFrames--;
                        	count++;
			}
			else
			{

                        	set_state(frame_final_number+count, FrameState::Used);
                        	nFreeFrames--;
                        	count++;
			}
                }
    }

    return (frame_final_number);



    // Console::puts("ContframePool::get_frames not implemented!\n");
    // assert(false);
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Mark all frames in the range as being used.
    set_state(_base_frame_no, FrameState::HoS);
    for(int fno = _base_frame_no+1; fno < _base_frame_no + _n_frames; fno++){
        set_state(fno, FrameState::Used);
    }

   // Console::puts("ContframePool::mark_inaccessible not implemented!\n");
   // assert(false);
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{

    ContFramePool *temp = frame_pool_manager->frame_pool_manager_next;
   
         if((_first_frame_no>frame_pool_manager->base_frame_no) && (_first_frame_no<(frame_pool_manager->base_frame_no + frame_pool_manager->nframes)))
	  {
		   if(frame_pool_manager->get_state(_first_frame_no) == FrameState::HoS)
		   {
  			      frame_pool_manager->set_state(_first_frame_no, FrameState::Free);
 		   }

 		   while(frame_pool_manager->get_state(++_first_frame_no) == FrameState::Used)
  	           {
 			      frame_pool_manager->set_state(_first_frame_no, FrameState::Free);
  		   }
 	  }
	  else
	  {
		   while(1)
       		   { 
          	   	if((_first_frame_no>(temp->base_frame_no)) && (_first_frame_no<(temp->base_frame_no + temp->nframes)))
			{
				if(temp->get_state(_first_frame_no) == FrameState::HoS)
		                {
                	              temp->set_state(_first_frame_no, FrameState::Free);
                  		}

                   		while(temp->get_state(++_first_frame_no) == FrameState::Used)
                   		{
                              	      temp->set_state(_first_frame_no, FrameState::Free);
                   		}
				break;
			}
			else
			{
				if(temp!=NULL)
				{
					temp = temp->frame_pool_manager_next;
				}
				else
				{
					Console::puts("Release Frames:: Frames not found in the available framepool!\n");
					break;
				}

			}
		   }
	    }
			

		

    // TODO: IMPLEMENTATION NEEEDED!
   // Console::puts("ContframePool::release_frames not implemented!\n");
   //  assert(false);
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    int i = _n_frames/16384;    
    return(i+1);

    // TODO: IMPLEMENTATION NEEEDED!
   // Console::puts("ContframePool::need_info_frames not implemented!\n");
   //  assert(false);
}
