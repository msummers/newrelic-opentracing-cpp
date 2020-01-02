//
// Created by Mike on 12/22/19.
//

#ifndef NR_OPENTRACING_CPP_TRACER_FACTORY_H
#define NR_OPENTRACING_CPP_TRACER_FACTORY_H

#include <opentracing/tracer_factory.h>

namespace newrelic {
    class TracerFactory : public opentracing::TracerFactory {
    public:
        // Source: OpenTracing API for C++
        TracerFactory();

        virtual ~TracerFactory();

        // Source: OpenTracing API for C++
        opentracing::expected<std::shared_ptr<opentracing::Tracer>> MakeTracer(const char *configuration, std::string &error_message) const noexcept override;
    };
}
#endif
