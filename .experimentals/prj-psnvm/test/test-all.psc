// psnvm tutorial/test.psn | grep -E "(OK|NG)"

string	_scriptDir;
int		_exit;


int main(string thisname)
{
	_scriptDir	= "test";
	_exit	= 0;

	go("0001-flow-1.psc");
	go("0001-flow-2.psc");
	go("0001-constractor.psc");
	go("0001-destractor.psc");
	go("0002-member.psc");
	go("0003-function.psc");
	go("0003-function-2.psc");
	go("0004-four-arithmetic-operation.psc");
	go("0005-pointer.psc");
	go("0006-functionpointer.psc");
	go("0007-namespace.psc");
	go("0008-compaund-assign.psc");

	_scriptDir	= "examples";
	go("001-HelloWorld.psc");
	go("002-string.psc");
	go("003-if_for_while.psc");
	go("004-function.psc");
	go("005-main.psc");
	go("006-class.psc");
	go("007-inner-class.psc");
	go("008-array.psc");
	go("009-decltype.psc");
	go("010-reference.psc");
	go("011-constractor.psc");
	go("012-global_variable.psc");
	go("013-class-upcast.psc");
	go("014-override.psc");
	go("100-regex.psc");
	go("200-ppp.psc");
	go("201-ppp-nested.psc");
	go("202-ppp-undef.psc");
	go("203-ppp-include.psc");
	go("204-ppp-ifdef.psc");
	go("900-tower_of_hanoi.psc");

	return	_exit;
}


int go(string script)
{
	string	vm	= vmpath();
	string	cmd	= "${vm} ${_scriptDir}/${script}";

	auto	p	= new process();
	auto	r	= p.run("${cmd}");
	if (r)
	{
		printf("${script} : NG\n");
		_exit	= 1;
	}
	else
	{
		printf("${script} : OK\n");
	}

	return	0;
}
