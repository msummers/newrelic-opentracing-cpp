//
// Created by Mike on 12/24/19.
//

#ifndef NR_OPENTRACING_CPP_SPAN_H
#define NR_OPENTRACING_CPP_SPAN_H

#include <opentracing/span.h>
#include <map>
#include "tracer.h"
#include "newrelic/opentracing.h"
#include "log.h"

namespace newrelic {
    class Span;

    class SpanContext : public opentracing::SpanContext, public std::enable_shared_from_this<SpanContext> {
    public:
        // Source: OpenTracing API for C++
        void ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const override;

        // TODO #ifdef ABI v3
        //std::unique_ptr<opentracing::SpanContext> Clone() const noexcept override;
        // #endif
        const static std::string ContextKey;
        std::string ContextValue{"{}"};
        newrelic::Span *span{};
        bool isRoot{false};
        bool isUsed{false};
        std::string payload{""};
    };

    class Span : public opentracing::Span, public std::enable_shared_from_this<Span> {
    public:
        // Source: OpenTracing API for C++
        ~Span() override;

        Span(const Tracer *tracer, opentracing::string_view operation_name, const opentracing::StartSpanOptions &options);

        // Source: OpenTracing API for C++
        void FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept override;

        // Source: OpenTracing API for C++
        void SetOperationName(opentracing::string_view name) noexcept override;

        // Source: OpenTracing API for C++
        void SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept override;

        // Source: OpenTracing API for C++
        void SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept override;

        // Source: OpenTracing API for C++
        std::string BaggageItem(opentracing::string_view restricted_key) const noexcept override;

        // Source: OpenTracing API for C++
        void Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept override;

        // Source: OpenTracing API for C++
        const opentracing::SpanContext &context() const noexcept override;

        // Source: OpenTracing API for C++
        const opentracing::Tracer &tracer() const noexcept override;

        // TODO #ifdef ABI v3
        // Source: OpenTracing API for C++
        //void Log(opentracing::SystemTime timestamp, const std::vector<std::pair<opentracing::string_view, opentracing::Value>> &fields) noexcept override;
        // Source: OpenTracing API for C++
        //void Log(opentracing::SystemTime timestamp, std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept override;
        // #endif

        newrelic_txn_t *newrelicTxn;
        newrelic_segment_t *newrelicSegment = nullptr;
    private:
        const Tracer *newrelicTracer;
        newrelic::SpanContext newrelicSpanContext{};
        const static std::string DummySpan;
        std::map<std::string, std::string> tags;
    };

    static newrelic::SpanContext *findSpanContext(const std::vector<std::pair<opentracing::SpanReferenceType, const opentracing::SpanContext *>> &references) {
        for (auto &reference : references) {
            Log::debug("findSpanContext: first: {} second: {}", typeid(reference.first).name(), typeid(reference.second).name());
            try {
                Log::debug("findSpanContext: exit with found context");
                return dynamic_cast<SpanContext *>(const_cast<opentracing::SpanContext*>(reference.second));
            } catch (...) {
                auto e = std::current_exception();
                Log::error("Exception {} casting type {} to newrelic::SpanContext", e.__cxa_exception_type()->name(), typeid(reference.second).name());
            }
        }
        Log::debug("findSpanContext: exit with nullptr");
        return nullptr;
    }
}
#endif //NR_OPENTRACING_CPP_SPAN_H
