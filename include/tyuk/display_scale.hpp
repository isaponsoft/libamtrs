#ifndef	__toyoukebime__display_scale__hpp
#define	__toyoukebime__display_scale__hpp
#include "tyuk-def.hpp"
#include "singleton_creater.hpp"
TYUK_NAMESPACE_BEGIN

class	display_scale
		: public singleton<display_scale, singleton_creator<display_scale>>
		, public ref_object
{
public:

	void set_scale(float _scale) noexcept { mScale = _scale; }

	template<class T>
	T to_physical(T _value) const noexcept { return static_cast<T>(static_cast<float>(_value) * mScale); }


private:
	float	mScale	= 1.0f;
};



TYUK_NAMESPACE_END
#endif
