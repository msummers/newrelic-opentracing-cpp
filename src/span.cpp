#include <iostream>
#include "span.h"
#include "config.h"

namespace newrelic {
    const std::string Span::DummySpan{"dummySpan"};
    const std::string SpanContext::ContextKey{"newrelic"};

    Span::Span(const Tracer *tracer, const opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) {
        std::cerr << "(" << this << ") Span::Span name: " << operation_name.data() << " options:" << options.tags.size() << std::endl;
        newrelicTracer = tracer;
        if (DummySpan == operation_name.data()) {
        } else {
            for (auto p : options.tags) {
                std::cerr << "(" << this << ") Span::Span name: " << operation_name.data() << " options key: " << p.first << " value: " << typeid(p.second).name() << std::endl;
            }
            this->newrelicSpanContext.span = this;
            // Root/non-root span
            auto parentContext = findSpanContext(options.references);
            if (parentContext != nullptr) {
                std::cerr << "(" << this << ") Span::Span child span" << std::endl;
                newrelicTxn = parentContext->span->newrelicTxn;
                this->newrelicSpanContext.isRoot = false;
            } else {
                newrelicTxn = newrelic_start_web_transaction(newrelicApp, operation_name.data());
                std::cerr << "(" << this << ") Span::Span root span" << std::endl;
            }
            std::cerr << "(" << this << ") Span::Span newrelicTxn: " << newrelicTxn << std::endl;

            std::string segmentName{operation_name.data()};
            std::replace(segmentName.begin(), segmentName.end(), '/', ' ');
            newrelicSegment = newrelic_start_segment(newrelicTxn, segmentName.c_str(), Config::getSegmentCategory().c_str());
            std::cerr << "(" << this << ") Span::Span newrelicSegment: " << newrelicSegment << " " << std::endl;

            auto payload = newrelic_create_distributed_trace_payload(newrelicTxn, newrelicSegment);
            this->newrelicSpanContext.ContextValue = payload;
            free(payload);
            std::cerr << "(" << this << ") Span::Span contextValue: " << this->newrelicSpanContext.ContextValue << " " << std::endl;
        }
    }

    Span::~Span() {
        std::cerr << "(" << this << ") Span::~Span" << std::endl;
    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {
        std::cerr << "(" << this << ") Span::FinishWithOptions " << std::endl;

        std::cerr << "(" << this << ") Span::FinishWithOptions newrelicTxn: " << &(newrelicTxn) << " " << std::endl;
        if (newrelicSegment != nullptr) {
            std::cerr << "(" << this << ") Span::FinishWithOptions newrelicSegment: " << &(newrelicSegment) << " " << std::endl;
            newrelic_end_segment(newrelicTxn, &newrelicSegment);
        }
        std::cerr << "(" << this << ") Span::FinishWithOptions segment ended " << std::endl;
        if (this->newrelicSpanContext.isRoot) {
            newrelic_end_transaction(&(newrelicTxn));
            std::cerr << "(" << this << ") Span::FinishWithOptions transaction ended " << std::endl;
        }
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
        std::cerr << "(" << this << ") Span::context " << &newrelicSpanContext << std::endl;
        return newrelicSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        std::cerr << "(" << this << ") Span::tracer " << std::endl;
        return *newrelicTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {
        std::cerr << "(" << this << ") Span::ForeachBaggageItem " << std::endl;
    }

    SpanContext::~SpanContext() {
        std::cerr << "(" << this << ") SpanContext::~SpanContext " << std::endl;
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
