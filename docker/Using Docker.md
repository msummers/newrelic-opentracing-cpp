1. Start Docker
2. Start the NGINX container: `docker run -d nginx:1.17.3`
3. Some commands will require the Container ID rather than the Image Name:
```
sudo docker ps
CONTAINER ID        IMAGE               COMMAND                  CREATED             STATUS              PORTS               NAMES
7b86dffd5246        nginx:1.17.3        "nginx -g 'daemon of…"   2 hours ago         Up 2 hours          80/tcp              nostalgic_tereshkova
```
4. Ensure NGINX is built `--with-compat`: `sudo docker run -it nginx:1.17.3 nginx -V | grep compat`
5. Get location of NGINX module directory: `sudo docker run -it nginx:1.17.3 find / -name modules -type d`
	/usr/lib/nginx/module
6. Copy ngx_http_opentracing_module.so to the module directory: `sudo docker cp ngx_http_opentracing_module.so 7b86dffd5246:/usr/lib/nginx/modules`
7. Check the result:
```
sudo docker exec -t -i 7b86dffd5246 ls -l /usr/lib/nginx/modules
total 3616
-rw-r--r-- 1 root root      24328 Aug 13  2019 ngx_http_geoip_module-debug.so
-rw-r--r-- 1 root root      24328 Aug 13  2019 ngx_http_geoip_module.so
-rw-r--r-- 1 root root      27568 Aug 13  2019 ngx_http_image_filter_module-debug.so
-rw-r--r-- 1 root root      27568 Aug 13  2019 ngx_http_image_filter_module.so
-rw-r--r-- 1 root root     462464 Aug 15  2019 ngx_http_js_module-debug.so
-rw-r--r-- 1 root root     458368 Aug 15  2019 ngx_http_js_module.so
-rwxr-xr-x 1  501 dialout 1675984 Aug 20  2019 ngx_http_opentracing_module.so
-rw-r--r-- 1 root root      23400 Aug 13  2019 ngx_http_xslt_filter_module-debug.so
-rw-r--r-- 1 root root      23400 Aug 13  2019 ngx_http_xslt_filter_module.so
-rw-r--r-- 1 root root      19864 Aug 13  2019 ngx_stream_geoip_module-debug.so
-rw-r--r-- 1 root root      19864 Aug 13  2019 ngx_stream_geoip_module.so
-rw-r--r-- 1 root root     453504 Aug 15  2019 ngx_stream_js_module-debug.so
-rw-r--r-- 1 root root     449408 Aug 15  2019 ngx_stream_js_module.so
```

8. Get location of nginx.conf: `sudo docker run -it nginx:1.17.3 find / -name nginx.conf`
	/etc/nginx/nginx.conf
9. Copy your modified nginx.conf to the container: `sudo docker cp nginx.conf 7b86dffd5246:/etc/nginx`
10. If needed  copy a config file into NGINX's conf.d directory: `sudo docker cp default.conf 7b86dffd5246:/etc/nginx/conf.d`
11. Copy `libnr_opentracing.so` to the Container using the location specied in `nginx.con`- `	opentracing_load_tracer /etc/newrelic/lib/libnr_opentracing.so  /etc/newrelic/nr-opentracing-config.conf;`
  - 

cat nginx.conf

libopentracing.so
newrelic-daemon

NOTES:
1. `run` works on an Image, `exec` works on a Container
  - `sudo docker run  -it <Image>     <Bash command>
  - `sudo docker exec -it <Container> <Bash command>
