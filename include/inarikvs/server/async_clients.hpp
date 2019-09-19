#ifndef	__inarikvs__server__async_clients__hpp
#define	__inarikvs__server__async_clients__hpp
#include <functional>
#include <mutex>
#include <thread>
#include <amtrs/net.hpp>
#include <amtrs/mutex.hpp>
#include "../inarikvs-def.hpp"
#include "../inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN




class	client
{
public:
	using	socket_type	= amtrs::net::socket;

	client(socket_type&& _s)
		: mSocket(std::move(_s))
	{}

	int recv(void* _buff, std::size_t _size)
	{
		return	mSocket.recv(_buff, _size);
	}

	int send(const void* _buff, std::size_t _size)
	{
		if (auto qs = send_buffer_size())
		{
			auto	ss	= mSocket.send(mSendBuffer.data() + mSendPos, qs);
			if (ss > 0)
			{
				// 送信したのでバッファを進める
				mSendPos += ss;
				if (mSendPos < mSendBuffer.size())
				{
					// まだ送信バッファが残っているのでバッファにデータを追加して終了
					add_send_request(_buff, _size);
					return	0;
				}
			}
			else
			{
				// 送信できなかった
				add_send_request(_buff, _size);
				return	0;
			}
		}

		auto	ss	= mSocket.send(_buff, _size);
		if (ss > 0)
		{
			// 送信できなかったのでバッファに追加しておく
			add_send_request(((const char*)_buff) + ss, _size - ss);
		}
		else
		{
			// 送信できなかったのでバッファに追加しておく
			add_send_request(_buff, _size);
		}
		return	ss;
	}

	void disconnect()
	{
		mSocket.clear();
	}

	bool empty() const noexcept
	{
		return	mSocket.empty();
	}

protected:
	void add_send_request(const void* _buff, std::size_t _size)
	{
		if (mSendBuffer.capacity() < _size)
		{
			// バッファのメモリが足りなくなっているのでバッファを詰める。
			std::vector<char>	newBuffer;
			newBuffer.reserve(mSendBuffer.size() + _size);
			newBuffer.assign(mSendBuffer.begin() + mSendPos, mSendBuffer.end());
			mSendPos	= 0;
			mSendBuffer	= std::move(newBuffer);
		}
		mSendBuffer.resize(mSendBuffer.size() + _size);
		auto	dest	= mSendBuffer.data() + mSendPos;
		for (std::size_t i = 0; i < _size; ++i)
		{
			dest[i] = ((const char*)_buff)[i];
		}
	}


	//! 送信待ちのデータサイズを返す。
	std::size_t send_buffer_size() const noexcept
	{
		return	mSendBuffer.size() - mSendPos;
	}

private:
	socket_type			mSocket;
	std::size_t			mSendPos	= 0;
	std::vector<char>	mSendBuffer;
};


class	client_data
{
public:
	using	socket_type	= amtrs::net::socket;

	client_data(socket_type&& _s, std::function<bool(client*)>&& _callback)
		: mClient(std::move(_s))
		, mCallback(std::move(_callback))
	{}

	bool call()
	{
		auto	r	= mCallback(&mClient);
		if (!r)
		{
			mClient.disconnect();
		}
		return	r;
	}

	bool empty() const noexcept { return mClient.empty(); }

private:
	client							mClient;
	std::function<bool(client*)>	mCallback;
};


struct	clients : amtrs::ref_object
{
	using	socket_type	= amtrs::net::socket;
	using	thread_type	= std::thread;

	void start()
	{
		mThread	= thread_type([this]()
		{
			while (!mExit)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));


				if (mAcceptLock.try_lock())
				{
					std::vector<client_data>	queue(std::move(mAcceptQueue));
					mAcceptLock.unlock();
					if (queue.size())
					{
						for (auto& s : queue)
						{
							mSockets.push_back(std::move(s));
						}
					}
				}

				if (mSockets.empty())
				{
					continue;
				}

				for (auto& c : mSockets)
				{
					c.call();
				}

				auto	it	= std::remove_if(mSockets.begin(), mSockets.end(), [](auto& a) -> bool
							{
								return	a.empty();
							});
				mSockets.erase(it, mSockets.end());
			}
		});
	}

	void add(client_data&& _s)
	{
		std::lock_guard<amtrs::spinlock>	lg(mAcceptLock);
		mAcceptQueue.push_back(std::move(_s));
	}

	bool						mExit	= false;

	thread_type					mThread;
	std::vector<client_data>	mSockets;

	amtrs::spinlock				mAcceptLock;
	std::vector<client_data>	mAcceptQueue;
};


class	async_clients
{
public:
	using	socket_type	= amtrs::net::socket;
	using	thread_type	= std::thread;



	void add(socket_type&& _sock, std::function<bool(client*)>&& _callback)
	{
		if (mClients.empty())
		{
			for (int i = 0; i < 1; ++i)
			{
				amtrs::ref<clients>	c	= new clients();
				mClients.push_back(c);
				c->start();
			}
		}
		mClients[0]->add({std::move(_sock), std::move(_callback)});
	}

	void exit()
	{
		for (auto& c : mClients)
		{
			c->mExit	= true;
		}
	}

	std::vector<amtrs::ref<clients>>	mClients;

};



INARIKVS_NAMESPACE_END
#endif
