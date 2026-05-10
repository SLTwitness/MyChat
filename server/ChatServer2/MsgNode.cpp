#include "MsgNode.h"

MsgNode::MsgNode(short max_len) :total_len(max_len), cur_len(0)
{
	_data = new char[total_len + 1];
	_data[total_len] = '\0';
}

MsgNode::~MsgNode()
{
	std::cout << "destruct MsgNode" << std::endl;
	delete[] _data;
}


void MsgNode::Clear() 
{
	std::memset(_data, 0, total_len);
	cur_len = 0;
}

RecvNode::RecvNode(short max_len, short msgId) :MsgNode(max_len), msg_id(msgId)
{
}

SendNode::SendNode(const char* msg, short max_len, short msgId):
	MsgNode(max_len+HEAD_TOTAL_LEN),msg_id(msgId)
{
	//发送id
	short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msgId);
	memcpy(_data, &msg_id_host, HEAD_ID_LEN);

	//转为网络字节序
	short max_len_host= boost::asio::detail::socket_ops::host_to_network_short(max_len);
	memcpy(_data + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
	memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}
