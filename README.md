Formatted-I-O-Library
=====================

A formatted input/output library written in C.

Built on top of the input/output system calls provided by the system: read and write. 
The main motivation is to provide users with a higher level interface and to minimize the actual number of system calls issued by a program. The higher level interface lets the user handle the data according to its type and not consider it only as a block of bytes. 
A smaller number of system calls accelerates a priori the execution and is achieved through bufferization.
