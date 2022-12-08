/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */


/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

SimpleDisk* FileSystem::disk;

unsigned char FileSystem::free_block_bitmap[512];
unsigned long FileSystem::total_blocks;

unsigned long FileSystem::inode_blocks;
unsigned int FileSystem::size;


FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    disk = NULL;
    total_blocks = 0;
    inode_blocks = 0;
    size = 0;
    //assert(false);
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    
    /* Make sure that the inode list and the free list are saved. */
    //assert(false);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");
    if (disk == NULL)
    {
	disk = _disk;
    }
    /* Here you read the inode list and the free list into memory */
    return true; 
    //assert(false);
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    disk = _disk;
    size = _size;
    int i, j;    
    
    total_blocks = ((FileSystem::size)/SimpleDisk::BLOCK_SIZE) + 1;
    inode_blocks = ((total_blocks * sizeof(I_node)) / SimpleDisk::BLOCK_SIZE) + 1;

 
    for (j = 0; j < (total_blocks/8); j++)
    {
	free_block_bitmap[j] = 0;
    }

    for (i = 0; i < (inode_blocks/8); i++) 
    {
	free_block_bitmap[i] = 0xFF;
    }

    free_block_bitmap[i] = 0;


   for (j = 0; j < (inode_blocks%8); j++)
   {
	free_block_bitmap[i] |= (1<<j);
   }

   char block_buffer[512];
   memset(block_buffer, 0, 512);
   for(j = 0; j < total_blocks; j++)
   {
	disk->write(j, (unsigned char *)block_buffer);
   }
    
    //assert(false); 

    return true;
}

Inode * FileSystem::LookupFile(int _file_id) 
{
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");

    /* Here you go through the inode list to find the file. */
    for(int i=0; i < inode_blocks; i++)
    {
	memset(block_buffer, 0, 512);
        disk->read(i, (unsigned char *)block_buffer);
 	I_node* I_node_new = (I_node *)block_buffer;

	for(int j=0; j < MAX_INODES ; ++j)
	{
		if(I_node_new[j].file_id == _file_id)
		{
        	        return (I_node_new + j);				
		}
	}

    }

    //assert(false);
    return NULL;
}

bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */

    if (LookupFile(_file_id) != NULL)
    {
        Console::puts("File already exists with this id, please choose new id to create a new file\n");
        return false;
    }


    for (int i = 0; i < inode_blocks; i++) 
    {

        memset(block_buffer, 0, 512);        //set the block_bufferfer to 0, to be used in reading the disk.
        disk->read (i, (unsigned char *)block_buffer);
        I_node* I_node_new = (I_node *) block_buffer;

        for (int j = 0; j < MAX_INODES; j++) 
	{
            if (I_node_new[j].file_id == 0) 
	    {
                I_node_new[j].file_id = _file_id;
                I_node_new[j].block_no = GetFreeBlock();

   	        File* file = new File;
		
		file->file_id       = _file_id;
		file->block_no  = I_node_new[j].block_no;
		file->current_position = 0;
		file->file_system = this;		 		

		I_node_new[j].filepointer = file;	

                disk->write(i, (unsigned char *)block_buffer);
                return true;
            }
        }
    }
	
    return false;
    //assert(false);
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */


    if (LookupFile(_file_id) == NULL)
    {
        Console::puts("File does not exists with this id on file system \n");
        return false;
    }


    for (int i = 0; i<inode_blocks; i++)
    {
        memset(block_buffer, 0, 512);     
        disk->read (i, (unsigned char *)block_buffer);
        I_node* I_node_new = (I_node *) block_buffer;

        for (int j = 0; j < MAX_INODES; j++) 
	{
            if (I_node_new[j].file_id == _file_id) 
	    {
                I_node_new[j].file_id = 0;
                FreeBlock(I_node_new[j].block_no);
                I_node_new[j].block_no = 0;
		delete I_node_new[j].filepointer;

   	    	disk->write(i, (unsigned char *)block_buffer);
            	return true;
            }
                
           
        }
    }

  return false;
}


int FileSystem::GetFreeBlock() {

    // We need to reserve the first inode_blocks in the disk for file system management.
    //TODO

    for (int i = 0; i < (total_blocks / 8); i++) 
    {
        if (free_block_bitmap[i] != 0xFF) 
	{
            for (int j = 0; j < 8; j++) 
	    {
                if (!(free_block_bitmap[i] & (1 << j))) 
		{
                    free_block_bitmap[i] |= (1 << j);
                    int free_block_number = i*8 + j;
                    return free_block_number;
                }
            }
        }
    }

    return 0;
}


void FileSystem::FreeBlock(int block_no)
{
	int inode_block = block_no / 8;
	int inode_no = block_no % 8;

    	free_block_bitmap[inode_block] ^= (1 << inode_no);
}


