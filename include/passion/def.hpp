#ifndef	__passion__def__hpp
#define	__passion__def__hpp
#ifdef	__cplusplus
#define	PASSION_NAMESPACE							psn
#define	PASSION_NAMESPACE_FULL						::PASSION_NAMESPACE
#define	PASSION_NAMESPACE_BEGIN						namespace PASSION_NAMESPACE {
#define	PASSION_NAMESPACE_END						}
#define	PASSION_NAMESPACE_IMPLIMENTS_BEGIN(_name)	namespace _name##_impl { 
#define	PASSION_NAMESPACE_IMPLIMENTS_END			}
#define	PASSION_NAMESPACE_IMPLIMENTS(_name)			PASSION_NAMESPACE::_name##_impl
PASSION_NAMESPACE_BEGIN
PASSION_NAMESPACE_END
#endif	// __cplusplus
#endif
