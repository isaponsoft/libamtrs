#ifndef	__inarikvs__command__hpp
#define	__inarikvs__command__hpp
#include "inaribase/database.hpp"
INARIKVS_NAMESPACE_BEGIN

class	command
{
public:
	using	argv	= amtrs::arguments;

	command(database* _db)
		: mCurrentDatabase(_db)
	{}

	bool operator () (argv _argv)
	{
		while (!_argv.empty() && exec(_argv))
		{}
		return	true;
	}

	bool exec(argv& _argv)
	{
		if (_argv.empty())
		{
			// コマンドそのものがない
			return	false;
		}

		std::string_view	cmd(_argv.shift());

		// 値を取得する
		//	get key[ key...]
		if (std::string_view("get") == cmd)
		{
			while (!_argv.empty())
			{
				auto	key	= std::string_view(_argv.shift());
				if (std::string_view(";") == key)
				{
					return	true;
				}

				if (auto* kd = db().get(std::string_view(key)); kd)
				{
					std::cout << kd->value.data << std::endl;
				}
				else
				{
					std::cout << "* NOTHING *" << key << std::endl;
				}
			}
			return	true;
		}

		// 値をセットする
		//	set key value
		if (std::string_view("set") == cmd)
		{
			if (_argv.size() < 2)
			{
				// 不明
				return	false;
			}

			std::string_view	key(_argv.shift());
			std::string_view	value(_argv.shift());
			db().set(key, value);
			if (!_argv.empty())
			{
				if (std::string_view(";") != std::string_view(_argv.front()))
				{
					return	false;
				}
				_argv.shift();
			}

			return	true;
		}

		// 値を削除する
		//	set key value
		if (std::string_view("del") == cmd)
		{
			while (!_argv.empty())
			{
				auto	key	= std::string_view(_argv.shift());
				if (std::string_view(";") == key)
				{
					return	true;
				}
				db().del(key);
			}
			return	true;
		}
#if	0
		// すべてのキーをダンプする
		if (std::string_view("dump") == cmd)
		{
			db().each([&](kv_data* _kv) -> bool
			{
				std::cout << '\"' << std::string_view(_kv->key) << "\":\"" << std::string_view(_kv->value) << '\"' << std::endl;
				return	false;
			});
			if (!_argv.empty())
			{
				if (std::string_view(";") != std::string_view(_argv.front()))
				{
					return	false;
				}
				_argv.shift();
			}
			return	true;
		}
#endif


		// 不明なコマンド
std::cout << "Unkown " << cmd;
for (auto v : _argv)
{
	std::cout << std::string_view(v) << cmd;
}

		return	false;
	}

	database& db() noexcept { return *mCurrentDatabase; }

private:
	database*	mCurrentDatabase	= nullptr;
};

INARIKVS_NAMESPACE_END
#endif
