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

    current_position = 0;

    //assert(false);
}


File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");

    file_id = _id;
    current_position = 0;
    file_system = _fs;

    Inode *temp = _fs->LookupFile(_id);

    if(temp != NULL)
    {
	block_no = temp->block_no;
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

    if (block_no == -1 || file_system == NULL) {
        Console::puts("File not intialized, file cannot be read \n");
        return 0;
    }


    int read_counter = 0;
    int chars_to_read = _n;

    memset(block_cache,0,512);
    
    file_system->disk->read(block_no, (unsigned char *)block_cache);

    while (!EoF() && (chars_to_read > 0))
    {
        _buf[read_counter] = block_cache[current_position];
        chars_to_read -= 1;
        read_counter  += 1; 
        current_position += 1;
    }

   // assert(false);
    return read_counter;	
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");

    if (block_no == -1 || file_system == NULL) {
        Console::puts("File not intialized, cannot write to the file \n");
        return 0;
    }

    int write_counter = 0;
    int chars_to_write = _n;

//    Console::puts("writing to file.... A\n");

    memset(block_cache,0,512);
//    Console::puts("writing to file.... A - B"); Console::puti(block_no);Console::puts("\n");

    file_system->disk->read(block_no, block_cache);
//    Console::puts("writing to file.... B\n");
    //Console::puts("current_position = "); Console::puti(current_position);
    while (!EoF() && (chars_to_write > 0))
    {
   // 	Console::puts("writing to file.... C\n");
        block_cache[current_position] = _buf[write_counter];
        chars_to_write -= 1;
        write_counter  += 1; 
        current_position += 1;

    }
//    	Console::puts("writing to file.... D\n");

    file_system->disk->write(block_no, block_cache);
//    	Console::puts("writing to file.... E\n");

    return write_counter;
  //  assert(false);
}

void File::Reset() {
    Console::puts("resetting file\n");
    
    current_position = 0;
  //  assert(false);
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    
    if(current_position > 512)
	return true;

    return false;
  //  assert(false);
   
}
