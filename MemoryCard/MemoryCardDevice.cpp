/*

 MemoryCard - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include "MemoryCardDevice.h"

#define SD_CARD_CHIP_SELECT_PIN 8 // For use with SparkFun MicroSD shield

#define MAX_COMPONENT_LEN 12 // What is max length?
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

bool getNextPathComponent(char *path, unsigned int *p_offset,
			  char *buffer) {
  /*

    Parse individual path components from a path.

      e.g. after repeated calls '/foo/bar/baz' will be split
           into 'foo', 'bar', 'baz'.

    This is similar to `strtok()` but copies the component into the
    supplied buffer rather than modifying the original string.


    `buffer` needs to be PATH_COMPONENT_BUFFER_LEN in size.

    `p_offset` needs to point to an integer of the offset at
    which the previous path component finished.

    Returns `true` if more components remain.

    Returns `false` if this is the last component.
      (This means path ended with 'foo' or 'foo/'.)

   */

  // TODO: Have buffer local to this function so we know it's the
  //       correct length?

  int bufferOffset = 0;

  int offset = *p_offset;

  // Skip root or other separator
  if (path[offset] == '/') {
    offset++;
  }
  
  while (bufferOffset < MAX_COMPONENT_LEN
	 && (path[offset] != '/')
	 && (path[offset] != '\0')) {
    buffer[bufferOffset++] = path[offset++];
  }

  buffer[bufferOffset] = '\0';

  // Skip trailing separator
  if (path[offset] == '/') {
    offset++;
  }

  *p_offset = offset;

  return (path[offset] != '\0');
}




void MemoryCardDevice::begin() {
  /*

    Performs the initialisation required by the sdfatlib library.

    Does not return if initialisation fails.

   */
  if (!(card.init(SPI_HALF_SPEED, SD_CARD_CHIP_SELECT_PIN) 
	&& volume.init(card) && root.openRoot(volume))) {
    while (true) {
      // Bail
    }
  }
}


boolean MemoryCardDevice::exists(char *filepath) {
  /*
   */
  return exists(filepath, root);
}


boolean callback_pathExists(SdFile& parentDir, char *filePathComponent, 
			    boolean isLastComponent, void *object) {
  /*
  */
  SdFile child;

  boolean exists = child.open(parentDir, filePathComponent, O_RDONLY);
  
  if (exists) {
     child.close(); 
  }
  
  return exists;
}


boolean callback_makeDirPath(SdFile& parentDir, char *filePathComponent, 
			     boolean isLastComponent, void *object) {
  /*
  */
  boolean result = false;
  SdFile child;
  
  result = callback_pathExists(parentDir, filePathComponent, isLastComponent, object);
  if (!result) {
    result = child.makeDir(parentDir, filePathComponent);
  } 
  
  return result;
}



boolean walkPath(char *filepath, SdFile& parentDir,
                 boolean (*callback)(SdFile& parentDir, char *filePathComponent, boolean isLastComponent, void *object), void *object = NULL) {
  /*
   */


  SdFile subfile1;
  SdFile subfile2;

  char buffer[PATH_COMPONENT_BUFFER_LEN]; 

  unsigned int offset = 0;

  SdFile *p_parent;
  SdFile *p_child;

  SdFile *p_tmp_sdfile;  
  
  p_child = &subfile1;
  
  p_parent = &parentDir;

  while (true) {

    boolean moreComponents = getNextPathComponent(filepath, &offset, buffer);

    boolean shouldContinue = callback((*p_parent), buffer, !moreComponents, object);

    if (!shouldContinue) {
      // TODO: Don't repeat this code?
      // If it's one we've created then we
      // don't need the parent handle anymore.
      if (p_parent != &parentDir) {
        (*p_parent).close();
      }
      return false;
    }
    
    boolean exists = (*p_child).open(*p_parent, buffer, O_RDONLY);

    // If it's one we've created then we
    // don't need the parent handle anymore.
    if (p_parent != &parentDir) {
      (*p_parent).close();
    }
    
    // Handle case when it doesn't exist and we can't continue...
    if (exists) {
      // We alternate between two file handles as we go down
      // the path.
      if (p_parent == &parentDir) {
        p_parent = &subfile2;
      }

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;
    } else {
      return false;
    }
    
    if (!moreComponents) {
      // TODO: Check if this check should be earlier.
      break;
    }
  }
  
  (*p_parent).close(); // TODO: Return/ handle different?

  return true;
}

boolean callback_openPath(SdFile& parentDir, char *filePathComponent, 
			  boolean isLastComponent, void *object) {
  /*
  */
  if (isLastComponent) {
    MemoryCardDevice *p_MemoryCard = static_cast<MemoryCardDevice*>(object);
    p_MemoryCard->file.open(parentDir, filePathComponent,
			    p_MemoryCard->fileOpenMode);
    // TODO: Return file open result?
    return false;
  }
  return true;
}


boolean MemoryCardDevice::open(char *filepath,
			       boolean write, boolean append) {
  /*
   */

  // TODO: Allow for read&write? (Possibly not as it requires seek.)

  uint8_t oflag = O_RDONLY;

  if (write) {
    oflag = O_CREAT | O_WRITE;
    if (append) {
      oflag |= O_APPEND;
    } else {
      oflag |= O_TRUNC;
    }
  }
  
  fileOpenMode = oflag;
  walkPath(filepath, root, callback_openPath, this);
}


boolean MemoryCardDevice::exists(char *filepath, SdFile& parentDir) {
  /*
   */
  return walkPath(filepath, parentDir, callback_pathExists);
}


boolean MemoryCardDevice::makeDir(char *filepath) {
  /*
  
    A rough equivalent to `mkdir -p`.
  
   */
  return walkPath(filepath, root, callback_makeDirPath);
}


boolean MemoryCardDevice::close() {
  /*
   */
  file.close();
}
 
