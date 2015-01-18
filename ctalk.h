//
//  ctalk.h
//  libctalk
//
//  Created by Alec Zadikian on 8/12/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#ifndef libctalk_ctalk_h
#define libctalk_ctalk_h

/*
These functions send and receive individual "messages" (arrays of bytes of any value) preceded by headers containing variable length quantities used to share the length in bytes of each message. Messages are guaranteed to be received separately and in completeness by individual calls to cTalkRecv on the receiving end. Messages sent by cTalkSend MUST be received by cTalkRecv in order to be properly interpreted, and cTalkRecv will only properly receive data sent by cTalkSend. They return the number of bytes sent or received, or 0 if there is an error (see below).
*/
//======

// Sends the data in message over file descriptor fd with a header indicating the size, messageLen. Returns 0 (error) if the system cannot send the data.

// cTalkSendF takes a function parameter f for sending data using a custom function.
// f must take a header, its length, the message, and the message length and send the header immediately preceding the message (similar to writev). It must return the number of bytes sent, or -1 upon a sending error.
//
size_t cTalkSend(int fd, unsigned char* message, size_t messageLen);
size_t cTalkSendF(size_t (*f)(unsigned char* f_header, size_t f_headerLen, unsigned char* f_message, size_t f_messageLen), unsigned char* message, size_t messageLen);

// Receives data on file descriptor fd without header and puts it in buffer. For the purpose of security and/or stability, a maximum buffer size bufferLen must be specified. Returns 0 (error) if the received data header indicates a size greater than bufferLen or if data with an invalid header is received. In either case, the system read buffer for fd is NOT flushed.

// cTalkRecvF takes a function parameter for receiving data using a custom function.
// f must take a buffer and its length, bufferLen, and read bufferLen bytes into the buffer (similar to recv or read). It must return the number of bytes read, or a number less than or equal to zero upon a reading error.
//
size_t cTalkRecv(int fd, unsigned char* buffer, size_t bufferLen);
size_t cTalkRecvF(size_t (*f)(unsigned char* f_buffer, size_t f_bufferLen), unsigned char* buffer, size_t bufferLen);

//======

#endif
