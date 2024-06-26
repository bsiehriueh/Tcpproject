#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"
#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed : name error or pwd error or relogin"
#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE  "online"
#define SEARCH_USR_OFFLINE "offline"
#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOEXIST "usr not exist"
#define DEL_FRIEND_OK "delete friend ok"

#define DIR_NO_EXIST "cur dir no exist"
#define FILE_NAME_EXIST "file name exist"
#define CREAT_DIR_OK "create dir ok"

#define DEL_DIR_OK "delete dir ok"
#define DEL_DIR_FAILURED "delete dir failured: is reguler file"

#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILURED "rename file failured"

#define ENTER_DIR_FAILURED "enter dir failured: is regular file"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REQUEST,    //注册请求
    ENUM_MSG_TYPE_RESPONSE,
    ENUM_MSG_TYPE_LOGIN_REQUEST,    //登录请求
    ENUM_MSG_TYPE_LOGIN_RESPONSE,
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   //在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE,  //在线用户回复

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,   //搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPONSE,  //搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,     //添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE,

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,    //刷新好友
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE,

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,     //删除好友
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE,

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,    //私聊
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPONSE,

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,    //群聊
    ENUM_MSG_TYPE_GROUP_CHAT_RESPONSE,

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,   //创建文件夹
    ENUM_MSG_TYPE_CREATE_DIR_RESPONSE,

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,    // 刷新文件
    ENUM_MSG_TYPE_FLUSH_FILE_RESPONSE,

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,       //删除文件夹
    ENUM_MSG_TYPE_DEL_DIR_RESPONSE,

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,   //重命名文件
    ENUM_MSG_TYPE_RENAME_FILE_RESPONSE,

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,    //进入文件夹
    ENUM_MSG_TYPE_ENTER_DIR_RESPONSE,

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,   //上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE,

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,   //删除文件
    ENUM_MSG_TYPE_DEL_FILE_RESPONSE,

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,   //下载文件
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPONSE,

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,
    ENUM_MSG_TYPE_SHARE_FILE_RESPONSE,

    ENUM_MSG_TYPE_SHARE_FILE_NOTE,
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPONSE,

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,
    ENUM_MSG_TYPE_MOVE_FILE_RESPONSE,

//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPONSE,
    ENUM_MSG_TYPE_MAX = 0x00ffffff
};

struct FileInfo
{
    char caName[32];
    int iFileType;

};

struct PDU
{
    uint uiPDULen;     //总的协议数据单元大小
    uint uiMsgType;    //消息类型
    char caData[64];
    uint uiMsgLen;    //实际消息长度
    int caMsg[];   //实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
