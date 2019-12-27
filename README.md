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