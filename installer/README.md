1. Download the zip file if you don't already have it
2. Unzip the zip file
3. Get the version of the [NGINX OpenTracing Module](https://github.com/opentracing-contrib/nginx-opentracing/releases) that corresponds to your version of NGINX and place it in `downloaded`
4. Read through `./install.sh` so you know what it's going to do!
5. Copy *your* `nginx.conf` to the `downloaded` sub-directory
6. Add the comment bracketed sections from `downloaded/nginx.conf.sample` to *your* copied `downloaded/nginx.conf`
   - You'll need at least the first two sections, setting up the module and the config
7. Edit `downloaded/nr-opentracing-config.conf`, set the APM License Key and the Application Name
8. Run `sudo ./install.sh`
9. Take note if there are any `FAIL: ` messages. If there are it's *strongly* recommended you start over with a clean instance.
10. On Ubuntu 18.04 LTS it should _just work_
