#!/usr/bin/env bash
#
# An opinionated script that sets-up a NGINX Docker Image with the New Relic Open Tracing Tracer.
#
# Use: ./InstallInDocker.sh <NGINX_Image_ID> <downloaded_directory>
#      NGINX_Image_ID:       the Docker Image ID that includes your NGINX
#      downloaded_directory: the directory where the files downloaded from the GHE Release are located. Default is  ./downloaded
#
# What's meant by "opinionated"?
# All files other than ngx_http_opentracing_module.so go in /etc/newrelic. This means your conf files have to be written to use /etc/newrelic.
# If you don't like this you have to change this script, it's not configurable.
#
# Required files
# libnr_opentracing.so             copied to /etc/newrelic in the image
# libopentracing.so                copied to /etc/newrelic in the image
# newrelic-daemon                  copied to /etc/newrelic in the image
# nginx.conf                       copied to the NGINX conf dir in the image
# nginxTransactionFilters.txt      copied to /etc/newrelic in the image
# ngx_http_opentracing_module.so   copied to the NGINX modules dir in the image
# nr-opentracing-config.conf       copied to /etc/newrelic in the image
#
# Optional files:
# conf.d/* If downloaded_directory contains a sub directory of conf.d all files in conf.d are copied to NGINX's conf.d directory

function finish() {
  if [ $? -eq 0 ]; then
    echo "Success"
  else
    echo "Failure"
  fi
  #sudo docker stop ${containerID}
  sudo docker logs ${containerID}
  #sudo docker commit ${containerID} ${imageID}-crash
  #docker run -it my-broken-container /bin/bash
  sudo docker container kill ${containerID}
  # This cleans out any Image whose name is <none>
  docker images | grep \<none\> | awk '{print $3}' | xargs -I '{}' sudo docker image rm -f '{}'
  echo "INFO: Finish"
}
trap finish EXIT

# Check for imageID on command line
if [ -z ${1+x} ]; then
  echo "FATAL: ImageID required as first (and only) parameter. Exiting"
  exit 1
else
  imageID=$1
  echo "INFO : ImageID: ${imageID}"
fi
echo ""

# Check for downloaded dir
if [ -z ${2+x} ]; then
  echo "INFO : using default downloaded directory ./downloaded"
  downloadedDir="./downloaded"
else
  downloadedDir=$2
fi
if [[ -d "${downloadedDir}" ]]; then
  echo "INFO : Downloaded directory: ${downloadedDir}"
else
  echo "FATAL: Downloaded directory: ${downloadedDir} not found."
  exit 1
fi
echo ""

# TODO verify we have everything we need (manifest)

# Start the Image
docker run -d ${imageID}

echo "INFO: This container's OS"
sudo docker run -it ${imageID} uname -a
sudo docker run -it ${imageID} cat /etc/os-release
echo ""

# Ensure NGINX was built with --with-compat
if sudo docker run -it ${imageID} nginx -V | grep -q with-compat; then
  echo "INFO : NGINX built with --with-compat"
else
  echo "FATAL: NGINX is not built with --with-compat, failing"
  exit 1
fi
echo ""

# Get the Container ID
containerID=$(sudo docker ps --format "{{.ID}}")
if [ -z ${containerID+x} ]; then
  echo "FATAL: ContainerID not found. Exiting"
  exit 1
else
  echo "INFO : ContinerID: ${containerID}"
fi
echo ""

# Get location of NGINX module directory
# DIRE WARNING: Watch for stray \r when capturing the output of docker commands as variables!
modulesDir=$(sudo docker run -it ${imageID} find / -name modules -type d)
if [ -z ${modulesDir+x} ]; then
  echo "FATAL: NGINX modules directory not found. Exiting"
  exit 1
else
  # At least on a Mac there's a trailing \r
  modulesDir=${modulesDir%?}
  echo "INFO : NGINX Modules Dir: ${modulesDir}"
fi
echo ""

# Copy ngx_http_opentracing_module.so to the module directory
echo "sudo docker cp ${downloadedDir}/ngx_http_opentracing_module.so ${containerID}:${modulesDir}"
sudo docker cp ${downloadedDir}/ngx_http_opentracing_module.so ${containerID}:${modulesDir}

# Check the result:
echo "INFO: ls -l ${modulesDir}/"
sudo docker exec -t -i ${containerID} ls -l ${modulesDir}/
echo ""

# Get location of nginx.conf
nginxConfDir=$(sudo docker run -it ${imageID} find / -name nginx.conf)
nginxConfDir=${nginxConfDir%?}
if [ -z ${nginxConfDir+x} ]; then
  echo "FATAL: NGINX configuration directory not found. Exiting"
  exit 1
else
  echo "INFO : NGINX Config Dir: ${nginxConfDir}"
fi
echo ""

# Copy your modified nginx.conf to the container: sudo docker cp nginx.conf 7b86dffd5246:/etc/nginx
echo "sudo docker cp ${downloadedDir}/nginx.conf ${containerID}:${nginxConfDir}"
sudo docker cp ${downloadedDir}/nginx.conf ${containerID}:${nginxConfDir}

# Show the result
echo "INFO : ls -l ${nginxConfDir}"
sudo docker exec -t -i ${containerID} ls -l ${nginxConfDir}
echo ""

# If needed  copy  config files into NGINX's conf.d directory
if [[ -d "${downloadedDir}/conf.d" ]]; then
  sudo docker cp ${downloadedDir}/conf.d/* ${containerID}:${nginxConfDir}/conf.d
fi

# Setup /etc/newrelic
newrelicDir=$(sudo docker run -it ${imageID} find / -name newrelic -type d)
newrelicDir=${newrelicDir%?}
if [ -z ${newrelicDir} ]; then
  echo "INFO: creating /etc/newrelic"
  sudo docker exec -t -i ${containerID} mkdir -p /etc/newrelic
else
  echo "INFO: /etc/newrelic already exists: ${newrelicDir}"
fi
sudo docker exec -t -i ${containerID} ls -ld /etc/newrelic
echo ""

# Copy non-libs to our directory
#sudo docker cp ${downloadedDir}/newrelic-daemon ${containerID}:/etc/newrelic/
sudo docker exec -t -i ${containerID} chmod +x /etc/newrelic/newrelic-daemon
sudo docker cp ${downloadedDir}/nginxTransactionFilters.txt ${containerID}:/etc/newrelic/
sudo docker cp ${downloadedDir}/nr-opentracing-config.conf ${containerID}:/etc/newrelic/
#sudo docker cp ${downloadedDir}/ContainerStartup.sh ${containerID}:/etc/newrelic/
#sudo docker exec -t -i ${containerID} chmod +x /etc/newrelic/ContainerStartup.sh
# Verify the copy
echo "INFO: ls -l /etc/newrelic/"
sudo docker exec -t -i ${containerID} ls -l /etc/newrelic/
echo ""

# Copy libs to /usr/lib/newrelic, something about docker commit causes the cache to be wiped and libs not in the trusted path are lost
libDir="/usr/local/lib"
#sudo docker exec -t -i ${containerID} mkdir  /usr/lib/newrelic
sudo docker cp ${downloadedDir}/libnr_opentracing.so ${containerID}:${libDir}
sudo docker cp ${downloadedDir}/libopentracing.so ${containerID}:${libDir}
# Verify the copy
echo "INFO: ls -l ${libDir}"
sudo docker exec -t -i ${containerID} ls -l ${libDir}
echo ""

# Ensure ld can resolve everything
sudo docker exec -t -i ${containerID} ldconfig ${libDir}
# Verify ldconfig worked
echo "INFO: ldd ${libDir}/libnr_opentracing.so"
sudo docker exec -t -i ${containerID} ldd ${libDir}/libnr_opentracing.so
echo ""

# No longer needed, kept as an example
# Start the daemon
#sudo docker exec -t -i ${containerID} which nohup
#echo "INFO : nohup /etc/newrelic/newrelic-daemon"
#sudo docker exec -t -i ${containerID} nohup /etc/newrelic/newrelic-daemon
#sudo docker exec -it ${containerID} /bin/bash

# Install the daemon init file.
# Copy the init script /etc/init.d
#sudo docker cp ${downloadedDir}/newrelic-daemon.sh ${containerID}:/etc/init.d/newrelic-daemon
#sudo docker exec -t -i ${containerID} chmod +x /etc/init.d/newrelic-daemon
# Run update-rc.d in the container
#sudo docker exec -t -i ${containerID} update-rc.d newrelic-daemon defaults
# Verify
#echo "INFO : verify init script installation into rc* directories"
#sudo docker exec -t -i ${containerID} sh -c "ls -l /etc/rc*"
#echo ""

# DIRE WARNING: if the Docker Image you're working with isn't running an init (the official Nginx images do not)
# then you can't "restart" Nginx
# The following commands depend on the version of Linux you're running for the Image
# Restart NGINX
#sudo docker exec -t -i ${containerID} systemctl restart nginx
#echo "INFO : /etc/init.d/nginx restart"
#sudo docker exec -t -i ${containerID} /etc/init.d/nginx restart
# Verify
#sudo docker exec -t -i ${containerID} systemctl status nginx
#echo "INFO : /etc/init.d/nginx status"
#sudo docker exec -t -i ${containerID} /etc/init.d/nginx status
#echo ""

# Save container as a new image and tweak the CMD so we don't have to have an init process
# docker  image inspect ${imageID}
# Original image CMD: "Cmd": [ "nginx", "-g", "daemon off;" ],
# CMD ["sh","-c","/etc/init.d/newrelic-daemon start && nginx -g daemon off"]
#sudo docker commit --change='CMD /etc/newrelic/ContainerStartup.sh' ${containerID} ${imageID}-newrelic
sudo docker commit  ${containerID} ${imageID}-newrelic
# Verify
echo "INFO : docker images"
sudo docker images
echo ""

