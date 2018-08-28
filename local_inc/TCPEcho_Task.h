/*! \file TCPEcho_Task.h
    \brief TCPEcho task
    \author Matthias Wenzl

   TCPEcho Task example.

*/


#ifndef LOCAL_INC_TCPECHO_TASK_H_
#define LOCAL_INC_TCPECHO_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>



/*! \fn TCPEchoFxn
 *  \brief Execute TCPEcho Task
 *
 *
 *   \param arg0 server listen port
 *   \param arg1 void
 *
 */
void TCPEchoFxn(UArg arg0, UArg arg1);


/*! \fn setup_TCPEcho_Task
 *  \brief Setup TCPEcho task
 *
 *  Setup TCPEcho task
 *  TODO: configure task priority according to network task settings
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_TCPEcho_Task(uint16_t lport);



#endif /* LOCAL_INC_TCPECHO_TASK_H_ */
