/*
 * mps.c
 *
 *  Created on: 18 Dec 2019
 *      Author: addod
 */


#include <mps.h>

uint8_t out_stream[30]; //30 bytes in total is the full MiWII command including the payload, Will increase this later if wrong

/*
 * Should put this in a struct and get the bytes as a packet to the BL model
 *
 * */
int mps_send(uint8_t msgID, void * payload, uint8_t size)
{
    uint32_t ix = 5;

    out_stream[0] = '$';
    out_stream[1] = 'M';
    out_stream[2] = '<'; // direction of the cmd to the qcopter
    out_stream[3] = size;
    out_stream[4] = msgID;
    uint8_t checksum = size ^ msgID;
    uint8_t * payloadPtr = (uint8_t*) payload;

    uint8_t i;
    for (i = 0; i < size; ++i)
    {
        uint8_t b = *(payloadPtr++);
        checksum ^= b;
        out_stream[ix] = b;
        ix++;
    }
    out_stream[ix] = checksum;

}
