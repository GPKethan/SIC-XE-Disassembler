#pragma once

//http://www.cplusplus.com/doc/tutorial/exceptions/
class CtorException {
	virtual const char* what() const throw() {
		return "Class DEFAULT constructor was called";
	};
} defaultCtorExcept;