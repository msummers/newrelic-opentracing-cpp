#include <iostream>
#include "tracer.h"
#include "span.h"

namespace newrelic {
    Tracer::Tracer(){
        std::cerr << "Tracer::Tracer" << std::endl;
    }
    Tracer::Tracer(const char *configuration) {
        std::cerr << "Tracer::Tracer(config)" << std::endl;
    }

    std::unique_ptr<opentracing::Span> Tracer::StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept {
        std::cerr << "Tracer::StartSpanWithOptions" << std::endl;
        return std::unique_ptr<newrelic::Span>(new Span(operation_name, options));
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, std::ostream &writer) const {
        std::cerr << "Tracer::Inject" << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const {
        std::cerr << "Tracer::Inject" << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const {
        std::cerr << "Tracer::Inject" << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(std::istream &reader) const {
        std::cerr << "Tracer::Extract" << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::TextMapReader &reader) const {
        std::cerr << "Tracer::Extract" << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::HTTPHeadersReader &reader) const {
        std::cerr << "Tracer::Extract" << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    void Tracer::Close() noexcept {
        std::cerr << "Tracer::Close" << std::endl;
        Tracer::Close();
    }
}