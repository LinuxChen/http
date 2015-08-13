#ifndef _CREATE_CON_H_
#define _CREATE_CON_H_

#include <uv.h>
#include "reju_list.h"

typedef struct con_context  con_context;

typedef void (*tcp_con_cb)(int error,void *pUserData,con_context  *context );

typedef void(*tcp_write_cb)(int error,void *pUserData,con_context *context);

typedef void(*tcp_read_cb)(int error,void *pUserData,char *pData,int dataLen,con_context *context);

typedef void(*tcp_close_cb)(int error,void *pUserData,con_context *context);

void tcp_con(uv_loop_t*loop,char *pSerAddr,unsigned short port,void *pUserData,tcp_con_cb conCB);

void tcp_write(con_context *context,char *pData,int dataLen,void *pUserData,tcp_write_cb cb);

void tcp_read(con_context *context,void *pUserData,tcp_read_cb cb);

void tcp_setCloseCB(con_context *context,void *pUserData,tcp_close_cb cb);

void tcp_close(con_context *context);

void tcp_run(uv_loop_t*loop);

#endif
