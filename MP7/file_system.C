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


#define NODES_PER_BLOCK (BLOCK_SIZE/size(m_node));

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

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    disk = NULL;
    total_blocks = 0;
    m_blocks = 0;
    m_nodes = 0;
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
	return true;
    }
    else
    {
	return false;
    }
    /* Here you read the inode list and the free list into memory */
    
    //assert(false);
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    disk = _disk;
    size = _size;
    
    total_blocks = ((FileSystem::size)/Simple_disk::BLOCK_SIZE) + 1;
    m_nodes = (total_blocks/BLOCK_LIMIT) + 1;
    m_blocks = (m_nodes * sizeof(m_node)) / BLOCK_SIZE + 1;

 
    for (int j = 0; j < (total_blocks/8); j++)
    {
	block_map[j] = 0;
    }

    int i;    
    for (i = 0; i < (m_blocks/8); i++) 
    {
	block_map[i] = 0xFF;
    }

    block_map[i] = 0;


   for (int j = 0; j < (m_blocks%8); j++)
   {
	block_map[i] = block_map[i] | (1<<j);
   }


   char buf[512];
   memset(buf, 0, 512);
   for(int j = 0; j < total_blocks; j++)
   {
	disk->write(j, (unsigned char *)buf);
   }
    
    assert(false);
}

File * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");

    File* file = (File *) new File();

    char buf[512];

    memset(buf, 0, 512);

    for(int i=0; i < m_blocks; i++)
    {
	memset(buf, 0, 512);
        disk->read(i, (unsigned char *)buf);
 	m_node* m_node_l = (m_node *)buf;

	for(int j=0;j < NODES_PER_BLOCK;j++)
	{
		if(m_node_l[j].fd == _file_id)
		{
			file->fd       = _file_id;
			file->size     = m_node_l[j].size;
			file->c_block  = m_node_l[j].block[0];
			file->index    = 1;
			file->position = 0;

			for(int k=0; k < BLOCK_LIMIT; k++)
			{
				file->blocks[k] = m_node_l[j].block[k];
			}

			file->file_system = FILE_SYSTEM;
			if (file->file_system == NULL)
			{
                    		Console::puts("File system NULL in lookup");Console::puts("\n");
			}

	                Console::puts("Found file with id ");Console::puti(_file_id);Console::puts("\n");
        	        return file;				

		}
	}

    }

    /* Here you go through the inode list to find the file. */
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
        Console::puts("File already exists with this id, choose new id\n");
        return false;
    }


    char buf[512];
    memset(buf, 0, 512);

    for (int i = 0; i < m_blocks; i++) 
    {

        memset(buf, 0, 512);        //set the buffer to 0, to be used in reading the disk.
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;

        for (int j = 0; j < NODES_PER_BLOCK; j++) 
	{
            if (m_node_l[j].fd == 0) 
	    {
                m_node_l[j].fd = _file_id;

                m_node_l[j].block[0] = GetFreeBlock();
                Console::puts("get block "); Console::puti(m_node_l[j].block[0]);
                m_node_l[j].b_size   = 1;

                disk->write(i, (unsigned char *)buf);
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


    char buf[512];
    memset(buf,0,512);

    for (int i = 0; i<m_blocks; i++)
    {


        memset(buf, 0, 512);        //set the buffer to 0, to be used in reading the disk.
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;

        for (int j = 0; j < NODES_PER_BLOCK; j++) {
            if (m_node_l[j].fd == _file_id) {
                m_node_l[j].fd = 0;
                m_node_l[j].size = 0;
                m_node_l[j].b_size = 0;
                for (int k = 0; k < BLOCK_LIMIT; k++) {
                    if (m_node_l[j].block[k] != 0) {
                        FreeBlock(m_node_l[j].block[k]);
                    }
                    m_node_l[j].block[k] = 0;
                }
                disk->write(i, (unsigned char *)buf);
                return true;
            }
        }
    }

}



int FileSystem::GetFreeBlock() {

    // We need to reserve the first m_blocks in the disk for file system management.
    //TODO

    Console::puts("Total blocks "); Console::puti(total_blocks/8);Console::puts("\n");

    for (int i = 0; i < (total_blocks / 8); i++) {
        if (block_map[i] != 0xFF) {
            for (int j = 0; j < 8; j++) {
                if (block_map[i] & (1 << j)) {
                    continue;
                } else {
                    block_map[i] = block_map[i] | (1 << j);
                    int b= j + i*8;
                    Console::puts("Allocating block number");Console::puti(b);Console::puts("\n");
                    return b;
                }
            }
        }
    }
    Console::puts("returning block 0\n");
    return 0;
}


void FileSystem::FreeBlock(int block_no)
{
	int node = block_no / 8;
	int index = block_no % 8;

    block_map[node] = block_map[node] | (1 << index) ;
    block_map[node] = block_map[node] ^ (1 << index) ;

}


