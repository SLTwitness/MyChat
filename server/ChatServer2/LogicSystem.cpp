#include "LogicSystem.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "StatusGrpcClient.h"
#include "CSession.h"
#include "UserMgr.h"
#include "ChatGrpcClient.h"

LogicSystem::~LogicSystem()
{
    b_stop = true;
    _consume.notify_one();
    worker_thread.join();
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con)
{
    if (_get_handlers.find(path) == _get_handlers.end()) {      //确认map里是否有这个key对应的可调用对象
        return false;
    }
    
    _get_handlers[path](con);           //取出可调用对象handler(lambda)并传入参数con
    return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con)
{
    if (_post_handlers.find(path) == _post_handlers.end()) { 
        return false;
    }

    _post_handlers[path](con);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    _get_handlers.insert(make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    _post_handlers.insert(make_pair(url, handler));
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{
    std::unique_lock<std::mutex> unique_lk(_mtx);
    msg_que.push(msg);
    if (msg_que.size() == 1) {
        unique_lk.unlock();
        _consume.notify_one();
    }
}

LogicSystem::LogicSystem() :b_stop(false)
{
    RegisterCallBack();
    worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::DealMsg()
{
    while (1) {
        std::unique_lock<std::mutex> unique_lk(_mtx);
        while (msg_que.empty() && !b_stop) {
            _consume.wait(unique_lk);
        }
        if (b_stop) {
            while (!msg_que.empty()) {
                auto msg_node = msg_que.front();
                std::cout << "recv_msg id is " << msg_node->recv_node->msg_id << std::endl;
                auto call_back_iter = fun_callbacks.find(msg_node->recv_node->msg_id);
                if (call_back_iter == fun_callbacks.end()) {
                    msg_que.pop();
                    continue;
                }
                call_back_iter->second(msg_node->_session, msg_node->recv_node->msg_id,
                    std::string(msg_node->recv_node->_data, msg_node->recv_node->cur_len));
                msg_que.pop();
            }
            break;
        }

        auto msg_node = msg_que.front();
        std::cout << "recv_msg id is " << msg_node->recv_node->msg_id << std::endl;
        auto call_back_iter = fun_callbacks.find(msg_node->recv_node->msg_id);
        if (call_back_iter == fun_callbacks.end()) {
            msg_que.pop();
            std::cout << "msg id [" << msg_node->recv_node->msg_id << "] handler not found" << std::endl;
            continue;
        }
        call_back_iter->second(msg_node->_session, msg_node->recv_node->msg_id, 
            std::string(msg_node->recv_node->_data, msg_node->recv_node->cur_len));
        msg_que.pop();
    }
}

void LogicSystem::RegisterCallBack()
{
    fun_callbacks[MSG_CHAT_LOGIN] = std::bind(&LogicSystem::LoginHandler, this, 
        placeholders::_1, placeholders::_2, placeholders::_3);

    fun_callbacks[ID_SEARCH_USER_REQ] = std::bind(&LogicSystem::SearchInfo, this, 
        placeholders::_1, placeholders::_2, placeholders::_3);

    fun_callbacks[ID_ADD_FRIEND_REQ] = std::bind(&LogicSystem::AddFriendApply, this,
        placeholders::_1, placeholders::_2, placeholders::_3);

    fun_callbacks[ID_AUTH_FRIEDN_REQ] = std::bind(&LogicSystem::AuthFriendApply, this,
        placeholders::_1, placeholders::_2, placeholders::_3);

    fun_callbacks[ID_TEXT_CHAT_MSG_REQ] = std::bind(&LogicSystem::DealChatTextMsg, this,
        placeholders::_1, placeholders::_2, placeholders::_3);
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto token = root["token"].asString();
    std::cout << "user login uid is " << uid << "user token is " << token << std::endl;
    
    Json::Value rtvalue;
    Defer defer([this, &rtvalue, session]() {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, MSG_CHAT_LOGIN_RSP);
        });

    //从redis中获取匹配token
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
    if (!success) {
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return;
    }
    if (token_value != token) {
        rtvalue["error"] = ErrorCodes::TokenInvalid;
        return;
    }
    rtvalue["error"] = ErrorCodes::Success;

    //内存中查询用户信息
    std::string base_key = USER_BASE_INFO + uid_str;
    auto user_info = std::make_shared<UserInfo>();
    bool b_base = GetBaseInfo(base_key, uid, user_info);
    if (!b_base) {
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return;
    }
    rtvalue["uid"] = uid;
    rtvalue["passwd"] = user_info->passwd;
    rtvalue["name"] = user_info->name;
    rtvalue["email"] = user_info->email;
    rtvalue["nick"] = user_info->nick;
    rtvalue["desc"] = user_info->desc;
    rtvalue["sex"] = user_info->sex;
    rtvalue["icon"] = user_info->icon;

    //从数据库获取申请列表
    std::vector<std::shared_ptr<ApplyInfo>> apply_list;
    auto b_apply = GetFriendApplyInfo(uid, apply_list);
    if (b_apply) {
        for (auto& apply : apply_list) {
            Json::Value obj;
            obj["name"] = apply->_name;
            obj["uid"] = apply->_uid;
            obj["icon"] = apply->_icon;
            obj["nick"] = apply->_nick;
            obj["sex"] = apply->_sex;
            obj["desc"] = apply->_desc;
            obj["status"] = apply->_status;
            rtvalue["apply_list"].append(obj);
        }
    }

    //获取好友列表
    std::vector<std::shared_ptr<UserInfo>> friend_list;
    bool b_friend_list = GetFriendList(uid, friend_list);
    for (auto& friedn_ele : friend_list) {
        Json::Value obj;
        obj["name"] = friedn_ele->name;
        obj["uid"] = friedn_ele->uid;
        obj["icon"] = friedn_ele->icon;
        obj["nick"] = friedn_ele->nick;
        obj["sex"] = friedn_ele->sex;
        obj["desc"] = friedn_ele->desc;
        obj["back"] = friedn_ele->back;
        rtvalue["friend_list"].append(obj);
    }

    auto server_name = ConfigMgr::Inst().GetValue("SelfServer", "Name");
    //将登录数量增加
    auto rd_res = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server_name);
    int count = 0;
    if (!rd_res.empty()) {
        count = std::stoi(rd_res);
    }
    count++;

    auto count_str = std::to_string(count);
    RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, count_str);

    //session绑定用户uid
    session->SetUserId(uid);

    //为用户设置登录ip server的名字
    std::string ipkey = USERIPPREFIX + uid_str;
    RedisMgr::GetInstance()->Set(ipkey, server_name);

    //uid绑定session，踢人操作
    UserMgr::GetInstance()->SetUserSession(uid, session);
    return;
}

void LogicSystem::SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid_str = root["uid"].asString();
    std::cout << "user SearchInfo uid is " << uid_str << endl;

    Json::Value rtvalue;
    Defer defer([this, &rtvalue, session]() {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, ID_SEARCH_USER_RSP);
        });

    bool b_digit = isPureDigit(uid_str);
    if (b_digit) {
        GetUserByUid(uid_str, rtvalue);
    }
    else {
        GetUserByName(uid_str, rtvalue);
    }
    return;
}

void LogicSystem::AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto applyname = root["applyname"].asString();
    auto bakname = root["bakname"].asString();
    auto touid = root["touid"].asInt();
    std::cout << "user login uid is " << uid << ",applyname is " << applyname 
        << ",bakname is " << bakname << ",touid is " << touid << std::endl;

    Json::Value rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    Defer defer([this, &rtvalue, session]() {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, ID_ADD_FRIEND_RSP);
        });

    //更新好友请求数据库
    MysqlMgr::GetInstance()->AddFriendApply(uid, touid);

    //查询redis，查找touid对应的server ip
    auto to_str = std::to_string(touid);
    auto to_ip_key = USERIPPREFIX + to_str;
    std::string to_ip_value = "";
    bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
    if (!b_ip) {
        return;
    }
    
    //读取自己的server ip
    auto& cfg = ConfigMgr::Inst();
    auto self_name = cfg["SelfServer"]["Name"];

    std::string base_key = USER_BASE_INFO + std::to_string(uid);
    auto apply_info = std::make_shared<UserInfo>();
    bool b_info = GetBaseInfo(base_key, uid, apply_info);

    //如果双方再同一个server
    if (to_ip_value == self_name) {
        auto session = UserMgr::GetInstance()->GetSession(touid);

        //如果连接不为空，说明对方在线，直接内存中通知对方
        if (session) {
            Json::Value notify;
            notify["error"] = ErrorCodes::Success;
            notify["applyuid"] = uid;
            notify["name"] = applyname;
            notify["desc"] = "";
            notify["icon"] = apply_info->icon;
            notify["sex"] = apply_info->sex;
            notify["nick"] = apply_info->nick;
            std::string return_str = notify.toStyledString();
            session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
        }

        return;
    }

    AddFriendReq add_req;
    add_req.set_applyuid(uid);
    add_req.set_touid(touid);
    add_req.set_name(applyname);
    add_req.set_desc("");
    if (b_info) {
        add_req.set_icon(apply_info->icon);
        add_req.set_sex(apply_info->sex);
        add_req.set_nick(apply_info->nick);
    }

    ChatGrpcClient::GetInstance()->NotifyAddFriend(to_ip_value, add_req);
}

void LogicSystem::AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);

    auto uid = root["fromuid"].asInt();
    auto touid = root["touid"].asInt();
    auto back_name = root["back_name"].asString();
    std::cout << "from " << uid << " auth friend to " << touid << std::endl;

    Json::Value rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    auto user_info = std::make_shared<UserInfo>();

    std::string base_key = USER_BASE_INFO + std::to_string(touid);
    bool b_info = GetBaseInfo(base_key, touid, user_info);
    if (b_info) {
        rtvalue["uid"] = touid;
        rtvalue["name"] = user_info->name;
        rtvalue["nick"] = user_info->nick;
        rtvalue["icon"] = user_info->icon;
        rtvalue["sex"] = user_info->sex;
    }
    else {
        rtvalue["error"] = ErrorCodes::UidInvalid;
    }

    Defer defer([this, &rtvalue, session]() {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, ID_AUTH_FRIEND_RSP);
        });

    //更新数据库
    MysqlMgr::GetInstance()->AuthFriendApply(uid, touid);

    //更新数据库添加好友
    MysqlMgr::GetInstance()->AddFriend(uid, touid, back_name);

    //查询redis，查找touid对应server ip
    auto to_str = std::to_string(touid);
    auto to_ip_key = USERIPPREFIX + to_str;
    std::string to_ip_value = "";
    bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
    if (!b_ip) {
        return;
    }

    auto& cfg = ConfigMgr::Inst();
    auto self_name = cfg["SelfServer"]["Name"];
    //直接通知对方有验证通过消息
    if (to_ip_value == self_name) {
        auto session = UserMgr::GetInstance()->GetSession(touid);
        if (session) {
            Json::Value notify;
            notify["error"] = ErrorCodes::Success;
            notify["fromuid"] = uid;
            notify["touid"] = touid;
            std::string base_key = USER_BASE_INFO + std::to_string(uid);
            auto user_info = std::make_shared<UserInfo>();
            bool b_info = GetBaseInfo(base_key, uid, user_info);
            if (b_info) {
                notify["name"] = user_info->name;
                notify["nick"] = user_info->nick;
                notify["icon"] = user_info->icon;
                notify["sex"] = user_info->sex;
            }
            else {
                notify["error"] = ErrorCodes::UidInvalid;
            }

            std::string return_str = notify.toStyledString();
            session->Send(return_str, ID_NOTIFY_AUTH_FRIEND_REQ);
        }
        return;
    }

    AuthFriendReq auth_req;
    auth_req.set_fromuid(uid);
    auth_req.set_touid(touid);

    //发送通知
    ChatGrpcClient::GetInstance()->NotifyAuthFriend(to_ip_value, auth_req);
}

void LogicSystem::DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);

    auto uid = root["fromuid"].asInt();
    auto touid = root["touid"].asInt();

    const Json::Value arrays = root["text_array"];

    Json::Value rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["text_array"] = arrays;
    rtvalue["fromuid"] = uid;
    rtvalue["touid"] = touid;

    Defer defer([this, &rtvalue, session]() {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, ID_TEXT_CHAT_MSG_RSP);
        });

    //查询redis查找touid对应server ip
    auto to_str = std::to_string(touid);
    auto to_ip_key = USERIPPREFIX + to_str;
    std::string to_ip_value = "";
    bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
    if (!b_ip) {
        return;
    }

    auto& cfg = ConfigMgr::Inst();
    auto self_name = cfg["SelfServer"]["Name"];
    //直接通知对方
    if (to_ip_value == self_name) {
        auto session = UserMgr::GetInstance()->GetSession(touid);
        if (session) {
            //在内存中则直接发送通知
            std::string return_str = rtvalue.toStyledString();
            session->Send(return_str, ID_TEXT_CHAT_MSG_REQ);
        }
        return;
    }

    TextChatMsgReq text_msg_req;
    text_msg_req.set_fromuid(uid);
    text_msg_req.set_touid(touid);
    for (const auto& txt_obj : arrays) {
        auto content = txt_obj["content"].asString();
        auto msgid = txt_obj["msgid"].asString();
        std::cout << "content is " << content << std::endl;
        std::cout << "msg_id is " << msgid << std::endl;
        auto* text_msg = text_msg_req.add_textmsgs();
        text_msg->set_msgid(msgid);
        text_msg->set_msgcontent(content);
    }

    //发送通知
    ChatGrpcClient::GetInstance()->NotifyTextChatMsg(to_ip_value, text_msg_req, rtvalue);
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    //优先从redis中查询用户信息
    std::string info_str = "";
    bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->name = root["name"].asString();
        userinfo->passwd = root["passwd"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "user login uid is " << userinfo->uid << ",name is " << userinfo->name
            << ",passwd is " << userinfo->passwd << ",email is " << userinfo->email << std::endl;
    }
    else {
        //否则查询mysql
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlMgr::GetInstance()->GetUser(uid);
        if (user_info == nullptr) {
            return false;
        }

        userinfo = user_info;
        //查询到则把内容写入redis缓存
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["passwd"] = userinfo->passwd;
        redis_root["name"] = userinfo->name;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
    }

    return true;
}

bool LogicSystem::isPureDigit(const std::string& str)
{
    for (unsigned char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

void LogicSystem::GetUserByUid(std::string uid_str, Json::Value& rtvalue)
{
    rtvalue["error"] = ErrorCodes::Success;
    std::string base_key = USER_BASE_INFO + uid_str;

    //优先查redis中查询用户信息
    std::string info_str = "";
    bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        auto uid = root["uid"].asInt();
        auto name = root["name"].asString();
        auto passwd = root["passwd"].asString();
        auto email = root["email"].asString();
        auto nick = root["nick"].asString();
        auto desc = root["desc"].asString();
        auto sex = root["sex"].asInt();
        auto icon = root["icon"].asString();
        std::cout << "user uid is " << uid << ",name is " << name << ",passwd is " 
            << passwd << ",email is " << email << ",icon is " << icon << std::endl;

        rtvalue["uid"] = uid;
        rtvalue["passwd"] = passwd;
        rtvalue["name"] = name;
        rtvalue["email"] = email;
        rtvalue["nick"] = nick;
        rtvalue["desc"] = desc;
        rtvalue["sex"] = sex;
        rtvalue["icon"] = icon;
        return;
    }

    auto uid = std::stoi(uid_str);

    //redis中没有则查询mysql
    //查询数据库
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MysqlMgr::GetInstance()->GetUser(uid);
    if (user_info == nullptr) {
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return; 
    }

    //然后将mysql数据写入redis缓存
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["passwd"] = user_info->passwd;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["icon"] = user_info->icon;

    RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

    //最后返回数据
    rtvalue["uid"] = user_info->uid;
    rtvalue["passwd"] = user_info->passwd;
    rtvalue["name"] = user_info->name;
    rtvalue["email"] = user_info->email;
    rtvalue["nick"] = user_info->nick;
    rtvalue["desc"] = user_info->desc;
    rtvalue["sex"] = user_info->sex;
    rtvalue["icon"] = user_info->icon;
}

void LogicSystem::GetUserByName(std::string name, Json::Value& rtvalue)
{
    rtvalue["error"] = ErrorCodes::Success;

    std::string base_key = NAME_INFO + name;

    //依旧优先查redis
    std::string info_str = "";
    bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        auto uid = root["uid"].asInt();
        auto name = root["name"].asString();
        auto passwd = root["passwd"].asString();
        auto email = root["email"].asString();
        auto nick = root["nick"].asString();
        auto desc = root["desc"].asString();
        auto sex = root["sex"].asInt();
        std::cout << "user uid is " << uid << ",name is " << name << ",passwd is "
            << passwd << ",email is " << email << std::endl;

        rtvalue["uid"] = uid;
        rtvalue["passwd"] = passwd;
        rtvalue["name"] = name;
        rtvalue["email"] = email;
        rtvalue["nick"] = nick;
        rtvalue["desc"] = desc;
        rtvalue["sex"] = sex;
        return;
    }

    //redis中没有则mysql
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MysqlMgr::GetInstance()->GetUser(name);
    if (user_info == nullptr) {
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return;
    }

    //写入redis
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["passwd"] = user_info->passwd;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->sex;
    
    RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
    
    //最后返回数据
    rtvalue["uid"] = user_info->uid;
    rtvalue["passwd"] = user_info->passwd;
    rtvalue["name"] = user_info->name;
    rtvalue["email"] = user_info->email;
    rtvalue["nick"] = user_info->nick;
    rtvalue["desc"] = user_info->desc;
    rtvalue["sex"] = user_info->sex;
    rtvalue["icon"] = user_info->icon;
}

bool LogicSystem::GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list)
{
    return MysqlMgr::GetInstance()->GetApplyList(to_uid, list, 0, 10);
}

bool LogicSystem::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list) {
    return MysqlMgr::GetInstance()->GetFriendList(self_id, user_list);
}
