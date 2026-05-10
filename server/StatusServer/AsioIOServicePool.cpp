#include "AsioIOServicePool.h"

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	size_t index = _nextIOService.fetch_add(1);		//原子操作以防线程竞争
	return _ioServices[index % _ioServices.size()];
}

void AsioIOServicePool::Stop()
{
	for (auto& work : _works) {
		work->reset();
	}

	for (auto& t : _threads) {
		t.join();
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size) :
	_ioServices(size), _works(size), _nextIOService(0) {
	for (std::size_t i = 0; i < size; i++) {
		_works[i] = std::make_unique<Work>(
			boost::asio::make_work_guard(_ioServices[i])
		);				//存入_works维持生命周期
	}			//阻塞线程保持运行

	for (std::size_t i = 0; i < _ioServices.size(); i++) {
		_threads.emplace_back([this, i](){
			_ioServices[i].run();
		});
	}
}
