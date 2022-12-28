struct	filetimes
{
	ssring	filename;
	time_t	modified;
};

struct	compilecache
{
	sstring					file;
	simplearray<filetimes>	depenences;
	psnvm::bytecodes		bytecode;
};


int main()
{
	hash_map<sstring, compilecache>	cache;
	psnvm::executer					exe;


	if (FCGI_accept() > 0)
	{
		sstring	url	= ...;

		auto	c	= cache.find(url);
		if (c)
		{
			if (is_modified())
			{
				psnvm::compiler	cmp;
				cmp.compile(url);
				c->depenences	= std::move(cmp.depenences);
				c->bytecode		= std::move(cmp.bytecode);
			}
		}
		else
		{
			psnvm::compiler	cmp;
			cmp.compile(url);
			c	= cache.insert(url, {
				url,
				std::move(cmp.depenences),
				std::move(cmp.bytecode),
			});
		}

		psnvm::executer	e(&exe);
		e.run(c->bytecode);
	}
	return	0;
}
