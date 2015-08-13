#ifndef _HTTP_REQ_H_
#define _HTTP_REQ_H_

#include <uv.h>

typedef struct http_req_context http_req_context;
typedef struct http_res_context http_res_context;

typedef void (*http_res_cb)(int error,void *pUserData,http_req_context *pReq,http_res_context *pRes);

typedef void(*http_con_cb)(int error,void *pUserData,http_req_context *pReq);

void set_method(http_req_context *pReq,char *pMethod);

void set_path(http_req_context *pReq,char *pPath);

void add_head(http_req_context *pReq,char *pItem,char *pValue);

void keep_live(http_req_context *pReq,bool bKeeplive);

void add_body(http_req_context *pReq,char *pBody,int len);

void http_req(uv_loop_t*loop,char *pSerAddr,unsigned short port,void *pUserData,http_con_cb conCB,http_res_cb resCB);


#endif
