#ifndef CTOREXCEPTION_H_
#define CTOREXCEPTION_H_

#pragma once

//http://www.cplusplus.com/doc/tutorial/exceptions/
class CtorException {
	virtual const char* what() const throw() {
		return "Class DEFAULT constructor was called";
	};
} defaultCtorExcept;

#endif //CTOREXCEPTION_H_