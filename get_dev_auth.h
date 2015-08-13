#ifndef _GET_DEV_AUTH_H_
#define _GET_DEV_AUTH_H_

#include <uv.h>

typedef void (*get_dev_auth_cb)(int error,void *pUserData,char *pAuthCode,char *pUpnpAddr,unsigned short pnpPort);

void get_dev_auth(uv_loop_t*loop  , char *pdmsAddr , unsigned short sDmsPort , char *pSn , char *pAccount ,char *pPassword, void *pUserData , get_dev_auth_cb cb);

#endif
