## nr-opentracing-cpp
New Relic C++ OpenTracing Tracer implementation for [Nginx OpenTracing](https://github.com/opentracing-contrib/nginx-opentracing)

### Installation
1. Nginx must be built with `--with-compat` and the Nginx version must match the Nginx OpenTracing module version _exactly_
   - Use `nginx -V` to determine the Nginx version and build settings
2. If Nginx is built with `--with-compat` _and_ there is a [pre-built Nginx OpenTracing Release](https://github.com/opentracing-contrib/nginx-opentracing/releases) that matches the Nginx version _exactly_ download the module and go to Step 3
   - If your version of Nginx is not built with `--with-compat` you either have to find one for your Linux distribution that is or [build Nginx from source](https://www.google.com/search?q=building+nginx+from+source&oq=building+nginx+from+source&aqs=chrome..69i57.4695j0j7&sourceid=chrome&ie=UTF-8) 
   - If there is no pre-built Nginx OpenTracing Release that matches your Nginx version you have to [build your own Nginx OpenTracing module](https://github.com/opentracing-contrib/nginx-opentracing#building-from-source)
3. Copy `ngx_http_opentracing_module.so` to the Nginx module directory
   - On Ubuntu for instance, this is `/usr/lib/nginx/modules/`
4. [Download the latest version of the New Relic Nginx OpenTracing Tracer](https://github.com/msummers/newrelic-opentracing-cpp/releases) `libnr_opentracing.so`

### Configuration
#### nginx
- `nginx.conf`
```
#
# Standard nginx.conf
#
load_module modules/ngx_http_opentracing_module.so;

user  nginx;
worker_processes  1;
error_log  /var/log/nginx/error.log debug;
pid        /var/run/nginx.pid;
working_directory  /tmp/;
worker_rlimit_core 500M;

events {
    worker_connections  1024;
}

http {
	opentracing_load_tracer <ABSOLUTE_PATH_TO>/libnr_opentracing.so  <ABSOLUTE_PATH_TO>/newrelic.conf
	opentracing on;
	opentracing_operation_name $uri;
	opentracing_propagate_context;

    include       /etc/nginx/mime.types;
    default_type  application/octet-stream;
    log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                      '$status $body_bytes_sent "$http_referer" '
                      '"$http_user_agent" "$http_x_forwarded_for"';
    access_log  /var/log/nginx/access.log  main;
    sendfile        on;
    keepalive_timeout  65;
    log_subrequest on;

    upstream backend {
      server <YOUR_BACKEND_HOST:PORT_HERE>
    }
    include /etc/nginx/conf.d/*.conf;
}
```
- `tracer.conf`
```
# This goes in Nginx's conf.d directory
server {
        listen 80;
        listen [::]:80;

        server_name <YOUR_SERVER_NAME_HERE>;

        location / {
	         # Full list of directives here https://github.com/opentracing-contrib/nginx-opentracing/blob/master/doc/Reference.md
             proxy_pass http://backend/;
        }
}
```
#### New Relic Tracer
- `newrelic.conf`
```
# New Relic Tracer configuration file
#
newrelicLicense: <YOUR_LICENCE_KEY_HERE> # No default
applicationName:                         # Default: hostname-nginx
logLevel: INFO                           # Default: INFO ()DEBUG | INFO | WARNING | ERROR)
segmentCategory: nginx                   # Default: nginx
csdkLogLocation: ./c_sdk.log             # C-SDK log location. Default: ./c_sdk.log
csdkLogLevel: NEWRELIC_LOG_INFO          # C-SDK log level (DEBUG | INFO | WARNING | ERROR). Default: INFO
```
### Trouble shooting

### Build the Tracer from Source
- `git clone`
- `cd` into clone directory
- `mkdir .build`
- `cd .build`
- `cmake ..`
- `make`
#### Build Notes
- Nginx version must match the Nginx OpenTracing Release version _exactly_
- [C++ OpenTracing Library](https://github.com/opentracing/opentracing-cpp) ABI version must match the Nginx OpenTracing implementation's version


### To do
#### Near term
- ~~Move txn start to Span::Span~~
- ~~Config via file~~
  - ~~Ignore case~~
- ~~Implement StringUtils::toLower and use it in Config::init~~
- ~~Allow comments in newrelic.conf~~
- Ensure no C-SDK memory leaks
- ~~Logging~~
- Generate OpenTelemetry Context from config setting
- ~~Get context for parent span from `Tracer::Extract` if present in the HTTP header~~
#### Long term
- Modify C SDK to allow setting Span as External after it is created
  - Use this to set Span as External if `Tracer::Inject` is called
- Inject span id into logs
- Unit tests
  - [doctest](https://github.com/onqtam/doctest)
  - [Google Test](https://github.com/google/googletest)
  - [Catch2](https://github.com/catchorg/Catch2)
- Dockerize
- CMakeLists.txt for C-SDK
- CMakeLists.text meta make (Nginx module against Nginx source, C++ API, C-SDK, Tracer)
