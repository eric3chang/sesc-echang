/*
 * to_string.h
 *
 *  Created on: ago 23, 2010
 *      Author: echang
 */

#ifndef TO_STRING_H_
#define TO_STRING_H_

#include <sstream>

template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}

#endif /* TO_STRING_H_ */
