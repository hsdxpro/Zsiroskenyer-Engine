
#ifndef BOOST_MPL_AUX_NA_HPP_INCLUDED
#define BOOST_MPL_AUX_NA_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: na.hpp 86245 2013-10-11 23:17:48Z skelly $
// $Date: 2013-10-12 01:17:48 +0200 (szo, 12 okt 2013) $
// $Revision: 86245 $

#include <boost/mpl/bool.hpp>
#include <boost/mpl/aux_/na_fwd.hpp>
#include <boost/mpl/aux_/config/msvc.hpp>
#include <boost/mpl/aux_/config/ctps.hpp>

namespace boost { namespace mpl {

template< typename T >
struct is_na
    : false_
{
};

template<>
struct is_na<na>
    : true_
{
};

template< typename T >
struct is_not_na
    : true_
{
};

template<>
struct is_not_na<na>
    : false_
{
};

template< typename T, typename U > struct if_na
{
    typedef T type;
};

template< typename U > struct if_na<na,U>
{
    typedef U type;
};

}}

#endif // BOOST_MPL_AUX_NA_HPP_INCLUDED
