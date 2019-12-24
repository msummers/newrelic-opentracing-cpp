#include "tracer.h"

namespace newrelic {
    Tracer::Tracer(const char *configuration) {
    }

    std::unique_ptr<opentracing::Span> Tracer::StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept {
        return std::unique_ptr<opentracing::Span>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, std::ostream &writer) const {
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const {
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const {
        return opentracing::expected<void>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(std::istream &reader) const {
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::TextMapReader &reader) const {
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::HTTPHeadersReader &reader) const {
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    void Tracer::Close() noexcept {
        Tracer::Close();
    }
}