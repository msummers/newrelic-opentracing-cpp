#include "tracer_factory.h"
#include "tracer.h"
#include "newrelic/opentracing.h"
#include <iostream>
#include <unistd.h>

namespace newrelic {
    newrelic_app_t* newrelicApp;

    opentracing::expected<std::shared_ptr<opentracing::Tracer>> TracerFactory::MakeTracer(const char *configuration, std::string &error_message) const noexcept {
        std::cerr << "(" << this <<  ") TracerFactory::MakeTracer " << std::endl;
        return static_cast<const std::shared_ptr<opentracing::Tracer> &>(std::shared_ptr<newrelic::Tracer>(new Tracer(configuration)));
    }

    TracerFactory::TracerFactory() {
        std::cerr << "(" << this << ") TracerFactory::TracerFactory" << std::endl;
        const std::string Prefix{"nginx://"};
        std::string Hostname{"unknown"};
        char hostname[1024];
        auto success = gethostname(hostname, sizeof(hostname));
        if (success == 0){
            Hostname = hostname;
        }
        std::string appName{Prefix + Hostname};
        // TODO Get appname and license key from config file
        newrelic_app_config_t *config = newrelic_create_app_config(appName.c_str(), "d9253857ae9e25c333205bc9d5b00126d80f8c49");

        config->distributed_tracing.enabled = true;
        if (!newrelic_configure_log("./c_sdk.log", NEWRELIC_LOG_INFO)) {
            std::cerr << "<" << this << ") FATAL: Error configuring New Relic daemon logging" << std::endl;
            std::terminate();
        }

        if (!newrelic_init(NULL, 0)) {
            std::cerr << "<" << this << ") FATAL: Error connecting to New Relic daemon" << std::endl;
            std::terminate();
        }

        /* Wait up to 10 seconds for the SDK to connect to the daemon */
        newrelicApp = newrelic_create_app(config, 10000);
        newrelic_destroy_app_config(&config);
        std::cerr << "(" << this << ") TracerFactory::TracerFactory newrelicApp: " << newrelicApp << std::endl;
    }

    TracerFactory::~TracerFactory() {
        std::cerr << "(" << this << ") TracerFactory::~TracerFactory" << std::endl;
        newrelic_destroy_app(&newrelicApp);
        std::cerr << "(" << this << ") TracerFactory::~TracerFactory: New Relic App destroyed" << std::endl;
    }
}
