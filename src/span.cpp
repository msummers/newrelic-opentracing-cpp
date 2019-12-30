#include <iostream>
#include "span.h"

namespace newrelic {
    Span::Span(const Tracer *tracer, opentracing::string_view operation_name, const opentracing::StartSpanOptions &options){
        std::cerr << "Span::Span name: " << operation_name << " " << this << std::endl;
        aTracer = const_cast<Tracer *>(tracer);
    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {
        std::cerr << "Span::FinishWithOptions " << this << std::endl;

        std::cerr << "Span::FinishWithOptions txn: " << &(aTracer-> txn) << " " << this << std::endl;
        std::cerr << "Span::FinishWithOptions seg: " << &(aTracer-> seg) << " " << this << std::endl;
        auto segPtr = aTracer -> seg;
        newrelic_end_segment(aTracer->txn, &segPtr);
        std::cerr << "Span::FinishWithOptions segment ended "  << std::endl;
        newrelic_end_transaction(&(aTracer->txn));
        std::cerr << "Span::FinishWithOptions transaction ended "  << std::endl;
    }

    void Span::SetOperationName(opentracing::string_view name) noexcept {
        std::cerr << "Span::SetOperationName name: " << name << " " << this << std::endl;
    }

    void Span::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept {
        std::cerr << "Span::SetTag key: " << key << " value: " << &value << " " << this << std::endl;
    }

    void Span::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept {
        std::cerr << "Span::SetBaggageItem " << this << std::endl;
    }

    std::string Span::BaggageItem(opentracing::string_view restricted_key) const noexcept {
        std::cerr << "Span::BaggageItem " << this << std::endl;
        return std::string();
    }

    void Span::Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
        std::cerr << "Span::Log " << this << std::endl;
    }

    const opentracing::SpanContext &Span::context() const noexcept {
        std::cerr << "Span::context " << this << std::endl;
        return aSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        std::cerr << "Span::tracer " << this << std::endl;
        return *aTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {
        std::cerr << "Span::ForeachBaggageItem " << this << std::endl;
    }

    // TODO  #ifdef ABI v3
    //    void Span::Log(opentracing::SystemTime timestamp, const std::vector<std::pair<opentracing::string_view, opentracing::Value>> &fields) noexcept {
    //    }

    //    void Span::Log(opentracing::SystemTime timestamp, std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
    //    }

    //    std::unique_ptr<opentracing::SpanContext> SpanContext::Clone() const noexcept {
    //        return std::unique_ptr<SpanContext>();
    //    }
    // #endif
}
