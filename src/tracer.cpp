#include <iostream>
#include "tracer.h"
#include "span.h"
#include "config.h"

namespace newrelic {
    Tracer::Tracer(const char *configuration) {
        std::cerr << "(" << this << ") Tracer::Tracer(config)" << std::endl;
    }

    std::unique_ptr<opentracing::Span> Tracer::StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept {
        std::cerr << "(" << this << ") Tracer::StartSpanWithOptions operation_name: " << operation_name << " options: " << &options << std::endl;
        auto span = std::unique_ptr<newrelic::Span>(new Span(this, operation_name, options));
        std::cerr << "(" << this << ") Tracer::StartSpanWithOptions span: " << &span << std::endl;
        return span;
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, std::ostream &writer) const {
        std::cerr << "(" << this << ") Tracer::Inject(ostream) " << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const {
        std::cerr << "(" << this << ") Tracer::Inject(TextMapWriter) " << std::endl;
        return opentracing::expected<void>();
    }

    // On start-up this method is called with a dummy span whose context must return a context with all of the keys we use!
    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const {
        // TODO Convert the Span associated with the Context to External. This requires a C-SDK change.
        if (Config::isExternal()){
        }

        std::cerr << "(" << this << ") Tracer::Inject(HTTPHeadersWriter) spanContext: " << &sc << std::endl;
        auto &nr_sc = (SpanContext &) sc;
        return writer.Set(newrelic::SpanContext::ContextKey, nr_sc.ContextValue);
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(std::istream &reader) const {
        std::cerr << "(" << this << ") Tracer::Extract(istream) " << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::TextMapReader &reader) const {
        std::cerr << "(" << this << ") Tracer::Extract(textMapReader) " << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    // TODO Get the `newrelic` payload for the nginx Root Span from here if present. (That is, we're in the middle.)
    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::HTTPHeadersReader &reader) const {
        std::cerr << "(" << this << ") Tracer::Extract(HTTPHeaderReader) " << std::endl;
        reader.ForeachKey([&](opentracing::string_view key, opentracing::string_view value) -> opentracing::expected<void> {
            std::cerr << "(" << this << ") Tracer::Extract(HTTPHeaderReader) key: " << key << " value: " << value << std::endl;
            return {};
        });
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    void Tracer::Close() noexcept {
        std::cerr << "(" << this << ") Tracer::Close " << std::endl;
        Tracer::Close();
    }

    Tracer::~Tracer() {
        std::cerr << "(" << this << ") Tracer::~Tracer " << std::endl;
    }
}