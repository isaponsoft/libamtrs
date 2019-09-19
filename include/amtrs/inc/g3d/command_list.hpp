class	command_list
{
public:
	struct	command_base
			: ref_object
	{
	};

	template<class CommandT, class... Args>
	void add(Args&&... _args)
	{
	}

protected;
	std::list<ref<command_base>>	mCommandList;
};
