#ifndef BUFFER_H
#define BUFFER_H
#include <mutex>
#include <queue>
#include <atomic>
#include <set>
#include <thread>
#include <numeric>
#include <iostream>
#define TRY_LOCK_FOR [&](const int _1_millisecond)->bool{\
			int count = 0;\
			while (!lock.try_lock()) {\
				if (count >= _1_millisecond) {\
					return false;\
				}\
				count++;\
				std::this_thread::sleep_for(std::chrono::milliseconds(1));\
			}\
			return true;}
template<class QueueData>class BlockingQueue
{
private:
	std::queue<QueueData*> m_queue;						// Use STL queue to store data
	std::mutex		  m_mutex;
	bool m_done;
	std::size_t m_max_size;
	std::string m_name;

	BlockingQueue(const BlockingQueue &);

	BlockingQueue* operator=(const BlockingQueue &);

	BlockingQueue(BlockingQueue &&);

	BlockingQueue* operator=(BlockingQueue &&);

public:

	BlockingQueue(const std::string &_name,
				std::size_t _max_size=0)
	{
		m_done = false;
		m_max_size = _max_size;
		m_name = _name;
	}

	~BlockingQueue() {
		ShutDown();
		CleanUp();
	};

	void Insert(QueueData *_data)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_done) {
			return;
		}
		while(m_max_size!=0 && m_queue.size()>=m_max_size){
			lock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			lock.lock();
			if (m_done) {
				lock.unlock();
				return;
			}
		}
		m_queue.push(_data);
		return;
	}

	bool Insert_try(QueueData *_data)
	{
		std::unique_lock<std::mutex> lock(m_mutex,std::defer_lock);
		if (!TRY_LOCK_FOR(10)) {
			return false;
		}
		if (m_done) {
			return true;
		}
		if(m_max_size==0 || m_queue.size()<m_max_size){
			m_queue.push(_data);
			return true;
		}
		return false;		
	}
	int Remove_try(QueueData **_data)
	{
		std::unique_lock<std::mutex> lock(m_mutex,std::defer_lock);
		if (!TRY_LOCK_FOR(500)) {
			return 2;
		}

		if (m_queue.size() == 0)
		{
			if (m_done) {
				return 0;
			}
			return 2;
		}
		*_data = &(*m_queue.front());
		m_queue.pop();
		return 1;
	};

	int Remove(QueueData **_data) {
		while (true) {
			auto c = Remove_try(_data);
			if (c == 0 || c == 1) {
				return c;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	int CanInsert()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_done) {
			return 0;
		}
		return 1;
	};


	void ShutDown()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_done = true;
	}

	void Restart() {
		CleanUp();
		std::unique_lock<std::mutex> lock(m_mutex);
		m_done = false;
	}

	bool IsShutDown() {
		std::unique_lock<std::mutex> lock(m_mutex);
		bool val= m_done;
		return val;
	}

	void CleanUp()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while (!m_queue.empty())
		{
			m_queue.pop();
		}
	}
	std::size_t size(){
		std::size_t s = 0;
		std::unique_lock<std::mutex> lock(m_mutex);
		s = m_queue.size();
		return s;
	}
};

#endif