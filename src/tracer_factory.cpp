#include "tracer_factory.h"

namespace newrelic {
    opentracing::expected<std::shared_ptr<opentracing::Tracer>> TracerFactory::MakeTracer(const char *configuration, std::string &error_message) const noexcept {
        return opentracing::expected<std::shared_ptr<opentracing::Tracer>>();
    }
}
