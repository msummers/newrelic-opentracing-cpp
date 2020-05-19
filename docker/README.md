# Sample Docker Setup
Here are some _guidelines_ for using the plugin with Docker. Your mileage can and will vary.

1. `InstallInDocker.sh` is a complete working example of how to install the plugin into a fresh NGINX Docker instance.
  - `./InstallInDocker.sh <NGINX-Image> downloaded`
2. The only way I could get the plugin to connect to the C Daemon was by using the host ip and the Daemon port.
3. `downloaded/conf.d` is _optional_
4. _*READ `InstallInDocker.sh` BEFORE YOU START*_

## Useful Docker commands
- `sudo docker run  -it <Image>     <Bash command> # Get a bash command line in the image`
- `sudo docker exec -it <Container> <Bash command> # Get a bash command line in the container`
- `./InstallInDocker.sh nginx:1.17.3 downloaded    # Run the installer`
- `docker-compose up -d                            # Bring-up everything from the docker-compose.yml file`
- `sudo docker ps                                  # See the running containers`
- `sudo docker logs <containerID>                  # Get the logs from the container`
- `sudo docker stop <containerID>                  # Stop the container`