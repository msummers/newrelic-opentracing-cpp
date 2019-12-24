//
// Created by Mike on 12/24/19.
//

#ifndef NR_OPENTRACING_CPP_SPAN_H
#define NR_OPENTRACING_CPP_SPAN_H

#include <opentracing/span.h>
#include "tracer.h"

namespace newrelic {
    class SpanContext : public opentracing::SpanContext, public std::enable_shared_from_this<SpanContext> {
    public:
        void ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const override;

        std::unique_ptr<opentracing::SpanContext> Clone() const noexcept override;
    };

    class Span : public opentracing::Span, public std::enable_shared_from_this<Span> {
    public:
        ~Span() override = default;

        void Log(opentracing::SystemTime timestamp, const std::vector<std::pair<opentracing::string_view, opentracing::Value>> &fields) noexcept override;

        void FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept override;

        void SetOperationName(opentracing::string_view name) noexcept override;

        void SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept override;

        void SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept override;

        std::string BaggageItem(opentracing::string_view restricted_key) const noexcept override;

        void Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept override;

        void Log(opentracing::SystemTime timestamp, std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept override;

        const opentracing::SpanContext &context() const noexcept override;

        const opentracing::Tracer &tracer() const noexcept override;

    private:
        newrelic::Tracer aTracer;
        newrelic::SpanContext aSpanContext;
    };
}

#endif //NR_OPENTRACING_CPP_SPAN_H
