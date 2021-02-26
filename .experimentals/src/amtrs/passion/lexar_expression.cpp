/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <passion/lexar.hpp>
#include <passion/lexar_rules.hpp>
PASSION_NAMESPACE_BEGIN


vmstring read_expression(int& _error, lexar& _lexar)
{
	lexar::cursor	backup(_lexar.position());
	lexar::cursor	next;
	vmstring		retval;
	_error	= 0;


	return	retval;
}


PASSION_NAMESPACE_END
