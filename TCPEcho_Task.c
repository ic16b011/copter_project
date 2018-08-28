/*
 *  ======== TCPEcho_Task.c ========
 *  Author: Matthias Wenzl
 */
#include <stdbool.h>
#include <stdint.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Application headers */
#include "TCPEcho_Task.h"

//network headers
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/_stack.h>



/*
 *  ======== TCP Echo  ========
 *  Receives and sends back tcp packets
 */
void TCPEchoFxn(UArg arg0, UArg arg1)
{

    SOCKET   stcp = INVALID_SOCKET;
    SOCKET   stcpactive = INVALID_SOCKET;
    SOCKET   stcpbusy;
    struct   sockaddr_in sin;
    struct   timeval timeout;           /* Timeout struct for select */
    int      size,tmpfd/*temporary file descriptor*/;
    static  char     pBuf[1500];

    uint16_t lport = (uint16_t)arg0;
    if(lport == 0)
        goto leave;

    /* Allocate the file environment for this task - every network task must call this function */
    fdOpenSession( TaskSelf() );

    /* Create the main TCP listen socket */
    stcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( stcp == INVALID_SOCKET )
        goto leave;


    /* Set Port = lport */
    bzero( &sin, sizeof(struct sockaddr_in) );
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(lport);
    sin.sin_addr.s_addr = INADDR_ANY;/*listen on ony address*/

    /* Bind socket */
    if ( bind( stcp, (struct sockaddr *) &sin, sizeof(sin) ) < 0 )
        goto leave;

    /* Start listening */
    if ( listen( stcp, 1) < 0 )
        goto leave;

    /* Configure our timeout to be 15 seconds
     * This is an example, we do not use timeoute here.
     * Pass it as last parameter to select if you want to use
     * timeouts. */
    timeout.tv_sec  = 15;
    timeout.tv_usec = 0;

    /* Run until task is destroyed by the system */
    while(1) {
        fd_set ibits, obits, xbits;
        int    cnt;

        /* Clear the select flags */
        FD_ZERO(&ibits);
        FD_ZERO(&obits);
        FD_ZERO(&xbits);

        /* We examine the read bits of the stcp socket's file descriptor */
        FD_SET(stcp, &ibits);

        /* Wait for socket activity */
        if( stcpactive == INVALID_SOCKET ) {
            /* Wait without timeout */
            tmpfd = fdSelect( sizeof(int), &ibits, &obits, &xbits, 0 );
        } else {
            /* Wait without timeout */
            FD_SET(stcpactive, &ibits);
            tmpfd = fdSelect( sizeof(int), &ibits, &obits, &xbits, 0 );
            if( tmpfd <= 0 ) {
                fdClose( stcpactive );
                stcpactive = INVALID_SOCKET;
            }
        }

        if( tmpfd < 0 )
            goto leave;

        /* Check for a new TCP connection */
        if( FD_ISSET(stcp, &ibits) ) {
            /* We have a new connection. Assign it to stcpbusy at */
            /* first... */
            size = sizeof( sin );
            stcpbusy = accept( stcp, (struct sockaddr *)&sin, &size );

            /* If the active socket is free, use it, otherwise close */
            /* the descriptor.
             * Thus, we allow only  a single connection at a time. */
            if( stcpactive == INVALID_SOCKET )
                stcpactive = stcpbusy;
            else
                fdClose( stcpbusy );
        }

        /* Check for new data on active TCP connection */
        if( stcpactive != INVALID_SOCKET && FD_ISSET(stcpactive, &ibits) ) {

            /* There is data available on the active connection */
            cnt = recv( stcpactive, pBuf, 1500, MSG_DONTWAIT/*per call non blocking*/);

            if( cnt > 0 ) {
                /* Close connection after receiving a complete message.
                 * Note: A complete message is detected by the TCP fsm
                 * by receiving the PUSH flag from a peer.*/
                if( send( stcpactive, pBuf, cnt, 0 ) < 0 ) {
                    fdClose( stcpactive );
                    stcpactive = INVALID_SOCKET;
                }
            } else {
                /* If the connection got an error or it got disconnected, close */
                fdClose( stcpactive );
                stcpactive = INVALID_SOCKET;
                cnt = 0;

            }
        }
    }/*end while(1)*/


    leave:
       /* We only get here in case of an error - close the sockets */
       if( stcp != INVALID_SOCKET )
           fdClose( stcp );

       System_printf("Fatal Error - terminating task\n");
       System_flush();

       /* Terminate task */
       Task_exit();
}


/*
 *  Setup task function
 */
int setup_TCPEcho_Task(uint16_t lport)
{
    Task_Params taskTCPEchoParams;
    Task_Handle taskTCPEcho;
    Error_Block eb;


    Error_init(&eb);
    Task_Params_init(&taskTCPEchoParams);
    taskTCPEchoParams.stackSize = (int)NdkTaskNormStack; /* according to NDK requirements */
    taskTCPEchoParams.priority = (int)NdkTaskNormPrio; /*NDK tasks require to have a lower priority than the NDK scheduler task; see application.cfg & NDK manual */
    taskTCPEchoParams.arg0 = (UArg)lport;
    taskTCPEcho = Task_create((Task_FuncPtr)TCPEchoFxn, &taskTCPEchoParams, &eb);
    if (taskTCPEcho == NULL) {
        System_abort("TaskTCPEcho create failed");
    }

    return (0);
}
