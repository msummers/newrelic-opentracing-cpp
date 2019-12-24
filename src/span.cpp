//
// Created by Mike on 12/24/19.
//

#include "span.h"
namespace newrelic{
    void Span::Log(opentracing::SystemTime timestamp, const std::vector<std::pair<opentracing::string_view, opentracing::Value>> &fields) noexcept {
    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {

    }

    void Span::SetOperationName(opentracing::string_view name) noexcept {

    }

    void Span::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept {

    }

    void Span::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept {

    }

    std::string Span::BaggageItem(opentracing::string_view restricted_key) const noexcept {
        return std::string();
    }

    void Span::Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {

    }

    void Span::Log(opentracing::SystemTime timestamp, std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {

    }

    const opentracing::SpanContext &Span::context() const noexcept {
        return aSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        return aTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {

    }

    std::unique_ptr<opentracing::SpanContext> SpanContext::Clone() const noexcept {
        return std::unique_ptr<SpanContext>();
    }
}
