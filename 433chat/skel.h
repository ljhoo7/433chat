////////////////////////////////////////
// 다음은 서버와 클라이언트 간 데이터를 주고 받을 때 사용하는 패킷을 정의한 헤더입니다.
#ifndef _REVSTRUCT_H
#define _REVSTRUCT_H
#pragma pack( 1 )
typedef struct _REV_STRUCT{
 int size;
 char p[256];
} REV_STRUCT;
#endif
///////////////////////////////////////