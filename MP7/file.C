/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/


File::File() {
    Console::puts("Opening file.\n");

    position = 0;

    //assert(false);
}


File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");

    fd = _id;
    position = 0;
    file_system = _fs;

    Inode *temp = _fs->LookupFile(_id);

    if(temp != NULL)
    {
	c_block = temp->block;
    }
    else
    {
    	Console::puts("Opening file: File not initialized.\n");
    }
    //assert(false);
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");

    if (c_block == -1 || file_system == NULL) {
        Console::puts("File not intialized, can not read \n");
        return 0;
    }


    int read = 0;
    int bytes_to_read = _n;

    memset(block_cache,0,512);
    
    file_system->disk->read(c_block, (unsigned char *)block_cache);

    while (!EoF() && (bytes_to_read > 0))
    {
        _buf[read] = block_cache[position];
        bytes_to_read--;
        read++; 
        position++;
    }

   // assert(false);
    return read;	
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");

    if (c_block == -1 || file_system == NULL) {
        Console::puts("File not intialized, can not read \n");
        return 0;
    }


     //Console::puts("passed buffer "); Console::puts(_buf);Console::puts("\n");
    int write = 0;
    int bytes_to_write = _n;

//    Console::puts("writing to file.... A\n");

    memset(block_cache,0,512);
//    Console::puts("writing to file.... A - B"); Console::puti(c_block);Console::puts("\n");

    file_system->disk->read(c_block, block_cache);
//    Console::puts("writing to file.... B\n");
    //Console::puts("position = "); Console::puti(position);
    while (!EoF() && (bytes_to_write > 0))
    {
   // 	Console::puts("writing to file.... C\n");
        block_cache[position] = _buf[write];
        write++;
        position++;
        bytes_to_write--;

    }
//    	Console::puts("writing to file.... D\n");

    file_system->disk->write(c_block, block_cache);
//    	Console::puts("writing to file.... E\n");

    return write;
  //  assert(false);
}

void File::Reset() {
    Console::puts("resetting file\n");
    
    position = 0;
  //  assert(false);
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    
    if(position > 512)
	return true;

    return false;
  //  assert(false);
   
}
