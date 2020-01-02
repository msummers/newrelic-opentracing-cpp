## nr-opentracing-cpp
New Relic C++ OpenTracing implementation for [Nginx OpenTracing](https://github.com/opentracing-contrib/nginx-opentracing)

### Notes
- Nginx version must match the Nginx OpenTracing Release version _exactly_
- [C++ OpenTracing Library](https://github.com/opentracing/opentracing-cpp) ABI version must match the Nginx OpenTracing implementation's version

### nginx configuration

### New Relic configuration

### Build
- `mkdir .build`
- `cd .build`
- `cmake ..`
- `make`

### To do
#### Near term
- Move txn start to Span::Span
- Ensure no C-SDK memory leaks
- Config via file
- Logging ([spdlog](https://github.com/gabime/spdlog))
- OpenTelemetry Context via config
- Load test
#### Long term
- Unit tests
  - [doctest](https://github.com/onqtam/doctest)
  - [Google Test](https://github.com/google/googletest)
  - [Catch2](https://github.com/catchorg/Catch2)
- Dockerize
- CMakeLists.txt for C-SDK
- CMakeLists.text meta make (Nginx module against Nginx source, C++ API, C-SDK, Tracer)
