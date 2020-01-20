#include <iostream>
#include <sstream>
#include "span.h"
#include "config.h"
//#include <nlohmann/json.hpp>

//using json = nlohmann::json;

namespace newrelic {
    const std::string Span::DummySpan{"dummySpan"};
    const std::string SpanContext::ContextKey{"newrelic"};

    Span::Span(const Tracer *tracer, const opentracing::string_view operation_name, const opentracing::StartSpanOptions &options) {
        // operation_name.data is NOT a null terminated C string, this makes it safe
        std::string operationName = operation_name;
        Log::trace("({}) Span::Span enter. operationName: {}", (void *) this, operationName);
        newrelicTracer = tracer;
        // Skip if we're the startup dummy span
        if (DummySpan == operationName) {
        } else {
            for (const auto &p : options.tags) {
                Log::debug("({}) Span::Span options key: {} value: {}", (void *) this, p.first.data(), typeid(p.second).name());
            }
            // Root/non-root span
            Log::debug("({}) Span::Span options.references.size(): {}", (void *) this, options.references.size());
            auto referenceContext = findSpanContext(options.references);
            // Process Root Span
            if (referenceContext->isRoot && !referenceContext->isUsed) {
                referenceContext->isUsed = true;
                // DIRE WARNING don't set anything in the SpanContext until *AFTER* the assignment!
                this->newrelicSpanContext = *referenceContext;
                this->newrelicSpanContext.span = this;
                // TODO is Config::skipTransaction useful? If so what does it do?
                auto txnName = Config::filterTransaction(operationName);
                Log::debug("({}) Span::Span is root span. Transaction name: {}", (void *) this, txnName);
                newrelicTxn = newrelic_start_web_transaction(newrelicApp, txnName.c_str());
                //Add inbound payload if any
                if (!this->newrelicSpanContext.payload.empty()) {
                    newrelic_accept_distributed_trace_payload_httpsafe(newrelicTxn, this->newrelicSpanContext.payload.c_str(), NEWRELIC_TRANSPORT_TYPE_HTTP);
                    Log::debug("({}) Span::Span inbound payload: {}", (void *) this, this->newrelicSpanContext.payload);
                }
                // Process child spans
            } else if (referenceContext->isRoot && referenceContext->isUsed) {
                this->newrelicSpanContext.span = this;
                Log::debug("({}) Span::Span is child span", (void *) this);
                newrelicTxn = referenceContext->span->newrelicTxn;
            } else {
                Log::warn("({}) Span: referenceContext->isRoot: {} referenceContext->isUsed: {}", (void *) this, referenceContext->isRoot, referenceContext->isUsed);
            }
            Log::debug("({}) Span::Span newrelicTxn: {}", (void *) this, (void *) newrelicTxn);

            // Apply the same Transaction name filter to the segment name
            std::string segmentName{Config::filterTransaction(operationName)};
            std::replace(segmentName.begin(), segmentName.end(), '/', ' ');
            newrelicSegment = newrelic_start_segment(newrelicTxn, segmentName.c_str(), Config::getSegmentCategory().c_str());
            Log::debug("({}) Span::Span newrelicSegment: {}", (void *) this, (void *) newrelicSegment);

            // TODO move defer payload generation to Tracer::Inject?
            auto payload = newrelic_create_distributed_trace_payload(newrelicTxn, newrelicSegment);
            this->newrelicSpanContext.ContextValue = payload;
            free(payload);
            Log::debug("({}) Span::Span contextValue: {}", (void *) this, this->newrelicSpanContext.ContextValue);
        }
    }

    Span::~Span() {
        Log::trace("({}) Span::~Span", (void *) this);
    }

    void Span::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept {
        Log::trace("({}) Span::FinishWithOptions", (void *) this);

        // Convert the Span associated with the Context to External. This requires a C-SDK change.
        if ( this->newrelicTxn != nullptr && this->newrelicSegment != nullptr) {
            std::string uri{tags["http.url"]};
            if(!Config::skipTransaction(uri)) {
                uri = Config::filterTransaction(uri);
                Log::debug("({}) Span::FinishWithOptions set segment external", (const void *) this);
                std::string procedure{tags["http.method"]};
                std::string library;
                newrelic_external_segment_params_t params = {const_cast<char *>(uri.c_str()), const_cast<char *>(procedure.c_str()), const_cast<char *>(library.c_str())};
                newrelic_set_segment_external(this->newrelicTxn, this->newrelicSegment, &params);
            }
        }

        if (newrelicSegment != nullptr) {
            Log::debug("({}) Span::FinishWithOptions ending segment: {}", (void *) this, (void *) newrelicSegment);
            newrelic_end_segment(newrelicTxn, &newrelicSegment);
        }
        if (this->newrelicSpanContext.isRoot) {
            Log::debug("({}) Span::FinishWithOptions ending transaction: {}", (void *) this, (void *) newrelicTxn);
            newrelic_end_transaction(&(newrelicTxn));
        }
    }

    void Span::SetOperationName(opentracing::string_view name) noexcept {
        Log::trace("({}) Span::SetOperationName name: {}", (void *) this, name.data());
    }

    namespace {
        // Visits and serializes an arbitrarily-nested variant type. Serialisation of value types is to
        // string while any composite types are expressed in JSON. eg. string("fred") -> "fred"
        // vector<string>{"felicity"} -> "[\"felicity\"]"
        struct VariantVisitor {
            // Populated with the final result.
            std::string &result;

            explicit VariantVisitor(std::string &result_) : result(result_) {}

        private:
            // Only set if VariantVisitor is recursing. Unfortunately we only really need an explicit
            // distinction (and all the conditionals below) to avoid the case of a simple string being
            // serialized to "\"string\"" - which is valid JSON but very silly never-the-less.
            //json *json_result = nullptr;
            std::string *json_result = nullptr;
            //            VariantVisitor(std::string &result_, json *json_result_)
            //                    : result(result_), json_result(json_result_) {}

        public:
            void operator()(bool value) const {
                if (json_result != nullptr) {
                    *json_result = value;
                } else {
                    result = value ? "true" : "false";
                }
            }

            void operator()(double value) const {
                if (json_result != nullptr) {
                    *json_result = value;
                } else {
                    result = std::to_string(value);
                }
            }

            void operator()(int64_t value) const {
                if (json_result != nullptr) {
                    *json_result = value;
                } else {
                    result = std::to_string(value);
                }
            }

            void operator()(uint64_t value) const {
                if (json_result != nullptr) {
                    *json_result = value;
                } else {
                    result = std::to_string(value);
                }
            }

            void operator()(const std::string &value) const {
                if (json_result != nullptr) {
                    *json_result = value;
                } else {
                    result = value;
                }
            }

            void operator()(std::nullptr_t) const {
                if (json_result != nullptr) {
                    *json_result = "nullptr";
                } else {
                    result = "nullptr";
                }
            }

            void operator()(const char *value) const {
                if (json_result != nullptr) {
                    *json_result = value;
                } else {
                    result = std::string(value);
                }
            }

            void operator()(const std::vector<opentracing::Value> &values) const {
                result = "Vector unsupported";
            }

            //            void operator()(const std::vector<opentracing::Value> &values) const {
            //                json list;
            //                for (auto value : values) {
            //                    json inner;
            //                    std::string r;
            //                    apply_visitor(VariantVisitor{r, &inner}, value);
            //                    list.push_back(inner);
            //                }
            //                if (json_result != nullptr) {
            //                    // We're a list in a dict/list.
            //                    *json_result = list;
            //                } else {
            //                    // We're a root object, so dump the string.
            //                    result = list.dump();
            //                }
            //            }
            //
            void operator()(const std::unordered_map<std::string, opentracing::Value> &value) const {
                result = "Map unsupported";
            }
            //            void operator()(const std::unordered_map<std::string, opentracing::Value> &value) const {
            //                json dict;
            //                for (auto pair : value) {
            //                    json inner;
            //                    std::string r;
            //                    apply_visitor(VariantVisitor{r, &inner}, pair.second);
            //                    dict[pair.first] = inner;
            //                }
            //                if (json_result != nullptr) {
            //                    // We're a dict in a dict/list.
            //                    *json_result = dict;
            //                } else {
            //                    // We're a root object, so dump the string.
            //                    result = dict.dump();
            //                }
            //            }
        };
    }  // namespace

    void Span::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept {
        std::string result;
        apply_visitor(VariantVisitor{result}, value);
        auto safeKey = std::string{key};
        tags[safeKey] = result;
        if(this->newrelicSegment != nullptr && this->newrelicTxn != nullptr) {
            newrelic_add_attribute_string(this->newrelicTxn, safeKey.c_str(), result.c_str());
        }
        Log::trace("({}) Span::SetTag key: {} value: {}", (void *) this, safeKey, result);
    }

    void Span::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept {
        Log::trace("({}) Span::SetBaggageItem key: {}", (void *) this, restricted_key.data());
    }

    std::string Span::BaggageItem(opentracing::string_view restricted_key) const noexcept {
        Log::trace("({}) Span::BaggageItem key: {}", (void *) this, restricted_key.data());
        return std::string();
    }

    void Span::Log(std::initializer_list<std::pair<opentracing::string_view, opentracing::Value>> fields) noexcept {
        Log::trace("({}) Span::Log", (void *) this);
    }

    const opentracing::SpanContext &Span::context() const noexcept {
        Log::trace("({}) Span::context", (void *) this);
        return newrelicSpanContext;
    }

    const opentracing::Tracer &Span::tracer() const noexcept {
        Log::trace("({}) Span::tracer", (void *) this);
        return *newrelicTracer;
    }

    void SpanContext::ForeachBaggageItem(std::function<bool(const std::string &key, const std::string &value)> f) const {
        Log::trace("({}) SpanContext::ForeachBaggageItem", (void *) this);
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
