#include <iostream>
#include "span.h"
#include "config.h"

namespace newrelic {
    const std::string Span::DummySpan{"dummySpan"};
    const std::string SpanContext::ContextKey{"newrelic"};

    Span::Span(const Tracer *tracer, const opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) {
        Log::trace("({}) Span::Span name: {}", (void *)this, operation_name.data());
        newrelicTracer = tracer;
        if (DummySpan == operation_name.data()) {
        } else {
            for (auto p : options.tags) {
                Log::debug("({}) Span::Span options key: {} value: {}", (void *)this, p.first.data(), typeid(p.second).name());
            }
            this->newrelicSpanContext.span = this;
            // Root/non-root span
            Log::debug("({}) Span::Span options.references.size(): {}", (void*)this, options.references.size());
            auto parentContext = findSpanContext(options.references);
            if (parentContext != nullptr) {
                Log::debug("({}) Span::Span is child span", (void *)this);
                newrelicTxn = parentContext->span->newrelicTxn;
                this->newrelicSpanContext.isRoot = false;
            } else {
                Log::debug("({}) Span::Span is root span", (void *)this);
                newrelicTxn = newrelic_start_web_transaction(newrelicApp, operation_name.data());
                //Add inbound payload if any
                //auto payload = tracer->getAndClearPayload();
                if (payload != ""){
                    newrelic_accept_distributed_trace_payload(newrelicTxn, payload.c_str(), NEWRELIC_TRANSPORT_TYPE_HTTP );
                    Log::debug("({}) Span::Span inbound payload: {}", (void *)this, payload);
                }
            }
            Log::debug("({}) Span::Span newrelicTxn: {}", (void *)this, (void *)newrelicTxn);

            std::string segmentName{operation_name.data()};
            std::replace(segmentName.begin(), segmentName.end(), '/', ' ');
            newrelicSegment = newrelic_start_segment(newrelicTxn, segmentName.c_str(), Config::getSegmentCategory().c_str());
            Log::debug("({}) Span::Span newrelicSegment: {}", (void *)this, (void *)newrelicSegment);

            auto payload = newrelic_create_distributed_trace_payload(newrelicTxn, newrelicSegment);
            this->newrelicSpanContext.ContextValue = payload;
            free(payload);
            Log::debug("({}) Span::Span contextValue: {}", (void *)this, this->newrelicSpanContext.ContextValue);
        }
    }

    Span::~Span() {
        Log::trace("({}) Span::~Span", (void *)this);
    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {
        Log::trace("({}) Span::FinishWithOptions", (void *)this);

        Log::debug("({}) Span::FinishWithOptions", (void *)this);
        if (newrelicSegment != nullptr) {
            Log::debug("({}) Span::FinishWithOptions ending segment: {}", (void *)this, (void*)newrelicSegment);
            newrelic_end_segment(newrelicTxn, &newrelicSegment);
        }
        if (this->newrelicSpanContext.isRoot) {
            Log::debug("({}) Span::FinishWithOptions ending transaction: {}", (void *)this, (void*)newrelicTxn);
            newrelic_end_transaction(&(newrelicTxn));
        }
    }

    void Span::SetOperationName(opentracing::string_view name) noexcept {
        Log::trace("({}) Span::SetOperationName name: {}", (void *)this, name.data());
    }

    void Span::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept {
        Log::trace("({}) Span::SetTag key: {}", (void *)this, key.data());
    }

    void Span::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept {
        Log::trace("({}) Span::SetBaggageItem key: {}", (void *)this, restricted_key.data());
    }

    std::string Span::BaggageItem(opentracing::string_view restricted_key) const noexcept {
        Log::trace("({}) Span::BaggageItem key: {}", (void *)this, restricted_key.data());
        return std::string();
    }

    void Span::Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
        Log::trace("({}) Span::Log", (void *)this);
    }

    const opentracing::SpanContext &Span::context() const noexcept {
        Log::trace("({}) Span::context", (void *)this);
        return newrelicSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        Log::trace("({}) Span::tracer", (void *)this);
        return *newrelicTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {
        Log::trace("({}) SpanContext::ForeachBaggageItem", (void *)this);
    }

    SpanContext::~SpanContext() {
        Log::trace("({}) SpanContext::~SpanContext", (void *)this);
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
