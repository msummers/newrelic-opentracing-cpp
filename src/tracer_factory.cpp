#include "tracer_factory.h"
#include "tracer.h"
#include  <newrelic/libnewrelic.h>
#include <iostream>

namespace newrelic {
    opentracing::expected<std::shared_ptr<opentracing::Tracer>> TracerFactory::MakeTracer(const char *configuration, std::string &error_message) const noexcept {
        std::cerr << "TracerFactory::MakeTracer" << std::endl;
        return static_cast<const std::shared_ptr<opentracing::Tracer> &>(std::shared_ptr<newrelic::Tracer>(new Tracer(configuration)));
    }

    TracerFactory::TracerFactory() {
        std::cerr << "TracerFactory::TracerFactory" << std::endl;
        //newrelic_app_config_t* config = newrelic_create_app_config("", "");
    }
}
