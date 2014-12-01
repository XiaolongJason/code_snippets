#ifndef _MARCO_H_
#define _MARCO_H_

/** Declare _TODO(message)  */
#if defined(_MSC_VER)
#	define _DO_PRAGMA(x) __pragma(x)
#	define __STR__(x) #x
#	define __STR(x) __STR__(x)
#	define __FILE__LINE__ __FILE__ "(" __STR(__LINE__) ") : "
#	define _MSG_PRAGMA(_msg) _DO_PRAGMA(message (__FILE__LINE__ _msg))
#elif defined(__GNUC__)
#	define _DO_PRAGMA(x) _Pragma (#x)
#	define _MSG_PRAGMA(_msg) _DO_PRAGMA(message (_msg))
#else
#	define _DO_PRAGMA(x)
#	define _MSG_PRAGMA(_msg) 
#endif

#define WARNING(x) _MSG_PRAGMA("Warning : " #x)
#define TODO(x)	_MSG_PRAGMA("TODO : " #x)
#define TODO_ASSERT(x) TODO(x) assert(0&&#x)

#ifndef MICRO_JOIN
# define MICRO_JOIN(X, Y) MICRO_DO_JOIN( X, Y )
# define MICRO_DO_JOIN(X, Y) MICRO_DO_JOIN2(X,Y)
# define MICRO_DO_JOIN2(X, Y) X##Y
#endif

// 禁用不必要的拷贝和赋值
#define  DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  TypeName& operator=(const TypeName&)


#endif //_MARCO_H_