#include "EdpPacket.h"


/* 
 * 函数名:  EdpPacketClass
 * 功能:    构造函数
 * 说明:    
 *   
 * 相关函数:
 * 参数:    无
 *   
 * 返回值:  无       
 */
EdpPacketClass::EdpPacketClass()
{
	this->data=NULL;
	this->write_pos=0;
	this->read_pos=0;
	this->capacity=EDP_BUFFER_SIZE;
	
	data=(uint8*)malloc(sizeof(uint8)*EDP_BUFFER_SIZE);
	assert(data!=NULL);
}

/* 
 * 函数名:  ~EdpPacketClass
 * 功能:    析构函数
 * 说明:    
 *   
 * 相关函数:
 * 参数:    无
 *   
 * 返回值:  无       
 */
EdpPacketClass::~EdpPacketClass()
{
	free(this->data);
	this->data=NULL;
}


/* 
 * 函数名:  GetData
 * 功能:    获取数据成员，EDP包的起始地址
 * 说明:    
 *          
 * 相关函数:
 * 参数:    无
    
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址
 */
uint8* EdpPacketClass::GetData(void)
{
	return this->data;
}

/* 
 * 函数名:  GetWritepos
 * 功能:    获取数据成员，EDP包的写位置
 * 说明:    
 *          
 * 相关函数:
 * 参数:    无
    
 * 返回值:  类型 (uint32) 
 *          EDP包的写入位置
 */
uint32 EdpPacketClass::GetWritepos(void)
{
	return this->write_pos;
}


/* 
 * 函数名:  GetReadpos
 * 功能:    获取数据成员，EDP包的读位置
 * 说明:    
 *          
 * 相关函数:
 * 参数:    无
    
 * 返回值:  类型 (uint32) 
 *          EDP包的读位置
 */
uint32 EdpPacketClass::GetReadpos(void)
{
	return this->read_pos;
}

/* 
 * 函数名:  ClearParameter
 * 功能:    重置数据成员，没一个打包函数先调用该函数，然后重新打包数据
 * 说明:    
 *          
 * 相关函数:
 * 参数:    无
    
 * 返回值:  void         
 */
void EdpPacketClass::ClearParameter(void)
{
	this->read_pos=0;
	this->write_pos=0;
	memset(data,0,EDP_BUFFER_SIZE);
}

/* 
 * 函数名:  CheckCapacity
 * 功能:    检查buffer是否能够写入长度为len的字节流，如果不够，自动扩展Buffer的容量（不影响buffer数据）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    无
    
 * 返回值:  void         
 */
int32 EdpPacketClass::CheckCapacity(uint32 len)
{
	uint32 cap_len = capacity;
    int32 flag = 0;
    uint8* pdata = NULL;

    while (cap_len - write_pos < len) /* remain len < len */
    {
        cap_len = cap_len << 1;
        if (++flag > 32)
            break;  /* overflow */
    }
    if (flag > 32)
        return -1;
    if (cap_len > capacity)
    {
        pdata = (uint8*)malloc(sizeof(uint8) * cap_len);
        memcpy(pdata, data, write_pos);
        free(data);
        data = pdata;
        capacity = cap_len;
    }
    return 0;
}

/* 
 * 函数名:  ReadByte
 * 功能:    按EDP协议，从BUFFER中读取一个字节的数据
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::ReadByte(uint8* val)
{
    if (read_pos+1 > write_pos) 
        return -1;
    *val =data[read_pos];
    read_pos += 1;
    return 0;
}


/* 
 * 函数名:  ReadBytes
 * 功能:    按EDP协议，从BUFFER中读取count个字节的数据
 * 说明:    其中VAL是MALLOC函数分配的存储空间，在使用完之后，需要free.
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::ReadBytes(uint8** val, uint32 count)
{
    if (read_pos+count > write_pos) 
        return -1;
    *val = (uint8*)malloc(sizeof(uint8) * count);
    memcpy(*val, data+read_pos, count);
    read_pos += count;
    return 0;
}


/* 
 * 函数名:  ReadUint16
 * 功能:    按EDP协议，从BUFFER中读取UINT16值
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::ReadUint16(uint16* val)
{
    uint8 msb, lsb;
    if (read_pos+2 > write_pos) 
        return -1;
    msb = data[read_pos];
    read_pos++;
    lsb = data[read_pos];
    read_pos++;
    *val = (msb<<8) + lsb;
    return 0;
}

/* 
 * 函数名:  ReadUint32
 * 功能:    按EDP协议，从BUFFER中读取UINT32值
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::ReadUint32(uint32* val)
{
    int32 i = 0;
    uint32 tmpval = 0;
    if (read_pos+4 > write_pos) 
        return -1;
    while (i++ < 4) {
        tmpval = (tmpval << 8) | (data[read_pos]);
        read_pos++;
    }
    *val = tmpval;
    return 0;
}

/* 
 * 函数名:  ReadStr
 * 功能:    按EDP协议，从BUFFER中读取字符串
 * 说明:    val是malloc函数分配的存储空间，在使用完后需要free
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::ReadStr(char** val)
{
    uint16 len = 0;
    int rc = 0;
    /* read str len */
    rc = ReadUint16(&len);
    if (rc) 
        return rc;
    if (read_pos+len > write_pos) 
        return -1;
    /* copy str val */
    *val = (char*)malloc(sizeof(char) * (len + 1));
    memset(*val, 0, len+1);
    strncpy(*val, (const char *)(data + read_pos), len);
    read_pos += len;
    return 0;
}


/* 
 * 函数名:  ReadRemainlen
 * 功能:    按EDP协议，从BUFFER中读取消息剩余长度
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::ReadRemainlen(uint32* len_val)
{
    uint32 multiplier = 1;
    uint32 len_len = 0;
    uint8 onebyte = 0;
    int32 rc;
    *len_val = 0;

    do {
        rc = ReadByte(&onebyte);
        if (rc) 
            return rc;
		if(len_len==0)
		{
			*len_val += (onebyte & 0x7f);
        }
		else if(len_len==1)
		{
			*len_val += (onebyte & 0x7f) <<7;
		}
		else if(len_len==2)
		{
			*len_val += (onebyte & 0x7f) <<14;
		}
		else if(len_len==3)
		{
			*len_val += (onebyte & 0x7f) <<21;
		}
		else
		{;}
        len_len++;
        if (len_len > 4) {
            return -1;/*len of len more than 4;*/
        }
    } while((onebyte & 0x80) != 0);
    return 0;
}

/* 
 * 函数名:  WriteByte
 * 功能:    按EDP协议，将一个字节数据写入EDP包缓冲（数据成员data所指的空间）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::WriteByte(uint8 byte)
{
    assert(read_pos == 0);
    if (CheckCapacity(1))
        return -1;
    data[write_pos] = byte;
    write_pos++;
    return 0;
}


/* 
 * 函数名:  WriteBytes
 * 功能:    按EDP协议，将count个字节数据写入EDP包缓冲（数据成员data所指的空间）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::WriteBytes(const void* bytes, uint32 count) 
{
    assert(read_pos == 0);
    if (CheckCapacity(count))
        return -1;
    memcpy(data + write_pos, bytes, count);
    write_pos += count;
    return 0;
}

/* 
 * 函数名:  WriteUint16
 * 功能:    按EDP协议，将uINT16值写入EDP包缓冲（数据成员data所指的空间）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::WriteUint16(uint16 val)
{
    assert( read_pos == 0);
    return WriteByte(0xff&((val)>>8)) 
        || WriteByte(0xff&(val));
}

/* 
 * 函数名:  WriteUint32
 * 功能:    按EDP协议，将uINT32值写入EDP包缓冲（数据成员data所指的空间）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::WriteUint32(uint32 val)
{
    assert(read_pos == 0);
    return WriteByte((val >> 24) & 0x00FF) 
        || WriteByte((val >> 16) & 0x00FF)
        || WriteByte((val >> 8) & 0x00FF) 
        || WriteByte((val) & 0x00FF);
}

/* 
 * 函数名:  WriteStr
 * 功能:    按EDP协议，将字符串写入EDP包缓冲（数据成员data所指的空间）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::WriteStr(const char *str)
{
    uint16 length = 0;
    assert(read_pos == 0);
    length = strlen(str);
    return WriteUint16(length) 
        || WriteBytes(str, length);
}


/* 
 * 函数名:  WriteRemainlen
 * 功能:    按EDP协议，将消息剩余长度值写入EDP包缓冲（数据成员data所指的空间）
 * 说明:    
 *          
 * 相关函数:
 * 参数:    
    
 * 返回值:         
 */
int32 EdpPacketClass::WriteRemainlen(uint32 len_val)
{
    uint32 remaining_length = len_val;
    int32  remaining_count = 0;
    uint8  byte = 0;

    assert(read_pos == 0);

    do {
        byte = remaining_length % 128;
        remaining_length = remaining_length / 128;
        /* If there are more digits to encode, set the top bit of this digit */
        if (remaining_length > 0) {
            byte = byte | 0x80;
        }
       data[write_pos++] = byte;
        remaining_count++;
    }while(remaining_length > 0 && remaining_count < 5);
    assert(remaining_count != 5);
    return 0;
}
/* 
 * 函数名:  PacketConnect1 
 * 功能:    打包 设备到设备云的EDP协议包，连接设备云的请求包（登录认证方式1）
 * 说明:    
 *          
 * 相关函数:UnpackConnectResp
 * 参数:    devid      设备ID，申请设备时平台返回的ID
 *          auth_key   鉴权信息，在平台申请的可以操作该设备的API-KEY字符串
    
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址         
 */
uint8* EdpPacketClass::PacketConnect1(const char* devid, const char* auth_key) 
{  
	uint32 remainlen;

	ClearParameter();
	/* msg type */
	WriteByte(CONNREQ);
	/* remain len */
	remainlen = (2+3)+1+1+2+(2+strlen(devid))+(2+strlen(auth_key));
	WriteRemainlen(remainlen);
	/* protocol desc */
	WriteStr(PROTOCOL_NAME);
	/* protocol version */
	WriteByte(PROTOCOL_VERSION);
	/* connect flag */
	WriteByte(0x40);
	/* keep time */
	WriteUint16(0x0080);
	/* DEVID */
	WriteStr(devid);
	/* auth key */
	WriteStr(auth_key);
	  
	return this->data;
}

/* 
 * 函数名:  PacketConnect2 
 * 功能:    打包 设备到设备云的EDP协议包，连接设备云的请求包（登录认证方式2）
 * 说明:    
 *          
 * 相关函数:UnpackConnectResp
 * 参数:    userid      项目ID，在平台注册项目时平台返回的项目ID
 *          auth_info   鉴权信息，在平台申请设备时填写设备的AUTH_INFO属性（Json对象字符串）
    
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址         
 */
uint8* EdpPacketClass::PacketConnect2(const char* userid, const char* auth_info) 
{	
	uint32 remainlen;
	
	ClearParameter();	
	/* msg type */
	WriteByte(CONNREQ);
	/* remain len */
	remainlen = (2+3)+1+1+2+2+(2+strlen(userid))+(2+strlen(auth_info));
	WriteRemainlen(remainlen);
	/* protocol desc */
	WriteStr(PROTOCOL_NAME);
	/* protocol version */
	WriteByte(PROTOCOL_VERSION);
	/* connect flag */
	WriteByte(0xC0);
	/* keep time */
	WriteUint16(0x0080);
	/* devid */
	WriteByte(0x00);
	WriteByte(0x00);
	/* USERID */
	WriteStr(userid);
	/* auth info */
	WriteStr(auth_info);
	return this->data;
}


/* 
 * 函数名:  PacketPushdata
 * 功能:    打包 设备到设备云的EDP协议包，设备与设备之间的转发数据
 * 说明:    
 *          
 * 相关函数:UnpackPushdata
 * 参数:    dst_devid   目的设备ID
 *          data        数据
 *          data_len    数据长度
 *          bin_len     二进制数据长度     
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址         
 */
uint8* EdpPacketClass::PacketPushdata(const char* dst_devid, const char* data, uint32 data_len)
{ 
    uint32 remainlen;
	ClearParameter();
    /* msg type */
    WriteByte(PUSHDATA);
    /* remain len */
    remainlen = (2+strlen(dst_devid))+data_len;
    WriteRemainlen(remainlen);
    /* dst devid */
    WriteStr(dst_devid);
    /* data */
    WriteBytes(data, data_len);
    return this->data;
}

/* 
 * 函数名:  PacketSavedataBinStr
 * 功能:    打包 设备到设备云的EDP协议包，存储数据（bin格式数据）
 * 说明:    
 *          
 * 相关函数:UnpackSavedataSimpleString
 * 参数:    dst_devid   目的设备ID
 *          desc_obj    数据描述 字符串格式，”ds_id”:”xxx" xxx为二进制数据流名称，详见 <设备终端接入协议2-EDP.docx>  
 *          bin_data    二进制数据
 *          bin_len     二进制数据长度     
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址         
 */
uint8* EdpPacketClass::PacketSavedataBinStr(const char* dst_devid, 
        const char* desc_str, const uint8* bin_data, uint32 bin_len)
{
    uint32 remainlen = 0;
    uint32 desc_len = 0;

    /* check arguments */
    desc_len = strlen(desc_str);
    if (desc_len > (0x01 << 16) || bin_len > (3 * (0x01 << 20)))           
    { /* desc < 2^16 && bin_len < 3M*/
        return 0;
    }
    ClearParameter();
	/* msg type */
	WriteByte(SAVEDATA);
    if (dst_devid)
    {
        /* remain len */
        remainlen = 1+(2+strlen(dst_devid))+1+(2+desc_len)+(4+bin_len);
        WriteRemainlen(remainlen);
        /* translate address flag */
        WriteByte(0x80);
        /* dst devid */
        WriteStr(dst_devid);
    }
    else
    {
        /* remain len */
        remainlen = 1+1+(2+desc_len)+(4+bin_len);
        WriteRemainlen(remainlen);
        /* translate address flag */
        WriteByte(0x00);
    }
    /* bin flag */
    WriteByte(0x02);
    /* desc */ 
    WriteStr(desc_str);
    /* bin data */
    WriteUint32(bin_len);
    WriteBytes(bin_data, bin_len);
    return this->data;
}

/* 
 * 函数名:  PacketSavedataSimpleString
 * 功能:    打包 设备到设备云的EDP协议包，存储数据（以分号分隔的简单字符串形式）
 * 说明:    
 *          
 * 相关函数:UnpackSavedataSimpleString
 * 参数:    dst_devid      命令ID
            input          以分号分隔的简单字符串形式
	                         详见 <设备终端接入协议2-EDP.docx>      
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址         
 */
uint8* EdpPacketClass::PacketSavedataSimpleString(const char* dst_devid, const char* input)
{
    //EdpPacket* pkg = NULL;
    uint32 remainlen = 0;
    uint32 input_len = 0;

    ClearParameter();
    input_len = strlen(input);
    /* msg type */
    WriteByte(SAVEDATA);
    if (dst_devid)
    {
        /* remain len */
        remainlen = 1+(2+strlen(dst_devid))+1+(2+input_len);
        WriteRemainlen(remainlen);
        /* translate address flag */
        WriteByte(0x80);
        /* dst devid */
        WriteStr(dst_devid);
    }
    else
    {
        /* remain len */
        remainlen = 1+1+(2+input_len);
        WriteRemainlen(remainlen);
        /* translate address flag */
        WriteByte(0x00);
    }
    /* json flag */
    WriteByte(kTypeString);
    /* json */ 
    WriteStr(input);

    return this->data;
}

/* 
 * 函数名:  PacketCmdResp
 * 功能:    向接入机发送命令响应
 * 说明:    
 *          
 * 相关函数:UnpackCmdReq
 * 参数:    cmdid      命令ID
            cmdid_len  命令ID的长度
						resp       响应的消息
            resp_len   响应消息的长度
            
 * 返回值:  类型 (uint8*) 
 *          EDP包的起始地址         
 */
uint8* EdpPacketClass::PacketCmdResp(const char* cmdid, uint16 cmdid_len,
			 const char* resp, uint32 resp_len)
{
    unsigned remainlen = 0;

		ClearParameter();
    /* 6 = 2 + 4 = len(cmdid_len) + len(resp_len) */
    remainlen = cmdid_len + resp_len + (resp_len ? 6 : 2);
    WriteByte(CMDRESP);
    WriteRemainlen(remainlen);
    WriteUint16(cmdid_len);
    WriteBytes(cmdid, cmdid_len);
    if (resp_len)
		{
			WriteUint32(resp_len);
			WriteBytes(resp, resp_len);
    }
    return this->data;
}

/* 
 * 函数名:  PacketPing
 * 功能:    打包 由设备到设备云的EDP协议包，心跳包
 * 说明:    
 *          
 * 相关函数:UnpackPingResp，EdpPacketType
 * 参数:    无
            
 * 返回值:  类型 (uint8*) 
 *          心跳包的起始地址         
 */
uint8* EdpPacketClass::PacketPing(void)
{
    ClearParameter();
    /* msg type */
    WriteByte(PINGREQ);
    /* remain len */
    WriteRemainlen(0);
    return this->data;
}

/* 
 * 函数名:  UnpackConnectResp
 * 功能:    解包 由设备云到设备的EDP协议包，连接响应
 * 说明:    
 *          
 * 相关函数:PacketConnect1, PacketConnect2，EdpPacketType
 * 参数:    bufdata   	EDP包数据
            
 * 返回值:  类型 (int32) 
 *          =0          解析成功
 *          >0          连接失败，具体失败原因见<设备终端接入协议2-EDP.docx>
 *					<0          解析失败，具体失败原因见本H文件的错误码
 */
int32 EdpPacketClass::IsPkgComplete(uint8* bufdata,uint32 buflen)
{
    uint32 data_len = 0;
    uint32 len_val = 0;
    uint32 len_len = 0;
    uint8* pdigit = NULL;
    uint32 pkg_total_len = 0;
    
    data_len = buflen;
    if (data_len <= 1) 
	  {
		  return 0;   
    }
    /* recevie remaining len */
    pdigit = bufdata;

    do 
	  {
			if (len_len > 4) 
			{
				return -1;  /* protocol error; */
			}
			if (len_len > data_len - 1) 
			{
				return 0;   /* continue receive */
			}
			len_len++;
			pdigit++;
			if(1==len_len)
			{
				len_val += ((*pdigit) & 0x7f);
			}
			else if(2==len_len)
			{
				len_val += ((*pdigit) & 0x7f)<<7;
			}
			else if(3==len_len)
			{
				len_val += ((*pdigit) & 0x7f)<<14;
			}
			else if(4==len_len)
			{
				len_val += ((*pdigit) & 0x7f)<<21;
			}
			else
			{;}
    }while(((*pdigit) & 0x80) != 0);

    pkg_total_len = len_len + 1 + len_val;
    /* receive payload */
    if (pkg_total_len <= (uint32)data_len)
	  {
		  return pkg_total_len;   /* all data for this pkg is read */
    }
	  else
	  {
		  return 0; 
    }
}

uint8 EdpPacketClass::EdpPacketType(uint8* bufdata)
{
    uint8 mtype = 0x00;
	mtype=*bufdata; 
    return mtype;
}

/* 
 * 函数名:  UnpackConnectResp
 * 功能:    解包 由设备云到设备的EDP协议包，连接响应
 * 说明:    
 *          
 * 相关函数:PacketConnect1, PacketConnect2，EdpPacketType
 * 参数:    bufdata   	EDP包数据
            
 * 返回值:  类型 (int32) 
 *          =0          解析成功
 *          >0          连接失败，具体失败原因见<设备终端接入协议2-EDP.docx>
 *					<0          解析失败，具体失败原因见本H文件的错误码
 */
int32 EdpPacketClass::UnpackConnectResp(uint8* bufdata)
{
    uint8  flag, rtn;
	uint32 len_len = 0;
    uint8* pdigit = bufdata+1;
    uint32 len_val = 0;
	do {
		if(len_len==0)
		{
			len_val += (*pdigit & 0x7f);
        }
		else if(len_len==1)
		{
			len_val += (*pdigit & 0x7f) <<7;
		}
		else if(len_len==2)
		{
			len_val += (*pdigit & 0x7f) <<14;
		}
		else if(len_len==3)
		{
			len_val += (*pdigit & 0x7f) <<21;
		}
		else
		{;}
		pdigit++;
        len_len++;
        if (len_len > 4) {
            return ERR_UNPACK_CONNRESP_REMAIN; /*len of len more than 4;*/
        }
    } while((*pdigit & 0x80) != 0);

	if(len_val>0)
	{
		flag = *pdigit++;
		len_val--;
	}
	else
	{
		return ERR_UNPACK_CONNRESP_FLAG;
	}

	if(len_val>0)
	{
		rtn = *pdigit;
		len_val--;
	}
	else
	{
		return ERR_UNPACK_CONNRESP_FLAG;
	}
 
    return (int32)rtn;
}

/* 
 * 函数名:  UnpackCmdReq
 * 功能:    解包 由设备云到设备的EDP协议包，命令请求消息
 * 说明:    解析命令请求包获取的cmdid及req需要在使用后释放
 *          
 * 相关函数:PacketCmdResp，EdpPacketType
 * 参数:    bufdata   	EDP包数据
            cmdid       获取命令ID
            cmdid_len   命令ID的长度
            req         用户命令的起始位置
            req_len     用户命令的长度

 * 返回值:  类型 (int32) 
 *          =0          解析成功
 *          <0          解析失败，具体失败原因见本H文件的错误码
 */
int32 EdpPacketClass::UnpackCmdReq(uint8* bufdata, char* * cmdid, uint16* cmdid_len, 
		   char* * req, uint32* req_len)
{
	uint8  msb, lsb;
	uint8  len1,len2,len3,len4;
	uint32 len_len = 0;
    uint8* pdigit = bufdata+1;
    uint32 len_val = 0;

	do {
		if(len_len==0)
		{
			len_val += (*pdigit & 0x7f);
    }
		else if(len_len==1)
		{
			len_val += (*pdigit & 0x7f) <<7;
		}
		else if(len_len==2)
		{
			len_val += (*pdigit & 0x7f) <<14;
		}
		else if(len_len==3)
		{
			len_val += (*pdigit & 0x7f) <<21;
		}
		else
		{;}
		pdigit++;
    len_len++;
        if (len_len > 4) {
            return ERR_UNPACK_CMDREQ; /*len of len more than 4;*/
        }
    } while((*pdigit & 0x80) != 0);

	if(len_val>=2)
	{
		msb=*pdigit++;
		lsb=*pdigit++;
		*cmdid_len=((msb<<8)+lsb);
		len_val-=2;
	}
	else
	{
		return ERR_UNPACK_CMDREQ;
	}
    
	if(len_val>=*cmdid_len)
	{
		*cmdid = (char*)malloc(sizeof(char) * (*cmdid_len+1));
		memset(*cmdid,0, (*cmdid_len+1));
		memcpy(*cmdid, pdigit, (*cmdid_len));
	
		pdigit+=(*cmdid_len);
		len_val-=(*cmdid_len);
	}
	else
	{
		return ERR_UNPACK_CMDREQ;
	}
    
	if(len_val>=4)
	{
		len1=*pdigit++;	
		len2=*pdigit++;		
		len3=*pdigit++;		
		len4=*pdigit++;
		*req_len=(len1<<24)+(len2<<16)+(len3<<8)+(len4);
		len_val-=4;
	}
	else
	{
		return ERR_UNPACK_CMDREQ;
	}
	if(len_val>=*req_len)
	{
		*req = (char*)malloc(sizeof(char) * (*req_len+1));
		memset(*req, 0, (*req_len+1));
		memcpy(*req, pdigit, (*req_len));
		
		pdigit+=(*req_len);
		len_val-=(*req_len);
	}
	else
	{
		return ERR_UNPACK_CMDREQ;
	}
    return 0;
}

/* 
 * 函数名:  UnpackPingResp
 * 功能:    解包 由设备云到设备的EDP协议包，心跳响应
 * 说明:    
 *          
 * 相关函数:PacketPing, GetEdpPacket, EdpPacketType
 * 参数:    bufdata   	EDP包，接收到的完整的心跳响应包
 * 返回值:  类型 (int32) 
 *          =0          心跳成功
 *          >0          心跳失败，具体失败原因见<设备终端接入协议EDP.docx>
 *          <0          解析失败，具体失败原因见本H文件的错误码
 */
int32 EdpPacketClass::UnpackPingResp(uint8* bufdata)
{
	uint32 len_val = 0;
	uint32 len_len = 0;
  uint8* pdigit = bufdata+1;
	do {
	if(len_len==0)
	{
		len_val += (*pdigit & 0x7f);
  }
	else if(len_len==1)
	{
		len_val += (*pdigit & 0x7f) <<7;
	}
	else if(len_len==2)
	{
		len_val += (*pdigit & 0x7f) <<14;
	}
	else if(len_len==3)
	{
		len_val += (*pdigit & 0x7f) <<21;
	}
	else
	{;}
	pdigit++;
  len_len++;
  if (len_len > 4) 
	{
     return ERR_UNPACK_PING_REMAIN;  /*len of len more than 4;*/
  }
  } while((*pdigit & 0x80) != 0);
	return 0;
}
