#include "tracer_factory.h"
#include "tracer.h"
#include "newrelic/opentracing.h"
#include <iostream>

namespace newrelic {
    newrelic_app_t* app ;

    opentracing::expected<std::shared_ptr<opentracing::Tracer>> TracerFactory::MakeTracer(const char *configuration, std::string &error_message) const noexcept {
        std::cerr << "TracerFactory::MakeTracer " << this << std::endl;
        return static_cast<const std::shared_ptr<opentracing::Tracer> &>(std::shared_ptr<newrelic::Tracer>(new Tracer(configuration)));
    }

    TracerFactory::TracerFactory() {
        std::cerr << "TracerFactory::TracerFactory " << this << std::endl;
        newrelic_app_config_t* config = newrelic_create_app_config("Nginx", "d9253857ae9e25c333205bc9d5b00126d80f8c49");
        config->distributed_tracing.enabled = true;
        if (!newrelic_configure_log("./c_sdk.log", NEWRELIC_LOG_INFO)) {
            std::cerr << "Error configuring New Relic daemon logging" << std::endl;
            std::terminate();
        }

        if (!newrelic_init(NULL, 0)) {
            std::cerr << "Error connecting to New Relic daemon" << std::endl;
            std::terminate();
        }

        /* Wait up to 10 seconds for the SDK to connect to the daemon */
        app = newrelic_create_app(config, 10000);
        newrelic_destroy_app_config(&config);
        std::cerr << "TracerFactory::TracerFactory: create New Relic app config " << this << std::endl;
    }
}
