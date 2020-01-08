## nr-opentracing-cpp
New Relic C++ OpenTracing implementation for [Nginx OpenTracing](https://github.com/opentracing-contrib/nginx-opentracing)

### Notes
- Nginx version must match the Nginx OpenTracing Release version _exactly_
- [C++ OpenTracing Library](https://github.com/opentracing/opentracing-cpp) ABI version must match the Nginx OpenTracing implementation's version

### Installation
### Configuration
#### nginx
#### New Relic Tracer

### Build
- `mkdir .build`
- `cd .build`
- `cmake ..`
- `make`

### To do
#### Near term
- ~~Move txn start to Span::Span~~
- ~~Config via file~~
- ~~Implement StringUtils::toLower and use it in Config::init~~
- ~~Allow comments in newrelic.conf~~
- Ensure no C-SDK memory leaks
- Logging ([spdlog](https://github.com/gabime/spdlog))
- Generate OpenTelemetry Context from config setting
- Get context for parent span from `Tracer::Extract` if present
#### Long term
- Modify C SDK to allow setting Span as External after it is created
  - Use this set Spans as External if `Tracer::Inject` is called
- Inject span id into logs
- Unit tests
  - [doctest](https://github.com/onqtam/doctest)
  - [Google Test](https://github.com/google/googletest)
  - [Catch2](https://github.com/catchorg/Catch2)
- Dockerize
- CMakeLists.txt for C-SDK
- CMakeLists.text meta make (Nginx module against Nginx source, C++ API, C-SDK, Tracer)
