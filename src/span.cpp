#include <iostream>
#include "span.h"

namespace newrelic {
    const  std::string Span::DummySpan{"dummySpan"};
    const  std::string SpanContext::ContextKey{"newrelic"};

    Span::Span(const Tracer *tracer, const opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) {
        std::cerr << "(" << this << ") Span::Span name: " << operation_name << " " << std::endl;
        newrelicTracer = const_cast<Tracer *>(tracer);
        if (DummySpan == operation_name.data()) {
        }else{
            newrelicSegment = newrelic_start_segment(tracer->newrelicTxn, operation_name.data(), "Custom");
            std::cerr << "(" << this << ") Span::Span newrelicSegment: " << newrelicSegment << " " << std::endl;
            this->newrelicSpanContext.ContextValue = newrelic_create_distributed_trace_payload(tracer->newrelicTxn, newrelicSegment);
            std::cerr << "(" << this << ") Span::Span contextValue: " << this->newrelicSpanContext.ContextValue << " " << std::endl;
        }
    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {
        std::cerr << "(" << this << ") Span::FinishWithOptions " << std::endl;

        std::cerr << "(" << this << ") Span::FinishWithOptions newrelicTxn: " << &(newrelicTracer->newrelicTxn) << " " << std::endl;
        if (newrelicSegment != nullptr) {
            std::cerr << "(" << this << ") Span::FinishWithOptions newrelicSegment: " << &(newrelicSegment) << " " << std::endl;
            newrelic_end_segment(newrelicTracer->newrelicTxn, &newrelicSegment);
        }
        std::cerr << "(" << this << ") Span::FinishWithOptions segment ended " << std::endl;
        newrelic_end_transaction(&(newrelicTracer->newrelicTxn));
        std::cerr << "(" << this << ") Span::FinishWithOptions transaction ended " << std::endl;
    }

    void Span::SetOperationName(opentracing::string_view name) noexcept {
        std::cerr << "(" << this << ") Span::SetOperationName name: " << name << " " << std::endl;
    }

    void Span::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept {
        std::cerr << "(" << this << ") Span::SetTag key: " << key << " value: " << &value << " " << std::endl;
    }

    void Span::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept {
        std::cerr << "(" << this << ") Span::SetBaggageItem " << std::endl;
    }

    std::string Span::BaggageItem(opentracing::string_view restricted_key) const noexcept {
        std::cerr << "(" << this << ") Span::BaggageItem " << std::endl;
        return std::string();
    }

    void Span::Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
        std::cerr << "(" << this << ") Span::Log " << std::endl;
    }

    const opentracing::SpanContext &Span::context() const noexcept {
        std::cerr << "(" << this << ") Span::context " << std::endl;
        return newrelicSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        std::cerr << "(" << this << ") Span::tracer " << std::endl;
        return *newrelicTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {
        std::cerr << "(" << this << ") Span::ForeachBaggageItem " << std::endl;
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
