#include "tracer_factory.h"
#include "tracer.h"
#include "newrelic/opentracing.h"
#include "config.h"
#include "log.h"
#include <iostream>

namespace newrelic {
    newrelic_app_t* newrelicApp;

    opentracing::expected<std::shared_ptr<opentracing::Tracer>> TracerFactory::MakeTracer(const char *configuration, std::string &error_message) const noexcept {
        Config::init(configuration);
        Log::setLevel(Config::getLogLevel());
        Log::trace("({}) TracerFactory::MakeTracer",  (const void*)this);
        Log::info("({}) TracerFactory::MakeTracer logLevel: {}",  (const void*) this, Config::getLogLevel());

        newrelic_app_config_t *config = newrelic_create_app_config(Config::getApplicationName().c_str(), Config::getLicense().c_str());

        config->distributed_tracing.enabled = true;
        if (!newrelic_configure_log(Config::getCSDKLogLocation().c_str(),  Config::getCSDKLogLevel())) {
            Log::fatal("({}) TracerFactory::MakeTracer Error configuring New Relic daemon logging", (const void*)this);
        }

        if (!newrelic_init(NULL, 0)) {
            Log::fatal("({}) TracerFactory::MakeTracer Error connecting to New Relic daemon", (const void *)this);
        }

        /* Wait up to 10 seconds for the SDK to connect to the daemon */
        newrelicApp = newrelic_create_app(config, 10000);
        newrelic_destroy_app_config(&config);
        Log::debug("({}) TracerFactory::MakeTracer newrelicApp: {}", (const void *)this, (const void*) newrelicApp);
        return static_cast<const std::shared_ptr<opentracing::Tracer> &>(std::shared_ptr<newrelic::Tracer>(new Tracer(configuration)));
    }

    TracerFactory::TracerFactory() {
        Log::trace("({}) TracerFactory::TraceFactory",  (const void*)this);
    }

    TracerFactory::~TracerFactory() {
        Log::trace("({}) TracerFactory::~TraceFactory",  (const void*)this);
        newrelic_destroy_app(&newrelicApp);
        Log::debug("({}) TracerFactory::~TraceFactory newrelicApp destroyed",  (const void*)this);
    }
}
