#pragma once
#include "const.h"

class LogicSystem;
class MsgNode
{
public:
	MsgNode(short max_len);
	~MsgNode();
	void Clear();

	short cur_len;
	short total_len;
	char* _data;
};

class RecvNode :public MsgNode {
	friend class LogicSystem;
public:
	RecvNode(short max_len, short msgId);
private:
	short msg_id;
};


class SendNode :public MsgNode {
	friend class LogicSystem;
public:
	SendNode(const char* msg, short max_len, short msgId);
private:
	short msg_id;
};
