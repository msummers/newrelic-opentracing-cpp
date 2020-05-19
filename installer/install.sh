#!/usr/bin/env bash
#
# An opinionated script that sets-up a preinstalled NGINX with the New Relic Open Tracing Tracer.
#
# Use: ./install.sh <downloaded_directory>
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

function finish() {
  if [ $? -eq 0 ]; then
    echo "Success"
  else
    echo "Failure"
  fi
  echo "INFO: Finish"
}
trap finish EXIT

# Check if we're root
if [[ $EUID -ne 0 ]]; then
   echo "FATAL: This script must be run as root- sudo me"
   exit 1
fi
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

# Ensure NGINX was built with --with-compat
if nginx -V 2>&1 | grep -q with-compat; then
  echo "INFO : NGINX built with --with-compat"
else
  echo "FATAL: NGINX is not built with --with-compat, failing"
  exit 1
fi
echo ""

# Get location of NGINX modules directory
modulesDir=$(find / -name modules -type d | grep -i nginx)
if [ -z ${modulesDir+x} ]; then
  echo "FATAL: NGINX modules directory not found. Exiting"
  exit 1
else
  echo "INFO : NGINX Modules Dir: ${modulesDir}"
fi
echo ""

# Copy ngx_http_opentracing_module.so to the module directory
cp ${downloadedDir}/ngx_http_opentracing_module.so ${modulesDir}

# Check the result:
echo "INFO: ls -l ${modulesDir}/"
ls -l ${modulesDir}/
echo ""

# Get location of nginx.conf
nginxConfDir=$(find /etc -name nginx.conf)
nginxConfDir=$(dirname ${nginxConfDir})
if [ -z ${nginxConfDir+x} ]; then
  echo "FATAL: NGINX configuration directory not found. Exiting"
  exit 1
else
  echo "INFO : NGINX Config Dir: ${nginxConfDir}"
fi
echo ""

# Copy your modified nginx.conf
cp ${downloadedDir}/nginx.conf ${nginxConfDir}

# Show the result
echo "INFO : ls -l ${nginxConfDir}"
ls -l ${nginxConfDir}
echo ""

# If needed  copy  config files into NGINX's conf.d directory
if [[ -d "${downloadedDir}/conf.d" ]]; then
  cp ${downloadedDir}/conf.d/* ${nginxConfDir}/conf.d
fi

# Setup /etc/newrelic
newrelicDir=$(find /etc -name newrelic -type d)
if [ -z ${newrelicDir} ]; then
  echo "INFO: creating /etc/newrelic"
  mkdir -p /etc/newrelic
else
  echo "INFO: /etc/newrelic already exists: ${newrelicDir}"
fi
ls -ld /etc/newrelic
echo ""

# Copy non-libs to our directory
cp ${downloadedDir}/newrelic-daemon             /etc/newrelic/
cp ${downloadedDir}/nginxTransactionFilters.txt /etc/newrelic/
cp ${downloadedDir}/nr-opentracing-config.conf  /etc/newrelic/
# Set to executable as required
chmod +x /etc/newrelic/newrelic-daemon
# Verify the copy
echo "INFO: ls -l /etc/newrelic/"
ls -l /etc/newrelic/
echo ""

# Copy libs to /usr/lib/
libDir="/usr/local/lib"
cp ${downloadedDir}/libnr_opentracing.so ${libDir}
cp ${downloadedDir}/libopentracing.so    ${libDir}
# Verify the copy
echo "INFO: ls -l ${libDir}"
ls -l ${libDir}
echo ""

# Ensure ld can resolve everything
ldconfig ${libDir}
# Verify ldconfig worked
echo "INFO: ldd ${libDir}/libnr_opentracing.so"
ldd ${libDir}/libnr_opentracing.so
echo ""

# Install the daemon init file.
cp ${downloadedDir}/newrelic-daemon.sh /etc/init.d/newrelic-daemon
chmod +x /etc/init.d/newrelic-daemon
# Run update-rc.d
update-rc.d newrelic-daemon defaults
# Verify
find /etc/rc* -name "*newrelic*"
echo ""

# Start the daemon
systemctl start newrelic-daemon
# Verify
echo "INFO : newrelic-daemon status"
systemctl status newrelic-daemon
echo ""

# The following commands depend on the version of Linux you're running for the Image
# Restart NGINX
systemctl restart nginx
#/etc/init.d/nginx restart
# Verify
echo "INFO : NGINX status"
systemctl status nginx
echo ""
