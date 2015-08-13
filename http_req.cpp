#include "http_req.h"
#include "tcp_con.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

const char *g_http_method[ ] = {"GET","POST"};
const char *g_http_version = "HTTP/1.1";

typedef struct header_pair
{
 char header[16];
 char value[64];
}header_pair;

typedef struct http_head
{
	int count;
	header_pair head[10];
}http_head;

typedef struct http_body
{
	char *pData;
	int dataLen;
}http_body;

typedef struct http_html
{
	char *pBuf;
	int bufLen;
	int dataLen;
}http_html;

typedef struct http_res_context
{
	http_head head;
	http_body body;
	http_html html;
}http_res_context;

typedef struct http_req_context
{
	const char *pMethod;
	bool bKeeplive;
	char path[32];
	http_head head;
	http_body body;
	http_html html;
	http_con_cb conCB;
	http_res_cb resCB;
	void *pUserData;
	http_res_context *res;
}http_req_context;

http_req_context * alloc_http_req_context(void *pUserData,http_con_cb conCB,http_res_cb resCB)
{
	http_req_context *pReq = new http_req_context;
	memset(pReq,0,sizeof(http_req_context));


	pReq->resCB = resCB;
	pReq->conCB = conCB;
	pReq->pUserData = pUserData;

	return pReq;
}

void free_http_req_context(http_req_context *pContext)
{
	if(NULL == pContext)
	{
		return;
	}

	delete pContext;
}

void set_method(http_req_context *pReq,char *pMethod)
{
	for(int i = 0; i < sizeof(g_http_method)/sizeof(char *); i++)
	{
		if(strcmp(pMethod,g_http_method[i]) == 0)
		{
			pReq->pMethod = g_http_method[i];
		}
	}

	if(NULL == pReq->pMethod)
	{
		assert(false);
	}
}

void set_path(http_req_context *pReq,char *pPath)
{
	if(NULL == pPath)
	{
			strcpy(pReq->path,"/");
	}
	else
	{
		if(strlen(pPath) > sizeof(pReq->path))
		{
			assert(false);
			return;
		}
		strcpy(pReq->path,pPath);
	}
}


void add_head(http_req_context *pReq,char *pItem,char *pValue)
{
	if(NULL == pReq || NULL == pItem || NULL == pValue)
	{
		return;
	}

	if(pReq->head.count > sizeof(pReq->head.head)/sizeof(header_pair))
	{
		return;
	}

	if(strlen(pItem) >= sizeof(pReq->head.head[0].header) || strlen(pValue) > sizeof(pReq->head.head[0].value))
	{
		return;
	}

	strcpy(pReq->head.head[pReq->head.count].header,pItem);
	strcpy(pReq->head.head[pReq->head.count].value,pValue);
	pReq->head.count++;
}

void keep_live(http_req_context *pReq,bool bKeeplive)
{
	if(NULL == pReq)
	{
		return;
	}

	pReq->bKeeplive = bKeeplive;
}

void add_body(http_req_context *pReq,char *pBody,int len)
{
	if(NULL == pReq || NULL == pBody || len < 0)
	{
		return;
	}

	if(NULL != pReq->body.pData)
	{
		delete [] pReq->body.pData;
		pReq->body.pData = NULL;
	}

	pReq->body.pData = new char[len];
	memcpy(pReq->body.pData,pBody,len);
	pReq->body.dataLen = len;
}

void http_make(http_req_context *pReq)
{
	int i = 0;
	int totalLen = 0;
	char strLength[16] = {};

	totalLen += strlen(pReq->pMethod);
	totalLen += strlen(pReq->path);
	totalLen += strlen(g_http_version);
	totalLen += 4;

	if(pReq->bKeeplive)
	{
		add_head(pReq,"Connection","keep-alive");
	}

	if(pReq->body.dataLen > 0)
	{
		snprintf(strLength,sizeof(strLength)-1,"%d",pReq->body.dataLen);
		add_head(pReq,"Content-Length",strLength);
	}

	for(;i < pReq->head.count;i++)
	{
		totalLen += strlen(pReq->head.head[i].header);
		totalLen += strlen(pReq->head.head[i].value);
		totalLen += 4;
	}
	totalLen += 2;

	totalLen += pReq->body.dataLen;
	pReq->html.bufLen = totalLen;
	pReq->html.dataLen = totalLen;
	pReq->html.pBuf = new char(totalLen);

	int formatLen = 0;
	formatLen += snprintf(pReq->html.pBuf+formatLen,pReq->html.bufLen-formatLen,"%s %s %s\r\n",pReq->pMethod,pReq->path,g_http_version);
	for(int i=  0; i < pReq->head.count;i++)
	{
		formatLen += snprintf(pReq->html.pBuf+formatLen,pReq->html.bufLen-formatLen,"%s: %s\r\n",pReq->head.head[i].header,pReq->head.head[i].value);
	}
	pReq->html.pBuf[formatLen++] = '\r';
	pReq->html.pBuf[formatLen++] = '\n';
	if(pReq->body.dataLen > 0)
	{
		memcpy(pReq->html.pBuf+formatLen,pReq->body.pData,pReq->body.dataLen);
	}

	pReq->html.dataLen = formatLen + pReq->body.dataLen;
}


void internal_http_con_cb(int error,void *pUserData,con_context * context )
{
	http_req_context *pReq = reinterpret_cast<http_req_context *>(pUserData);
	if(error)
	{
			pReq->conCB(-1,pReq->pUserData,NULL);
			free_http_req_context(pReq);
			return;
	}
	else
	{
		pReq->conCB(0,pReq->pUserData,pReq);
		http_make(pReq);
		tcp_write(context,pReq->html.pBuf,pReq->html.dataLen,pReq,NULL);
	}
}

void http_req(uv_loop_t*loop,char *pSerAddr,unsigned short port,void *pUserData,http_con_cb conCB,http_res_cb resCB)
{
	http_req_context *pReq = alloc_http_req_context(pUserData,conCB,resCB);
	tcp_con(loop,pSerAddr,port,pReq,internal_http_con_cb);
}
