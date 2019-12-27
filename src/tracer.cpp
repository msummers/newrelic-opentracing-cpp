#include <iostream>
#include "tracer.h"
#include "span.h"

namespace newrelic {
    Tracer::Tracer(){
        std::cerr << "Tracer::Tracer " << this << std::endl;
        txn = newrelic_start_web_transaction(app, "Nginx Transaction");
        seg = newrelic_start_segment(txn, "Nginx Segment", "Custom");
    }

    Tracer::Tracer(const char *configuration) {
        std::cerr << "Tracer::Tracer(config) " << this << std::endl;
    }

    std::unique_ptr<opentracing::Span> Tracer::StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept {
        std::cerr << "Tracer::StartSpanWithOptions operation_name: " << operation_name << " options: " << &options << " " << this << std::endl;
        return std::unique_ptr<newrelic::Span>(new Span(this, operation_name, options));
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, std::ostream &writer) const {
        std::cerr << "Tracer::Inject " << this << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const {
        std::cerr << "Tracer::Inject " << this << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const {
        std::cerr << "Tracer::Inject " << this << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(std::istream &reader) const {
        std::cerr << "Tracer::Extract(istream) " << this << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::TextMapReader &reader) const {
        std::cerr << "Tracer::Extract(textMapReader) " << this << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::HTTPHeadersReader &reader) const {
        std::cerr << "Tracer::Extract(HTTPHeaderReader) " << this << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    void Tracer::Close() noexcept {
        std::cerr << "Tracer::Close " << this << std::endl;
        Tracer::Close();
    }
}