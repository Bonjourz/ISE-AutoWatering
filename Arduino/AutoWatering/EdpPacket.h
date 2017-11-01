/**********************************************************************
 *  Copyright(c) 2014-2015 China Mobile M2M Company Limited.
 *  All rights reserved.
 *
 *  文件名称: EdpPacket.h
 *  简要描述: Arduino edp packets encapsulation library
 *
 *  创建日期: 2015.5.27
 *   
 *  说明:  Arduino edp packets encapsulation library
 *  版本:  V1.0
 *
 *  修改日期:
 *  作者:  
 *  说明:  
 *
 * 
 **********************************************************************/

#ifndef EDP_PACKET_H_
#define EDP_PACKET_H_

using namespace std;

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
   
#define RET_ERR 0
#define RET_OK (!(RET_ERR))


/*----------------------------消息类型--------------------------------------*/
/* 连接请求 */
#define CONNREQ             0x10
/* 连接响应 */
#define CONNRESP            0x20
/* 转发（透传）数据 */
#define PUSHDATA            0x30
/* 存储（透传）数据 */
#define SAVEDATA            0x80
/* 存储确认 */
#define SAVEACK             0x90
/* 命令请求 */
#define CMDREQ              0xA0
/* 命令响应 */
#define CMDRESP             0xB0
/* 心跳请求 */
#define PINGREQ             0xC0
/* 心跳响应 */
#define PINGRESP            0xD0
/* 加密请求 */
#define ENCRYPTREQ          0xE0
/* 加密响应 */
#define ENCRYPTRESP         0xF0

/*--------------------------------------------------------------------*/
#define ERR_UNPACK_CONNRESP_REMAIN              -1000
#define ERR_UNPACK_CONNRESP_FLAG                -1001
#define ERR_UNPACK_CONNRESP_RTN                 -1002
#define ERR_UNPACK_PUSHD_REMAIN                 -1010
#define ERR_UNPACK_PUSHD_DEVID                  -1011
#define ERR_UNPACK_PUSHD_DATA                   -1012
#define ERR_UNPACK_SAVED_REMAIN                 -1020
#define ERR_UNPACK_SAVED_TANSFLAG               -1021
#define ERR_UNPACK_SAVED_DEVID                  -1022
#define ERR_UNPACK_SAVED_DATAFLAG               -1023
#define ERR_UNPACK_SAVED_JSON                   -1024
#define ERR_UNPACK_SAVED_PARSEJSON              -1025
#define ERR_UNPACK_SAVED_BIN_DESC               -1026
#define ERR_UNPACK_SAVED_PARSEDESC              -1027
#define ERR_UNPACK_SAVED_BINLEN                 -1028
#define ERR_UNPACK_SAVED_BINDATA                -1029
#define ERR_UNPACK_PING_REMAIN                  -1030
#define ERR_UNPACK_CMDREQ                       -1031
#define ERR_UNPACK_ENCRYPT_RESP                 -1032
#define ERR_UNPACK_SAVEDATA_ACK                 -1033

#define EDP_BUFFER_SIZE		200
#define PROTOCOL_NAME     "EDP"
#define PROTOCOL_VERSION  1

typedef unsigned char    uint8;
typedef char             int8;
typedef unsigned short   uint16;
typedef short            int16;
typedef unsigned int     uint32;
typedef int              int32;

typedef enum {
    kTypeFullJson = 0x01,
    kTypeBin = 0x02,
    kTypeSimpleJsonWithoutTime = 0x03,
    kTypeSimpleJsonWithTime = 0x04,
    kTypeString = 0x05
}SaveDataType;

class EdpPacketClass
{
    public:
    EdpPacketClass();
    ~EdpPacketClass();
        
		uint8* GetData(void);
		uint32 GetWritepos(void);
		uint32 GetReadpos(void);
		void  ClearParameter(void);
		int32 CheckCapacity(uint32 len);
		int32 ReadByte(uint8* val);
		int32 ReadBytes(uint8** val, uint32 count);
    int32 ReadUint16(uint16* val);
		int32 ReadUint32(uint32* val);
		int32 ReadStr(char** val);
		int32 ReadRemainlen(uint32* len_val);
		int32 WriteByte(uint8 byte);
		int32 WriteBytes(const void* bytes, uint32 count);
		int32 WriteUint16(uint16 val);
		int32 WriteUint32(uint32 val);
		int32 WriteStr(const char *str);
		int32 WriteRemainlen(uint32 len_val);
		uint8* PacketConnect1(const char* devid, const char* auth_key);
		uint8* PacketConnect2(const char* userid, const char* auth_info);
		uint8* PacketPushdata(const char* dst_devid, const char* data, uint32 data_len);
		uint8* PacketSavedataBinStr(const char* dst_devid, const char* desc_str, const uint8* bin_data, uint32 bin_len);
		uint8* PacketSavedataSimpleString(const char* dst_devid, const char* input);
		
		uint8* PacketCmdResp(const char* cmdid, uint16 cmdid_len,const char* resp, uint32 resp_len);
		uint8* PacketPing(void);
		int32  IsPkgComplete(uint8* bufdata,uint32 buflen);
		uint8  EdpPacketType(uint8* bufdata);
		int32  UnpackConnectResp(uint8* bufdata);
		int32  UnpackCmdReq(uint8* bufdata, char** cmdid, uint16* cmdid_len, char** req, uint32* req_len);
		int32  UnpackPingResp(uint8* bufdata);
    private:
    uint8*  data;          
		uint32  write_pos;     
		uint32  read_pos;      
		uint32  capacity; 
        
};

#endif  //EDP_PACKET_H_
