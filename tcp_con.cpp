#include "tcp_con.h"
#include <assert.h>
#include <iostream>

typedef struct _tcp_send_context_{
	void *pUserData;
	tcp_write_cb cb;
	char *pData;
	int dataLen;
}tcp_send_context;

typedef struct _tcp_send_req_
{
	bool bSending;
	uv_write_t *sreq;
	uv_buf_t *sBuf;
	Reju_List_DList *reqList;
}tcp_send_req;

typedef struct _tcp_recv_req_
{
	char *buf;
	int bufLen;
	int dataLen;
	tcp_read_cb rCB;
	void *pUserData;
	bool bRecving;
}tcp_recv_req;

typedef struct _tcp_close_req_
{
	tcp_close_cb clCB;
	void *pUserData;
}tcp_close_req;

typedef struct _tcp_con_req_
{
	uv_connect_t * connect;
	tcp_con_cb conCB;
	void *pUserData ;
}tcp_con_req;

typedef struct con_context{
	uv_loop_t*loop;
	uv_tcp_t * socket;
	bool bError;
	tcp_send_req senReq;
	tcp_close_req closeReq;
	tcp_recv_req recvReq;
	tcp_con_req conReq;
}con_context;

con_context * alloc_con_context(uv_loop_t*loop  ,  void *pUserData  , tcp_con_cb  conCB);
void free_con_context(con_context *pReq);
void internal_tcp_write(con_context *context);
void internal_write_cb(uv_write_t* req, int status);
void internal_alloc_cb(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf);
void internal_read_cb(uv_stream_t* stream,ssize_t nread,const uv_buf_t* buf);
void internal_close_cb(uv_handle_t* handle);


con_context * alloc_con_context(uv_loop_t*loop  ,  void *pUserData  , tcp_con_cb conCB)
{
	con_context *pReq = new con_context;

	pReq->loop = loop;
	pReq->bError = false;
	pReq->socket = new uv_tcp_t;

	pReq->conReq.conCB = conCB;
	pReq->conReq.pUserData = pUserData;
	pReq->conReq.connect = new uv_connect_t;

	pReq->closeReq.clCB = NULL;
	pReq->closeReq.pUserData = NULL;

	pReq->recvReq.bRecving = false;
	pReq->recvReq.buf = new char[8*1024];
	pReq->recvReq.bufLen = 8*1024;
	pReq->recvReq.dataLen = 0;
	pReq->recvReq.pUserData = NULL;
	pReq->recvReq.rCB = NULL;

	pReq->senReq.bSending = false;
	pReq->senReq.reqList = Reju_List_InitList();
	pReq->senReq.sBuf = NULL;
	pReq->senReq.sreq = new uv_write_t;

	return pReq;
}

void free_con_context(con_context *pReq)
{
	if(NULL == pReq)
	{
		return ;
	}

	if(NULL != pReq->conReq.connect)
	{
		delete pReq->conReq.connect;
	}

	if(NULL != pReq->recvReq.buf)
	{
		delete [] pReq->recvReq.buf;
	}

	if(NULL != pReq->senReq.reqList)
	{
		Reju_List_PNode pNode = Reju_List_Remove(pReq->senReq.reqList);
		while(NULL != pNode)
		{
			delete reinterpret_cast<tcp_send_context *>(pNode->data);
			Reju_List_FreeNode(pNode);
			pNode = Reju_List_Remove(pReq->senReq.reqList);
		}
		Reju_List_DestroyList(pReq->senReq.reqList);
	}

	if(NULL != pReq->socket)
	{
		delete pReq->socket;
	}

	delete pReq;
}

void internal_connect_cb(uv_connect_t* req, int status)
{
	con_context *pReq = reinterpret_cast<con_context *>(req->data);
	if(status)
	{
		pReq->conReq.conCB(-1,pReq->conReq.pUserData,NULL);
		free_con_context(pReq);
	}
	else
	{
		pReq->conReq.conCB(0,pReq->conReq.pUserData,pReq);
	}
}

void internal_write_cb(uv_write_t* req, int status)
{
	con_context *context = reinterpret_cast<con_context *>(req->data);

	if(status)
	{
		if(!context->bError)
		{
			context->bError = true;
			uv_close((uv_handle_t *)context->socket,internal_close_cb);
		}
	}
	else
	{
		Reju_List_PNode pNode = Reju_List_Remove(context->senReq.reqList);
		tcp_send_context *pReq = reinterpret_cast<tcp_send_context *>(pNode->data);
		assert(context->senReq.bSending);
		assert(context->senReq.sBuf->base == pReq->pData);
		context->senReq.bSending = false;
		pReq->cb(0,pReq->pUserData,context);
		delete pReq;

		Reju_List_FreeNode(pNode);
		internal_tcp_write(context);
	}
}

void internal_tcp_write(con_context *context)
{
	if(context->bError || context->senReq.bSending || Reju_List_GetSize(context->senReq.reqList) <= 0)
	{
		return;
	}
	context->senReq.bSending = true;
	tcp_send_context *pReq = reinterpret_cast<tcp_send_context *>(Reju_List_GetTail(context->senReq.reqList));
	context->senReq.sBuf->base = pReq->pData;
	context->senReq.sBuf->len = pReq->dataLen;
	context->senReq.sreq->data = context;

	uv_write(context->senReq.sreq,(uv_stream_t *)context->socket,context->senReq.sBuf,1,internal_write_cb);
}

void internal_alloc_cb(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf)
{
	con_context *context = reinterpret_cast<con_context *>(handle->data);
	assert(context->recvReq.bRecving);
	buf->base = context->recvReq.buf + context->recvReq.dataLen;
	buf->len = context->recvReq.bufLen- context->recvReq.dataLen;
}

void internal_read_cb(uv_stream_t* stream,ssize_t nread,const uv_buf_t* buf)
{
	con_context *context = reinterpret_cast<con_context *>(stream->data);
	assert(context->recvReq.bRecving);

	if(nread <= 0)
	{
		std::cout<<uv_strerror(nread)<<std::endl;

		if(!context->bError)
		{
			context->bError = true;
			uv_close((uv_handle_t *)context->socket,internal_close_cb);
		}
	}
	else
	{
		context->recvReq.dataLen += nread;
		if(NULL == context->recvReq.rCB)
		{
			uv_read_stop((uv_stream_t *)context->socket);
			context->recvReq.bRecving = false;
		}
		else
		{
			context->recvReq.rCB(0,context->recvReq.pUserData,context->recvReq.buf,context->recvReq.dataLen,context);
			context->recvReq.dataLen = 0;
		}
	}
}

void internal_close_cb(uv_handle_t* handle)
{
	con_context *context = reinterpret_cast<con_context *>(handle->data);
	context->closeReq.clCB(0,NULL,context);
	free_con_context(context);
}

void tcp_con(uv_loop_t*loop,char *pSerAddr,unsigned short port,void *pUserData,tcp_con_cb conCB)
{
	struct sockaddr_in dest;
	con_context *pReq = alloc_con_context(loop,pUserData,conCB);
	uv_tcp_init(pReq->loop, pReq->socket);
	uv_ip4_addr(pSerAddr,port, &dest);

	pReq->conReq.conCB = conCB;
	pReq->conReq.connect->data = pReq;
	pReq->conReq.pUserData = pUserData;

	uv_tcp_connect(pReq->conReq.connect, pReq->socket, (sockaddr*)&dest, internal_connect_cb);
}

void tcp_write(con_context *context,char *pData,int dataLen,void *pUserData,tcp_write_cb cb)
{
	if(context->bError)
	{
		return;
	}

	tcp_send_context *pReq = new tcp_send_context;
	pReq->cb = cb;
	pReq->dataLen = dataLen;
	pReq->pData = pData;
	pReq->pUserData = pUserData;

	Reju_List_InsFirst(context->senReq.reqList,Reju_List_MakeNode(pReq));

	internal_tcp_write(context);
}

void tcp_read(con_context *context,void *pUserData,tcp_read_cb cb)
{
	if(context->bError)
	{
		return;
	}

	context->recvReq.pUserData = pUserData;
	context->recvReq.rCB = cb;

	if(!context->recvReq.bRecving)
	{
		if(NULL != context->recvReq.rCB)
		{
			context->recvReq.bRecving = true;
			context->socket->data = context;
			uv_read_start((uv_stream_t *) context->socket, internal_alloc_cb, internal_read_cb);
		}
	}
}

void tcp_setCloseCB(con_context *context,void *pUserData,tcp_close_cb cb)
{
	context->closeReq.clCB = cb;
	context->closeReq.pUserData = pUserData;
}

void tcp_close(con_context *context)
{
	uv_close((uv_handle_t*)context->socket,internal_close_cb);
}

void tcp_run(uv_loop_t*loop)
{
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
}
