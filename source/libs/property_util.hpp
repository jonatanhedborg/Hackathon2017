/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

property_util.hpp - v0.1 - Helper functions for creating property instances.

Dependencies: 
    property.hpp
    refcount.hpp
*/

#ifndef property_util_hpp
#define property_util_hpp

#include "property.hpp"
#include "refcount.hpp"

namespace property {
	
template< typename T > refcount::ref< property<T> > make_property( T* variable );
template< typename T > refcount::ref< property<T> > make_property( T (*get)(), void (*set)( T ) );


template< typename T, typename U > refcount::ref< property<T> > make_property( T (*get)( U* ), void (*set)( T, U* ), U* usr );
template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T U::*member );
template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T (U::*get)(), void (U::*set)( T ) );
template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T (U::*get)() const, void (U::*set)( T ) );
template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T (U::*get)() const, U& (U::*set)( T ) );

template< typename T, typename U > refcount::ref< property<T> > make_property( T (*get)( U* ), void (*set)( T,  U* ), refcount::ref<U> const& usr );
template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T U::*member );
template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T (U::*get)(), void (U::*set)( T ) );
template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T (U::*get)() const, void (U::*set)( T ) );
template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T (U::*get)() const, U& (U::*set)( T ) );

} /* namespace property */

#endif /* property_util_hpp */

/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifndef property_util_impl
#define property_util_impl

namespace property { namespace internal {


template< typename T > struct variable : property<T>
	{
	virtual T get() const { return *variable_; }
	virtual void set( T const& value ) { *variable_ = value; }	
	private:
		friend refcount::ref<variable>;
		variable( T* var ) : variable_( var ) {  }	
		T* variable_;
	};


template< typename T > struct functions : property<T>
	{
	virtual T get() const { return get_(); }
	virtual void set( T const& value ) { set_( value ); }	
	private:
		friend refcount::ref<functions>;
		functions( T (*get)(), void (*set)( T ) ) : get_( get ), set_( set ) { }
		T (*get_)();
		void (*set_)( T );
	};




template< typename T, typename U > struct functions_usr : property<T>
	{
	virtual T get() const { return get_( usr_ ); }
	virtual void set( T const& value ) { set_( value, usr_ ); }	
	private:
		friend refcount::ref<functions_usr>;
		functions_usr( T (*get)( U* ), void (*set)( T, U* ), U* usr ) : get_( get ), set_( set ), usr_( usr ) { }
		T (*get_)( U* );
		void (*set_)( T, U* );
		U* usr_;
	};


template< typename T, typename U > struct member_var : property<T>
	{
	virtual T get() const { return instance_->*member_; }
	virtual void set( T const& value ) { ( instance_->*member_ ) = value; }	
	private:
		friend refcount::ref<member_var>;
		member_var( U* instance, T U::* member ) : instance_( instance ), member_( member ) { }
		U* instance_;
		T U::* member_;		
	};


template< typename T, typename U > struct member_funcs : property<T>
	{
	virtual T get() const { return (instance_->*get_)(); }
	virtual void set( T const& value ) { (instance_->*set_ )( value ); }	
	private:
		friend refcount::ref<member_funcs>;
		member_funcs( U* instance, T (U::*get)(), void (U::*set)( T ) ) : instance_( instance ), get_( get ), set_( set ) { }
		U* instance_;
		T (U::*get_)();
		void (U::*set_)( T );
	};


template< typename T, typename U > struct member_funcs_const : property<T>
	{
	virtual T get() const { return (instance_->*get_)(); }
	virtual void set( T const& value ) { (instance_->*set_ )( value ); }	
	private:
		friend refcount::ref<member_funcs_const>;
		member_funcs_const( U* instance, T (U::*get)() const, void (U::*set)( T ) ) : instance_( instance ), get_( get ), set_( set ) { }
		U* instance_;
		T (U::*get_)() const;
		void (U::*set_)( T );
	};


template< typename T, typename U > struct member_funcs_const_flow : property<T>
	{
	virtual T get() const { return (instance_->*get_)(); }
	virtual void set( T const& value ) { (instance_->*set_ )( value ); }	
	private:
		friend refcount::ref<member_funcs_const_flow>;
		member_funcs_const_flow( U* instance, T (U::*get)() const, U& (U::*set)( T ) ) : instance_( instance ), get_( get ), set_( set ) { }
		U* instance_;
		T (U::*get_)() const;
		U& (U::*set_)( T );
	};





template< typename T, typename U > struct ref_functions_usr : property<T>
	{
	virtual T get() const { return get_( usr_ ); }
	virtual void set( T const& value ) { set_( value, usr_ ); }	
	private:
		friend refcount::ref<ref_functions_usr>;
		ref_functions_usr( T (*get)( U* ), void (*set)( T, U* ), refcount::ref<U> const& usr ) : get_( get ), set_( set ), usr_( usr ) { }
		T (*get_)( U* );
		void (*set_)( T, U* );
		refcount::ref<U> usr_;
	};


template< typename T, typename U > struct ref_member_var : property<T>
	{
	virtual T get() const { return instance_->*member_; }
	virtual void set( T const& value ) { ( instance_->*member_ ) = value; }	
	private:
		friend refcount::ref<ref_member_var>;
		ref_member_var( refcount::ref<U> const& instance, T U::* member ) : instance_( instance ), member_( member ) { }
		refcount::ref<U> instance_;
		T U::* member_;		
	};


template< typename T, typename U > struct ref_member_funcs : property<T>
	{
	virtual T get() const { return (instance_->*get_)(); }
	virtual void set( T const& value ) { (instance_->*set_ )( value ); }	
	private:
		friend refcount::ref<ref_member_funcs>;
		ref_member_funcs( refcount::ref<U> const& instance, T (U::*get)(), void (U::*set)( T ) ) : instance_( instance ), get_( get ), set_( set ) { }
		refcount::ref<U> instance_;
		T (U::*get_)();
		void (U::*set_)( T );
	};


template< typename T, typename U > struct ref_member_funcs_const : property<T>
	{
	virtual T get() const { return (instance_->*get_)(); }
	virtual void set( T const& value ) { (instance_->*set_ )( value ); }	
	private:
		friend refcount::ref<ref_member_funcs_const>;
		ref_member_funcs_const( refcount::ref<U> const& instance, T (U::*get)() const, void (U::*set)( T ) ) : instance_( instance ), get_( get ), set_( set ) { }
		refcount::ref<U> instance_;
		T (U::*get_)() const;
		void (U::*set_)( T );
	};


template< typename T, typename U > struct ref_member_funcs_const_flow : property<T>
	{
	virtual T get() const { return (instance_->*get_)(); }
	virtual void set( T const& value ) { (instance_->*set_ )( value ); }	
	private:
		friend refcount::ref<ref_member_funcs_const_flow>;
		ref_member_funcs_const_flow( refcount::ref<U> const& instance, T (U::*get)() const, U& (U::*set)( T ) ) : instance_( instance ), get_( get ), set_( set ) { }
		refcount::ref<U> instance_;
		T (U::*get_)() const;
		U& (U::*set_)( T );
	};

} /* namespace internal */ } /* namespace property */



namespace property {


template< typename T > refcount::ref< property<T> > make_property( T* variable )
	{
	return refcount::ref< internal::variable<T> >::make_new( variable );
	}

template< typename T > refcount::ref< property<T> > make_property( T (*get)(), void (*set)( T ) )
	{
	return refcount::ref< internal::functions<T> >::make_new( get, set );
	}



template< typename T, typename U > refcount::ref< property<T> > make_property( T (*get)( U* ), void (*set)( T,  U* ), U* usr )
	{
	return refcount::ref< internal::functions_usr<T, U> >::make_new( get, set, usr );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T U::* member )
	{
	return refcount::ref< internal::member_var<T, U> >::make_new( instance, member );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T (U::*get)(), void (U::*set)( T ) )
	{
	return refcount::ref< internal::member_funcs<T, U> >::make_new( instance, get, set );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T (U::*get)() const, void (U::*set)( T ) )
	{
	return refcount::ref< internal::member_funcs_const<T, U> >::make_new( instance, get, set );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( U* instance, T (U::*get)() const, U& (U::*set)( T ) )
	{
	return refcount::ref< internal::member_funcs_const_flow<T, U> >::make_new( instance, get, set );
	}



template< typename T, typename U > refcount::ref< property<T> > make_property( T (*get)( U* ), void (*set)( T,  U* ), refcount::ref<U> const& usr )
	{
	return refcount::ref< internal::ref_functions_usr<T, U> >::make_new( get, set, usr );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T U::* member )
	{
	return refcount::ref< internal::ref_member_var<T, U> >::make_new( instance, member );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T (U::*get)(), void (U::*set)( T ) )
	{
	return refcount::ref< internal::ref_member_funcs<T, U> >::make_new( instance, get, set );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T (U::*get)() const, void (U::*set)( T ) )
	{
	return refcount::ref< internal::ref_member_funcs_const<T, U> >::make_new( instance, get, set );
	}

template< typename T, typename U > refcount::ref< property<T> > make_property( refcount::ref<U> const& instance, T (U::*get)() const, U& (U::*set)( T ) )
	{
	return refcount::ref< internal::ref_member_funcs_const_flow<T, U> >::make_new( instance, get, set );
	}

} /* namespace property */


#endif /* property_util_impl */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2015 Mattias Gustavsson

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

------------------------------------------------------------------------------

ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------
*/
