//
// Created by Mike on 12/23/19.
//

#ifndef NR_OPENTRACING_CPP_TRACER_H
#define NR_OPENTRACING_CPP_TRACER_H

#include <functional>
#include <random>
#include <opentracing/tracer.h>
#include <opentracing/expected/expected.hpp>


namespace newrelic {

    class SpanBuffer;

    // The interface for providing IDs to spans and traces.
    typedef std::function<uint64_t()> IdProvider;

    uint64_t getId();

    class Tracer : public opentracing::Tracer, public std::enable_shared_from_this<Tracer> {
    public:
        // Creates a Tracer by copying the given options and injecting the given dependencies.
        //Tracer(TracerOptions options, std::shared_ptr<SpanBuffer> buffer, TimeProvider get_time, IdProvider get_id, std::shared_ptr<SampleProvider> sampler);

        // Creates a Tracer by copying the given options and using the preconfigured writer.
        // The writer is either an AgentWriter that sends trace data directly to the Datadog Agent, or
        // an ExternalWriter that requires an external HTTP client to encode and submit to the Datadog
        // Agent.
        //Tracer(TracerOptions options, std::shared_ptr<Writer> &writer, std::shared_ptr<SampleProvider> sampler);

        Tracer() = delete;

        Tracer(const char *configuration);

        // Starts a new span.
        std::unique_ptr<opentracing::Span> StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept override;

        // Ensure we aren't hiding methods from opentracing::Tracer due to the overloaded overrides.
        using opentracing::Tracer::Extract;
        using opentracing::Tracer::Inject;

        opentracing::expected<void> Inject(const opentracing::SpanContext &sc, std::ostream &writer) const override;

        opentracing::expected<void> Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const override;

        opentracing::expected<void> Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const override;

        opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(std::istream &reader) const override;

        opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(const opentracing::TextMapReader &reader) const override;

        opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(const opentracing::HTTPHeadersReader &reader) const override;

        void Close() noexcept override;

    private:
//        const TracerOptions opts_;
//        // Keeps finished spans until their entire trace is finished.
//        std::shared_ptr<SpanBuffer> buffer_;
//        TimeProvider get_time_;
//        IdProvider get_id_;
//        std::shared_ptr<SampleProvider> sampler_;
    };

}
#endif //NR_OPENTRACING_CPP_TRACER_H
