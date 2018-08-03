#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
/*********************************************************************
   *
   * These C functions use patterns and functionality often found in
   * operating system code. Your job is to implement them. Of course you
   * should write test cases. However, do not hand in your test cases
   * and (especially) do not hand in a main() function since it will
   * interfere with our tester.
   *
   * Additional requirements on all functions you write:
   *
   * - you may not refer to any global variables
   *
   * - you may not call any functions except those specifically
   *   permitted in the specification
   *
   * - your code must compile successfully on CADE lab Linux
   *   machines when using:
   *
   * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition -c assign1.c 




   *
   * NOTE 1: Some of the specifications below are specific to 64-bit
   * machines, such as those found in the CADE lab.  If you choose to
   * develop on 32-bit machines, some quantities (the size of an
   * unsigned long and the size of a pointer) will change. Since we will
   * be grading on 64-bit machines, you must make sure your code works
   * there.
   *
   * NOTE 2: You should not need to include any additional header files,
   * but you may do so if you feel that it helps.
   *
   * HANDIN: submit your finished file, still called assign1.c, in Canvas
   *
   *
   *********************************************************************/

  /*********************************************************************
   *
   * byte_sort()
   *
   * specification: byte_sort() treats its argument as a sequence of
   * 8 bytes, and returns a new unsigned long integer containing the
   * same bytes, sorted numerically, with the smaller-valued bytes in
   * the lower-order byte positions of the return value
   * 
   * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
   *
   *********************************************************************/

  unsigned long byte_sort(unsigned long arg) {
    unsigned long result;
    unsigned char arrayOfByte[8];
    int i = 0, j, temp;
    for (i = 0; i < 8; i++) {
      arrayOfByte[i] = arg & 0xff;
      arg = arg >> 8;
    }
    for (i = 0; i < 8; i++)
      for (j = 0; j < 8 - i - 1; j++)
        if (arrayOfByte[j] < arrayOfByte[j + 1]) {
          temp = arrayOfByte[j];
          arrayOfByte[j] = arrayOfByte[j + 1];
          arrayOfByte[j + 1] = temp;
        }
    result = arrayOfByte[0];
    for (i = 1; i < 8; i++) {
      result = (result << 8) + arrayOfByte[i];
    }
    return result;
  }

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort(unsigned long arg) {
  unsigned long result;
  unsigned char arrayOfByte[16];
  int i = 0, j, temp;
  for (i = 0; i < 16; i++) {
    arrayOfByte[i] = arg & 0xf;
    arg = arg >> 4;
  }
  for (i = 0; i < 16; i++)
    for (j = 0; j < 16 - i - 1; j++)
      if (arrayOfByte[j] < arrayOfByte[j + 1]) {
        temp = arrayOfByte[j];
        arrayOfByte[j] = arrayOfByte[j + 1];
        arrayOfByte[j + 1] = temp;
      }
  result = arrayOfByte[0];
  for (i = 1; i < 16; i++) {
    result = (result << 4) + arrayOfByte[i];
  }
  return result;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *  
 *********************************************************************/

struct elt {
  char val;
  struct elt * link;
};

struct elt * name_list(void) {
  char name[] = "";
  struct elt * prev;
  struct elt * first = NULL;
  struct elt * cur = NULL;
  int i = 0;
  while (name[i] != '\0') {
    struct elt * ptr = NULL;
    ptr = malloc(sizeof(struct elt));
    if (name[i] == 'S')
      ptr = NULL;
    if (ptr == NULL) {
      cur = first;
      //printf("Was unable to allocate memory for char '%c'. Going to free heap and exit.\n", name[i]);
      while (cur != NULL) {
        //printf("Freed %c located at %d\n", cur -> val, cur);
        free(cur);
        //printf("Let's look inside cur (%d) : %c \n", cur, *cur);
        cur = first ->link;
        first = first ->link;
      }
      return NULL;
    }

    ptr ->val = name[i];
    ptr ->link = NULL;
    if (i == 0) {
      first = ptr;
      //printf("Allocated %d to %c \n",ptr, *ptr);
    } else {
      prev ->link = ptr;
      //printf("Allocated %d to %c \n",ptr, *ptr);
    }
    prev = ptr;
    i++;
  }
  return first;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert(enum format_t mode, unsigned long value) {
  unsigned long quo;
  int i, j, temp;
  switch (mode) {
  case OCT:
    {
      char octal[100];
      i = 0,
      quo = value;
      while (quo != 0) {
        octal[i++] = (quo % 8) + 48;
        quo = quo / 8;
      }
      if (value != 0)
      {
        temp = i-1;
        for(j=1;j<32-temp;j++)
          putc('0',stdout);
        for (j = i - 1; j >= 0; j--)
          putc(octal[j], stdout);
      }        
      else
        for(j=1;j<32;j++)
          putc('0',stdout);
      putc('\n', stdout);
    }
    break;

  case BIN:
    {
      char binary[100];
      i = 0,
      quo = value;
      while (quo != 0) {
        binary[i++] = (quo % 2) + 48;
        quo = quo / 2;
      }
      temp=i-1;
      if (value != 0){
        for(j=1;j<64-temp;j++)
          putc('0',stdout);
        for (j = i - 1; j >= 0; j--)
          putc(binary[j], stdout);
      }
        
      else
        for(j=1;j<64;j++)
          putc('0',stdout);
      putc('\n', stdout);
    }

    break;
  case HEX:
    {
      char hexa[18];
      memset(hexa, '0', 18);
      hexa[1] = 'x';
      i = 17;
      quo = value;
      while (quo != 0) {
        temp = quo % 16;
        if (temp < 10)
          temp = temp + 48;
        else
          temp = temp + 55;
        hexa[i--] = temp;
        quo = quo / 16;
      }
      for (j = 0; j < 18; j++)
        putc(hexa[j], stdout);
    }
    putc('\n', stdout);
    break;

  default:
    break;
  }
}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 * 
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/
static void checkAndDelete(const char filename[]) {
  int try_count = 1, file_operation = -1;
  while (file_operation == -1 && try_count <= 5) {
    file_operation = syscall(SYS_access, filename, F_OK);
    try_count++;
  }
  if (file_operation == 0) {
    //file exists, delete it
    //printf("---File exists, deleting it---\n");
    file_operation = -1, try_count = 1;
    while (file_operation == -1 && try_count <= 5) {
      file_operation = syscall(SYS_unlink, filename);
      try_count++;
    }
    if (file_operation == -1) {
      printf("Was unable to delete the file. \n");
      return;
    } else {
      // printf("---Deleted the file---\n");
    }
  }
}
void draw_me(void) {
  char image[1000] = "             ***                  *** \n            *****                ***** \n            *****                ***** \n             ***                  *** \n \n \n                        *** \n       ***                              *** \n         ***                          *** \n           ***                      ***  \n              ********************** \n                 **************** \n";

  //LOGIC: Check if file exists, delete if it does. Create a new file, write into it and close it. Handle errors and clean up if any of the calls fail.

  int try_count = 1, file_operation = -1, fd;
  checkAndDelete("me.txt");

  //file was either deleted or didn't exist, now create it and open it
  fd = -1, try_count = 1;
  while (fd == -1 && try_count <= 5) {
    //umask(0000);
    fd = syscall(SYS_open, "me.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777);
    try_count++;
  }
  if (fd == -1) {
    //CLEAN UP
    printf("Was unable to create and open the file. Going to delete it if it was created.\n");
    checkAndDelete("me.txt");
    return;
  } else {
    //printf("---Opened the file---\n");
  }
  //file was created and opened, now write into it
  file_operation = -1, try_count = 1;
  while ((file_operation == -1 || file_operation < abs(strlen(image) - 1))&& try_count <= 5) {
    syscall(SYS_truncate,"me.txt",0);
    file_operation = syscall(SYS_write, fd, image, strlen(image) - 1);
    try_count++;
  }
 
  if (file_operation == -1 || file_operation < abs(strlen(image) - 1)) {
    //CLEAN UP
    printf("Was unable to write into the file. Going to delete it. \n");
    checkAndDelete("me.txt");
    return;
  } else {
    //printf("---Wrote into the file---\n");
  }

  //close the file
  file_operation = -1, try_count = 1;
  while (file_operation == -1 && try_count <= 5) {
    file_operation = syscall(SYS_close, fd);
    try_count++;
  }
  if (file_operation == -1) {
    //CLEAN UP
    printf("Was unable to close the file. Going to delete it. \n");
    checkAndDelete("me.txt");
    return;
  } else {
    //printf("---Closed the file---\n");
  }
}

/*
int main(void) {
  unsigned long result1 = byte_sort(0x0403deadbeef0201);
  printf("Result 1: 0x%lx\n", result1);

  unsigned long result2 = nibble_sort(0x0403deadbeef0201);
  printf("Result 2: 0x%lx\n", result2);

  struct elt * result3 = name_list();
  if (result3 != NULL)
    printf("Result 3: Pointer to first element is:   %ld\n", (long)result3);

  convert(66, 68);
  convert(67, 68);
  convert(68, 68);
  convert(66, 0xdeadbeef);
  convert(67, 0xdeadbeef);
  convert(68, 0xdeadbeef);

  draw_me();
  return 0;
}
*/
