//
// Created by Mike on 12/24/19.
//

#include <iostream>
#include "span.h"
namespace newrelic{
    Span::Span(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options){
        std::cerr << "Span::Span" << std::endl;
    }

//    void Span::Log(opentracing::SystemTime timestamp, const std::vector<std::pair<opentracing::string_view, opentracing::Value>> &fields) noexcept {
//    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {
        std::cerr << "Span::FinishWithOptions" << std::endl;
    }

    void Span::SetOperationName(opentracing::string_view name) noexcept {
        std::cerr << "Span::SetOperationName" << std::endl;

    }

    void Span::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept {
        std::cerr << "Span::SetTag" << std::endl;

    }

    void Span::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept {
        std::cerr << "Span::SetBaggageItem" << std::endl;

    }

    std::string Span::BaggageItem(opentracing::string_view restricted_key) const noexcept {
        std::cerr << "Span::BaggageItem" << std::endl;
        return std::string();
    }

    void Span::Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
        std::cerr << "Span::Log" << std::endl;
    }

//    void Span::Log(opentracing::SystemTime timestamp, std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
//
//    }

    const opentracing::SpanContext &Span::context() const noexcept {
        std::cerr << "Span::context" << std::endl;
        return aSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        std::cerr << "Span::tracer" << std::endl;
        return aTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {
        std::cerr << "Span::ForeachBaggageItem" << std::endl;
    }

//    std::unique_ptr<opentracing::SpanContext> SpanContext::Clone() const noexcept {
//        return std::unique_ptr<SpanContext>();
//    }
}
