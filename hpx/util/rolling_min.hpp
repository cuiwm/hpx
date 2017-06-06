//  Copyright (c) 2017 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ROLLING_MIN_HPP
#define ROLLING_MIN_HPP

#include <hpx/config.hpp>

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/numeric/functional.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/statistics/rolling_window.hpp>

namespace hpx { namespace util
{
    namespace detail
    {
        template <typename Sample>
        struct rolling_min_impl : boost::accumulators::accumulator_base
        {
            typedef Sample float_type;

            // for boost::result_of
            typedef float_type result_type;

            template <typename Args>
            rolling_min_impl(Args const& args)
              : min_(boost::numeric::as_max(
                    args[boost::accumulators::sample | Sample()]))
            {}

            template <typename Args>
            void operator()(Args const &args)
            {
                if (boost::accumulators::impl::is_rolling_window_plus1_full(args))
                {
                    auto r = boost::accumulators::rolling_window_plus1(args);

                    this->min_ = boost::numeric::as_max(
                        args[boost::accumulators::sample]);

                    auto it = r.begin();
                    for (++it; it != r.end(); ++it)
                    {
                        boost::numeric::min_assign(this->min_, *it);
                    }
                }
                else
                {
                    boost::numeric::min_assign(
                        this->min_, args[boost::accumulators::sample]);
                }
            }

            template <typename Args>
            result_type result(Args const &args) const
            {
                return this->min_;
            }

        private:
            Sample min_;
        };
    }
}}

///////////////////////////////////////////////////////////////////////////////
// tag::rolling_min
namespace boost { namespace accumulators
{
    namespace tag
    {
        struct rolling_min
          : depends_on<rolling_window_plus1>
        {
            struct impl
            {
                template <typename Sample, typename Weight>
                struct apply
                {
                    typedef hpx::util::detail::rolling_min_impl<Sample> type;
                };
            };
        };
    }

    ///////////////////////////////////////////////////////////////////////////////
    // extract::rolling_min
    namespace extract
    {
        extractor<tag::rolling_min> const rolling_min = {};
    }
}} // namespace boost::accumulators

namespace hpx { namespace util
{
    namespace tag
    {
        using boost::accumulators::tag::rolling_min;
    }

    using boost::accumulators::extract::rolling_min;
}}

#endif

