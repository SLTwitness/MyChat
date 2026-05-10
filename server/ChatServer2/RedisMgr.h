#pragma once
#include "const.h"

class RedisPool {
public:
	RedisPool(size_t poolSize, const char* host, int port, const char* pwd);
	~RedisPool();
	redisContext* getCon();
	void returnCon(redisContext* context);
	void Close();
private:
	std::atomic<bool> b_stop;
	size_t pool_size;
	const char* _host;
	int _port;
	std::queue<redisContext*> _cons;
	std::mutex _mtx;
	std::condition_variable _cond;
};

class RedisMgr:public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	bool Get(const std::string& key, std::string& val);
	bool Set(const std::string& key, const std::string& val);
	bool LPush(const std::string& key, const std::string& val);
	bool LPop(const std::string& key, std::string& val);
	bool RPush(const std::string& key, const std::string& val);
	bool RPop(const std::string& key, std::string& val);
	bool HSet(const std::string& key, const std::string& hkey, const std::string& hval);
	bool HSet(const char* key, const char* hkey, const char* hval, size_t hval_len);
	std::string HGet(const std::string& key, const std::string& hkey);
	bool Del(const std::string& key);
	bool HDel(const std::string& key, const std::string& field);
	bool ExistsKey(const std::string& key);
	void Close();
private:
	RedisMgr();

	std::unique_ptr<RedisPool> _pool;
};

