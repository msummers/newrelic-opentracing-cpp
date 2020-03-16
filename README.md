## nr-opentracing-cpp
New Relic C++ OpenTracing Tracer implementation for [Nginx OpenTracing](https://github.com/opentracing-contrib/nginx-opentracing)

_Note: this tracer utilizes the New Relic C-SDK which is an Agent and requires an Agent license._

### Installation
1. Nginx must be built with `--with-compat` and the Nginx version must match the Nginx OpenTracing module version _exactly_
   - Use `nginx -V` to determine the Nginx version and build settings
2. If Nginx is built with `--with-compat` _and_ there is a [pre-built Nginx OpenTracing Release](https://github.com/opentracing-contrib/nginx-opentracing/releases) that matches the Nginx version _exactly_ download the module and go to Step 3
   - If your version of Nginx is not built with `--with-compat` you either have to find one for your Linux distribution that is or [build Nginx from source](https://www.google.com/search?q=building+nginx+from+source&oq=building+nginx+from+source&aqs=chrome..69i57.4695j0j7&sourceid=chrome&ie=UTF-8) 
   - If there is no pre-built Nginx OpenTracing Release that matches your Nginx version you have to [build your own Nginx OpenTracing module](https://github.com/opentracing-contrib/nginx-opentracing#building-from-source)
3. Place `ngx_http_opentracing_module.so` in the Nginx module directory
   - On Ubuntu for instance, this is `/usr/lib/nginx/modules/`
4. [Download the latest version of the New Relic Nginx OpenTracing Tracer](https://github.com/msummers/newrelic-opentracing-cpp/releases) `libnr_opentracing.so`, `libopentracing.so`, and `newrelic-daemon`
5. Place the downloaded libs anywhere you like
   - Update the linker cache with `sudo ldconfig <full_path_to_libopentracing.so>`
   - Verify `libnr_opentracing.so` with `ldd <full_path>/libnr_opentracing.so` you should see something similar to 
``` 
    linux-vdso.so.1 (0x00007fff7efc3000)
   	libopentracing.so.1 => /usr/local/lib/libopentracing.so.1 (0x00007f0805d5c000)
   	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f08059d3000)
   	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f0805635000)
   	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f080541d000)
   	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f080502c000)
   	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f0804e28000)
   	/lib64/ld-linux-x86-64.so.2 (0x00007f080633d000)
```
   which shows that ld can dynamically resolve `libopentracing` 

6. C-SDK Daemon
   - Either add the C-SDK Daemon to the Nginx init script or create a new init script to start the Daemon. It does not matter where the Daemon is installed, it is a standalone executable.

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
transactionFilterFile: /etc/newrelic/nginxTransactionFilters.txt # C++ regex filter file for NR Transaction naming
```
- `nginxTransactionFilters.txt` File of raw (unescaped) C++ regex patterns applied to URLs to help minimize Transaction name explosion
```
#
# Nginx Tracer transaction filter file
# regex:<regex> replace:<replacement>
# regex:<regex> skip:
regex:\/(*[0-9]+) replace:/{id} # This pattern replaces numeric ids preceeded by a forward slash with the string `{id}`
```
### Trouble shooting
- `sudo systemctl status nginx`
- `tail -f /var/log/nginx/error.log`
- `sudo apport-retrace  -s /var/crash/<crash-file` (Note: you may have to add `Package: 0` to the crash file)
  - `cmake -DCMAKE_BUILD_TYPE=Debug ..` to get more info in the crash

### Build the Tracer from Source
- clone and install [OpenTracing CPP v1.5.1](https://github.com/opentracing/opentracing-cpp)
- clone and install [fmt](https://github.com/fmtlib/fmt.git)
- `git clone`
- `cd` into clone directory
- `mkdir .build`
- `cd .build`
- `cmake ..`
- `make`
#### Installing an exact version of Nginx on Ubuntu
1. [Setup apt to install the official packages fron Nginx](https://www.nginx.com/resources/wiki/start/topics/tutorials/install/#official-debian-ubuntu-packages)
2. List the available versions of Nginx
```
apt policy nginx
```
3. Pin the package by like this in `/etc/apt/preferences.d/nginx `
```
Package: nginx
Pin: version 1.17.3-1~bionic
Pin-Priority: 1001
```
4. Now a normal apt-get install
```
sudo apt-get install nginx
```

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
- ~~Add Tags to Transaction~~
- ~~Set Transaction as External~~
- ~~Regex filter Transaction names~~
  - Default (no filter file) to first path segment
- Prefix Transaction _target_ host to Transaction Name.
  - Make this an option whose default is on.
- What should Skip Transaction do?
- Standalone C++ regex tester
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
