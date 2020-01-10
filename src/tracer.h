//
// Created by Mike on 12/23/19.
//

#ifndef NR_OPENTRACING_CPP_TRACER_H
#define NR_OPENTRACING_CPP_TRACER_H

#include <functional>
#include "newrelic/opentracing.h"
#include <random>
#include <opentracing/tracer.h>
#include <opentracing/expected/expected.hpp>
#include <mutex>


namespace newrelic {
    class Tracer : public opentracing::Tracer, public std::enable_shared_from_this<Tracer> {
    public:
        // Source: OpenTracing API for C++
        Tracer() = delete;

        Tracer(const char *configuration);

        virtual ~Tracer();

        // Source: OpenTracing API for C++
        std::unique_ptr<opentracing::Span> StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept override;

        // Ensure we aren't hiding methods from opentracing::Tracer due to the overloaded overrides.
        using opentracing::Tracer::Extract;
        using opentracing::Tracer::Inject;

        // Source: OpenTracing API for C++
        opentracing::expected<void> Inject(const opentracing::SpanContext &sc, std::ostream &writer) const override;

        // Source: OpenTracing API for C++
        opentracing::expected<void> Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const override;

        // Source: OpenTracing API for C++
        opentracing::expected<void> Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const override;

        // Source: OpenTracing API for C++
        opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(std::istream &reader) const override;

        // Source: OpenTracing API for C++
        opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(const opentracing::TextMapReader &reader) const override;

        // Source: OpenTracing API for C++
        opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(const opentracing::HTTPHeadersReader &reader) const override;

        // Source: OpenTracing API for C++
        void Close() noexcept override;

//        std::string getPayload() const;
//        std::string getAndClearPayload() const;

    private:
//        std::string payload {""};
//        void setPayload(std::string value);
//        std::mutex payloadMutex;
    };
}
#endif //NR_OPENTRACING_CPP_TRACER_H
