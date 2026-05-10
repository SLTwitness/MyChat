#ifndef CONST_H
#define CONST_H

enum MSG_IDS {
    MSG_CHAT_LOGIN = 1005,		//用户登录
    MSG_CHAT_LOGIN_RSP = 1006,		//用户登录回包
    ID_SEARCH_USER_REQ = 1007,		//用户搜索请求
    ID_SEARCH_USER_RSP = 1008,		//用户搜索回包
    ID_ADD_FRIEND_REQ = 1009,		//申请添加好友请求
    ID_ADD_FRIEND_RSP = 1010,		//申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,		//通知用户添加好友申请
    ID_AUTH_FRIEDN_REQ = 1012,		//认证好友请求
    ID_AUTH_FRIEND_RSP = 1013,		//认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1014,		//通知用户认证好友请求
    ID_TEXT_CHAT_MSG_REQ = 1015,	//文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1016,	//文本聊天请求回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1017,		//通知用户文本聊天信息
};

#endif // CONST_H
