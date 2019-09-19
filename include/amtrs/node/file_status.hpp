/*
 * File status monitor node.
 */
#ifndef	__libamtrs__node__file_status__hpp
#define	__libamtrs__node__file_status__hpp
#include "../filesystem.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! ファイル状態を表示する
// ----------------------------------------------------------------------------
template<class NodeT>
struct	file_status_traits
{
	using	path_type			= filesystem::path;
};


// ============================================================================
//! ファイル状態を表示する
// ----------------------------------------------------------------------------
template<class NodeT, class Traits = file_status_traits<NodeT>>
class	file_status_node
		: public NodeT
{
public:
	using	traits_type		= file_status_traits<NodeT>;
	using	path_type		= typename traits_type::path_type;


	file_status_node(path_type&& _path)
		: mTarget(_path)
	{}



private:
	path_type		mTarget;

};


AMTRS_NAMESPACE_END
#endif
