#include <iostream>
#include <fstream>
#include <amtrs/hashnode/@>

using namespace amtrs;



int main(int _argc, char* _args[])
{
	hash_tree_link<std::string>	s;
	s.add("Val1");
	s.add("Val2");
	s.add("Val3");

	std::cout << "=================" << std::endl;
	for (auto& v : s)
	{
		std::cout << v << std::endl;
	}

	std::cout << "=================" << std::endl;
	s.del("Val2");
	for (auto& v : s)
	{
		std::cout << v << std::endl;
	}
	std::cout << "=================" << std::endl;

	hash_tree_link<std::string>	s2(s);
	std::cout << "================= copy : " << s2.size() << std::endl;
	for (auto& v : s2)
	{
		std::cout << v << std::endl;
	}

	hash_tree_link<std::string>	s3(std::move(s2));
	std::cout << "================= move (s2) : " << s2.size() << std::endl;
	for (auto& v : s2)
	{
		std::cout << v << std::endl;
	}

	std::cout << "================= move (s3) : " << s3.size() << std::endl;
	for (auto& v : s3)
	{
		std::cout << v << std::endl;
	}

	for (auto it = s3.rbegin(); it != s3.rend(); ++it)
	{
		std::cout << *it << std::endl;
	}

	// map型
	hash_tree_link<std::pair<std::string, int>>	si;
	si.add(decltype(si)::value_type("VAL1", 1));
	std::cout << si.get("VAL1")->second << std::endl;
	si.del("VAL1");


	return	0;
}

