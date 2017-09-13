### Programming Assignment 1
#### Brennon Lee
#### Email: brle1617@colorado.edu
#### Student ID: 103419905
---

**1. arch/x86/kernel/simple_add.c** <br />
```
  This file is our system call which refers to the number 333. It takes in two integers and a pointer integer to the result.
  When called, this function adds the two integers and that result is stored inside of our pointer result which gets printed
  as a kernel messege as well as printed back in the user space when it gets returned.
```
**2. arch/x86/kernel/Makefile** <br />
```
  The Makefile is where the helloworld and simple_add files were added so they could be compiled.
```
**3. arch/x86/entry/syscalls/syscall_64.tbl** <br />
```
  This is our kernel table that holds our system calls.
 ``` 
**4. include/linux/syscalls.h** <br />
```
  This is the header file which defines/ delcares our helloworld and simple_add system calls.
```
**5. /var/log/syslog** <br />
```
  This file is where the kernel messages are logged at. 
```  
**6. simple_addtest.c** <br />
```
  This is the source code to test simple_add system call
``` 
  ## To build & run
1. Compile simple_addtest.c by the following:
  ```
   gcc -o simple_addtest simple_addtest.c
  ```
2. Run the test program by: 
  ```
  ./simple_addtest
  ```
3. See result in terminal and then check kernel messages by: <br />
  ```
    dmesg
  ```
  or 
  ```
    sudo tail /var/log/syslog  
  ```
