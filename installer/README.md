# Sample plugin installer

Installing the plugin is tricky, the following works for a fresh Ubuntu 18.04 LTS setup.

- Get the version of the [NGINX OpenTracing Module](https://github.com/opentracing-contrib/nginx-opentracing/releases) that corresponds to your version of NGINX and place it in `downloaded`
- Read through `./install.sh` so you know what it's going to do!
- Copy *your* `nginx.conf` to the `downloaded` sub-directory
- Add the comment bracketed sections from `downloaded/nginx.conf.sample` to *your* copied `downloaded/nginx.conf`
   - You'll need at least the first two sections, setting up the module and the config
- Edit `downloaded/nr-opentracing-config.conf`, set the APM License Key and the Application Name
- Run `sudo ./install.sh`
- Take note if there are any `FAIL: ` messages. If there are it's *strongly* recommended you start over with a clean instance.
- On Ubuntu 18.04 LTS it should _just work_
