#include <iostream>
#include "tracer.h"
#include "span.h"

namespace newrelic {
//    Tracer::Tracer(){
//        std::cerr << "Tracer::Tracer " << std::endl;
//        newrelicTxn = newrelic_start_web_transaction(app, "Nginx Transaction");
//        std::cerr << "Tracer::Tracer newrelicTxn: " <<  newrelicTxn << " " << std::endl;
//        newrelicSegment = newrelic_start_segment(newrelicTxn, "Nginx Segment", "Custom");
//        std::cerr << "Tracer::Tracer newrelicSegment: " <<  newrelicSegment << " " << std::endl;
//    }

    Tracer::Tracer(const char *configuration) {
        std::cerr << "(" << this << ") Tracer::Tracer(config)" << std::endl;
        newrelicTxn = newrelic_start_web_transaction(app, "Nginx Transaction");
        std::cerr << "(" << this << ") Tracer::Tracer(config) newrelicTxn: " << newrelicTxn << std::endl;
    }

    std::unique_ptr<opentracing::Span> Tracer::StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept {
        std::cerr << "(" << this << ") Tracer::StartSpanWithOptions operation_name: " << operation_name << " options: " << &options << " " << std::endl;
        return std::unique_ptr<newrelic::Span>(new Span(this, operation_name, options));
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, std::ostream &writer) const {
        std::cerr << "(" << this << ") Tracer::Inject1 " << std::endl;
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const {
        std::cerr << "(" << this << ") Tracer::Inject2 " << std::endl;
        return opentracing::expected<void>();
    }

    // On start-up this method is called with a dummy span whose context must return a context with all of the keys we use!
    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const {
        std::cerr << "(" << this << ") Tracer::Inject3 " << std::endl;
        auto& nr_sc = (SpanContext &) sc;
        return writer.Set(newrelic::SpanContext::ContextKey, nr_sc.ContextValue);
        //return opentracing::expected<void>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(std::istream &reader) const {
        std::cerr << "(" << this << ") Tracer::Extract(istream) " << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::TextMapReader &reader) const {
        std::cerr << "(" << this << ") Tracer::Extract(textMapReader) " << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::HTTPHeadersReader &reader) const {
        std::cerr << "(" << this << ") Tracer::Extract(HTTPHeaderReader) " << std::endl;
        return opentracing::expected<std::unique_ptr<opentracing::SpanContext>>();
    }

    void Tracer::Close() noexcept {
        std::cerr << "(" << this << ") Tracer::Close " << std::endl;
        Tracer::Close();
    }
}