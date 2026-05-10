#include "RedisMgr.h"
#include "ConfigMgr.h"

RedisPool::RedisPool(size_t poolSize, const char* host, int port, const char* pwd) :
    pool_size(poolSize), _host(host), _port(port), b_stop(false)
{
    for (size_t i = 0; i < pool_size; i++) {
        auto* context = redisConnect(host, port);
        if (!context || context->err != 0) {
            if (context) {              //对象创建成功，连接失败
                redisFree(context);     //关闭连接，释放内存
            }
            continue;
        }                               //创建尽可能多的连接

        auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
        if (reply->type == REDIS_REPLY_ERROR) {
            std::cout << "认证失败" << std::endl;
            freeReplyObject(reply);
            continue;
        }

        freeReplyObject(reply);
        std::cout << "认证成功" << std::endl;
        _cons.push(context);
    }
}

RedisPool::~RedisPool()
{
    std::lock_guard<std::mutex> lock(_mtx);
    while (!_cons.empty()) {
        redisFree(_cons.front());
        _cons.pop();
    }
}

redisContext* RedisPool::getCon()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _cond.wait(lock, [this]() {
        return b_stop || !_cons.empty();
        });
    if (b_stop) {
        return nullptr;
    }
    auto* context = _cons.front();
    _cons.pop();
    return context;
}

void RedisPool::returnCon(redisContext* context)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (b_stop) {
        return;
    }
    _cons.push(context);
    _cond.notify_one();
}

void RedisPool::Close()
{
    b_stop = true;
    _cond.notify_all();
}



RedisMgr::RedisMgr()
{
    auto& gCfgMgr = ConfigMgr::Inst();
    std::string host = gCfgMgr["Redis"]["Host"];
    std::string port = gCfgMgr["Redis"]["Port"];
    std::string pwd = gCfgMgr["Redis"]["Passwd"];
    _pool.reset(new RedisPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

RedisMgr::~RedisMgr()
{
    Close();
}

bool RedisMgr::Get(const std::string& key, std::string& val)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());    //%s拼接字符串，c_str()转为c风格
    
    if (!reply || reply->type != REDIS_REPLY_STRING) {
        std::cout << "[ GET " << key << " ] failed" << std::endl;
        freeReplyObject(reply);      //失败则释放reply链接
        _pool->returnCon(connect);
        return false;
    }

    val = reply->str;
    freeReplyObject(reply);
    std::cout << "Succeed to execute command [ GET " << key << " ]" << std::endl;
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::Set(const std::string& key, const std::string& val)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "SET %s %s", key.c_str(), val.c_str());
    
    if (!reply) {
        std::cout << "Set reply is null!" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }

    if (reply->type != REDIS_REPLY_STATUS || strcmp(reply->str, "OK") != 0) {     //strcmp相同返回0
        std::cout << "Execute command [ SET " << key << " " << val << " ] failed !" << std::endl;
        freeReplyObject(reply); 
        _pool->returnCon(connect);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Succeed to execute command [ SET " << key << " " << val << " ]" << std::endl;
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::LPush(const std::string& key, const std::string& val)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), val.c_str());

    if (!reply || 
        reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Execute command [ LPUSH " << key << " " << val << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    std::cout << "Execute command [ LPUSH " << key << " " << val << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& val)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "LPOP %s", key.c_str());

    if (!reply || reply->type == REDIS_REPLY_NIL) {
        std::cout << "Execute command [ LPOP " << key << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    val = reply->str;
    std::cout << "Execute command [ LPOP " << key << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& val)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), val.c_str());

    if (!reply ||
        reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Execute command [ RPUSH " << key << " " << val << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    std::cout << "Execute command [ RPUSH " << key << " " << val << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& val)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "RPOP %s", key.c_str());

    if (!reply || reply->type == REDIS_REPLY_NIL) {
        std::cout << "Execute command [ RPOP " << key << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    val = reply->str;
    std::cout << "Execute command [ RPOP " << key << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& hval)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), hval.c_str());
    
    if (!reply || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execute command [ HSet " << key << " " << hkey << " " << hval << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    std::cout << "Execute command [ HSet " << key << " " << hkey << " " << hval << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hval, size_t hval_len)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hval;
    argvlen[3] = hval_len;

    auto reply = (redisReply*)redisCommandArgv(connect, 4, argv, argvlen);
    if (!reply || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execute command [ HSet " << key << " " << hkey << " " << hval << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    std::cout << "Execute command [ HSet " << key << " " << hkey << " " << hval << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return "";
    }

    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();

    auto reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);
    if (!reply || reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        std::cout << "Execute command [ HGet " << key << " " << hkey << " " << " ] failed !" << std::endl;
        _pool->returnCon(connect);
        return "";
    }
    std::string val = reply->str;
    freeReplyObject(reply);
    std::cout << "Execute command [ HGet " << key << " " << hkey << " ] succeed !" << std::endl;
    _pool->returnCon(connect);
    return val;
}

bool RedisMgr::Del(const std::string& key)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());

    if (!reply || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execute command [ Del " << key << " ] failed !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    std::cout << "Execute command [ Del " << key << " ] succeed !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

bool RedisMgr::HDel(const std::string& key, const std::string& field)
{
    auto connect = _pool->getCon();
    if (connect == nullptr) {
        return false;
    }

    Defer defer([&connect, this]() {
        _pool->returnCon(connect);
        });

    redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
    if (reply == nullptr) {
        std::cerr << "HDEL command failed" << std::endl;
        return false;
    }

    bool success = false;
    if (reply->type == REDIS_REPLY_INTEGER) {
        success = reply->integer > 0;
    }
    freeReplyObject(reply);
    return success;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
    auto connect = _pool->getCon();
    if (!connect) {
        return false;
    }

    auto reply = (redisReply*)redisCommand(connect, "exists %s", key.c_str());

    if (!reply || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
        std::cout << "Not found [ Key " << key << " ] !" << std::endl;
        freeReplyObject(reply);
        _pool->returnCon(connect);
        return false;
    }
    std::cout << "Found [ Key " << key << " ] exists !" << std::endl;
    freeReplyObject(reply);
    _pool->returnCon(connect);
    return true;
}

void RedisMgr::Close()
{
    _pool->Close();
}
