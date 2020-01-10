#include <iostream>
#include "tracer.h"
#include "span.h"
#include "config.h"
#include "string_utils.h"

namespace newrelic {
    Tracer::Tracer(const char *configuration) {
        Log::trace("({}) Tracer::Tracer config: {}", (const void*)this, configuration);
    }

    std::unique_ptr<opentracing::Span> Tracer::StartSpanWithOptions(opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) const noexcept {
        Log::trace("({}) Tracer::StartSpanWithOptions operation_name: {} options:{}", (const void*)this, operation_name.data(), (const void*) &options);
        auto span = std::unique_ptr<newrelic::Span>(new Span(this, operation_name, options));
        Log::debug("({}) Tracer::StartSpanWithOptions span:{}", (const void*)this, (const void*) &span);
        return span;
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, std::ostream &writer) const {
        Log::trace("({}) Tracer::Inject(ostream)", (const void*)this);
        return opentracing::expected<void>();
    }

    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::TextMapWriter &writer) const {
        Log::trace("({}) Tracer::Inject(TextMapWriter)", (const void*)this);
        return opentracing::expected<void>();
    }

    // On start-up this method is called with a dummy span whose context must return a context with all of the keys we use!
    opentracing::expected<void> Tracer::Inject(const opentracing::SpanContext &sc, const opentracing::HTTPHeadersWriter &writer) const {
        // TODO Convert the Span associated with the Context to External. This requires a C-SDK change.
        Log::trace("({}) Tracer::Inject(HTTPHeaderWriter) spanContext: {}", (const void*)this, (const void*)&sc);
        auto &nr_sc = (SpanContext &) sc;
        return writer.Set(newrelic::SpanContext::ContextKey, nr_sc.ContextValue);
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(std::istream &reader) const {
        Log::trace("({}) Tracer::Extract(istream)", (const void*)this);
        return std::unique_ptr<opentracing::SpanContext>{};
    }

    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::TextMapReader &reader) const {
        Log::trace("({}) Tracer::Extract(TextMapReader)", (const void*)this);
        return std::unique_ptr<opentracing::SpanContext>{};
    }

    // A Span is the root span if gets this context. It may, or may not, get a payload from here.
    opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Tracer::Extract(const opentracing::HTTPHeadersReader &reader) const {
        Log::trace("({}) Tracer::Extract(HTTPHeaderReader)", (const void*)this);
        auto context = newrelic::SpanContext();
        reader.ForeachKey([&](opentracing::string_view key, opentracing::string_view value) -> opentracing::expected<void> {
            Log::debug("({}) Tracer::Extract(HTTPHeaderReader) key: {} value: {}", (const void*)this, key.data(), value.data());
            if(StringUtils::toLower(key.data()) == "newrelic"){
                context.payload = value;
                context.isRoot = true;
            }
            return {};
        });
        return std::unique_ptr<opentracing::SpanContext>{&context};
        //return std::unique_ptr<opentracing::SpanContext>{};
    }

    void Tracer::Close() noexcept {
        Log::trace("({}) Tracer::Close", (const void*)this);
        Tracer::Close();
    }

    Tracer::~Tracer() {
        Log::trace("({}) Tracer::~Tracer", (const void*)this);
    }

//    std::string Tracer::getAndClearPayload()const{
//        std::lock_guard<std::mutex> lock(const_cast<Tracer*>(this)->payloadMutex);
//        auto result = this->payload;
//        const_cast<Tracer*>(this)->payload = "";
//        return result;
//    }
//
//    std::string Tracer::getPayload()const {
//        std::lock_guard<std::mutex> lock(const_cast<Tracer*>(this)->payloadMutex);
//        return this->payload;
//    }
//
//    void Tracer::setPayload(std::string value){
//        std::lock_guard<std::mutex> lock(payloadMutex);
//        this->payload = value;
//    }
}