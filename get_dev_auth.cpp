#include "get_dev_auth.h"
#include <string.h>

typedef struct _req_auth_context_{
	uv_loop_t*loop;
	uv_tcp_t * socket ;
	char *pSn ;
	char *pAccount ;
	char *pPassword;
	void *pUserData;
	get_dev_auth_cb cb;
}req_auth_context;


req_auth_context * alloc_auth_context(uv_loop_t*loop , char *pSn , char *pAccount  , char *pPassword, void *pUserData , get_dev_auth_cb cb)
{
	req_auth_context *pReq = new req_auth_context;
	pReq->loop = loop;

	pReq->pSn  = new char[strlen(pSn)];
	strcpy(pReq->pSn,pSn);

	pReq->pAccount  = new char[strlen(pAccount)];
	strcpy(pReq->pAccount,pAccount);

	pReq->pUserData = pUserData;

	pReq->pPassword = new char[strlen(pPassword)];
	strcpy(pReq->pPassword,pPassword);

	pReq->cb = cb;
	pReq->socket = new uv_tcp_t;

	return pReq;
}

void free_auth_context(req_auth_context *pReq)
{
	if(NULL == pReq)
	{
		return ;
	}
	if(NULL != pReq->pAccount)
	{
		delete [] pReq->pAccount;
	}
	if(NULL != pReq->pPassword)
	{
		delete [] pReq->pPassword;
	}
	if(NULL != pReq->pSn)
	{
		delete [] pReq->pSn;
	}
	if(NULL != pReq->socket)
	{
		delete pReq->socket;
	}

	delete pReq;
}

void get_dev_auth(uv_loop_t*loop  , char *pDmsAddr , unsigned short sDmsPort , char *pSn , char *pAccount  , char *pPassword , void *pUserData, get_dev_auth_cb cb)
{
	req_auth_context *pReq = alloc_auth_context(loop,pSn,pAccount,pPassword,pUserData,cb);
}
