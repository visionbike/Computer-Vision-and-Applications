# Docker Toolchain for CUDA + OpenCV + PyTorch Projects in CLion

## Features

The main feature to use docker container for development is to encapsulate all dependencies in a container. This allows to build and run the code on any host system without the need to install dependencies (or only a minimal installation is required).

- [x] Build all dependencies encapsulated in a Docker container (minimal host dependencies).
- [x] Use local NVIDIA GPU in docker.
- [x] Use Pytorch with CUDA support.
- [x] Use OpenCV with local devives (Webcam, screen output) in docker.

## Docker Details

- NVIDIA CUDA 12.6.2.
- NVIDIA CUDNN 9.
- CMAKE 3.38.3
- GCC 12.
- LLVM/CLang 17.
- Ninja 1.11.1.
- Pytorch 2.5.1.
- OpenCV 4.x.
- C++ 17.

## Host System Setup               

- NVIDIA RTX-3070 Ti (Notebook).
- Ubuntu 22.04 LTS.
- NVIDIA Driver 555.58.02.

## Dependencies Installation

### Docker

First, install Docker on your host system. Use the common instructions from [Docker Engine installation guide on Ubuntu](https://docs.docker.com/engine/install/ubuntu/).

##### A. Old version Uninstallation

Before you can install Docker Engine, you need to uninstall any conflicting packages.

```sh
for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc; do sudo apt remove -y $pkg; done
```

Images, containers, volumes, and networks stored in `/var/lib/docker/` aren't automatically removed when you uninstall Docker.

1. Uninstall the Docker Engine, CLI, containerd, and Docker Compose packages:

```sh
sudo apt purge -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin docker-ce-rootless-extras
```

2. Delete all images, containers, and volumes:

```sh
sudo rm -rf /var/lib/docker
sudo rm -rf /var/lib/containerd
```

3. Remove source list and keyrings:

```sh
sudo rm /etc/apt/sources.list.d/docker.list
sudo rm /etc/apt/keyrings/docker.asc
```

#### B. The `apt` repository Installation

1. Set up Docker's `apt` repository.

```sh
# Add Docker's official GPG key:
sudo apt update -y
sudo apt install -y ca-certificates curl
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc

# Add the repository to Apt sources:
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
```

2. Install the Docker packages.

```sh
sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

#### C. Linux Post-installation

1. Add `$USER` to the docker group to avoid using sudo for docker commands:

```sh
# Create the docker group
sudo groupadd docker
# Add $USER to the docker group
sudo usermod -aG docker $USER
```

2. Log out and log back in so that the group membership is re-evaluated.

3. Acticate the changes to groups:

```sh
newgrp docker
su - $USER
```

4. Check the docker availability:

```sh
docker --version
```

5. Verify that you can run `docker` commands without `sudo`.

```sh
docker run hello-world
```

#### D. Docker Configuration for Starting On Boot with `systemd`

1. Start `docker` and `containerd` on boot using `systemd`:

```sh
sudo systemctl enable docker.service
sudo systemctl enable containerd.service
```

2. Disable this behavior:

```sh
sudo systemctl disable docker.service
sudo systemctl disable containerd.service
```

### NVIDIA Container Toolkit

#### A. NVIDIA Driver + CUDA Installation

If the NVIDIA driver is not ready in your host machine, please follow the [CUDA installation guide for Linux](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/). The restart the system to activate the installation.

#### B. NVIDIA Container Toolkit Installation

1. Configure the production repository:

```sh
curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg \
  && curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
    sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
    sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list
```

2. Update the packages list from the repository:

```sh
sudo apt-get update
```

3. Install the NVIDIA Container Toolkit packages:

```sh
sudo apt-get install -y nvidia-container-toolkit
```

#### C. `docker` Configuration

1. Configure the container runtime by using the `nvidia-ctk` command:

```sh
sudo nvidia-ctk runtime configure --runtime=docker
```

The `nvidia-ctk` command modifies the `/etc/docker/daemon.json` file on the host. The file is updated so that Docker can use the NVIDIA Container Runtime.

2. Restart the Docker daemon:

```sh
sudo systemctl restart docker
```

#### D. `containerd` Configuration

1. Configure the container runtime by using the `nvidia-ctk` command:

```sh
sudo nvidia-ctk runtime configure --runtime=containerd
```

The `nvidia-ctk` command modifies the `/etc/containerd/config.toml` file on the host. The file is updated so that containerd can use the NVIDIA Container Runtime.

2. Restart `containerd`:

```sh
sudo systemctl restart containerd
```

## Docker Image Building

Go to the directory where you store the `Dockerfile` and ru the command:

```sh
cd ./docker/
docker build --no-cache --build-arg UID=$(id -u) -t clioncv -f ./Dockerfile .
```

## Acknowledgement

- [https://github.com/sleepingwithshoes/torchopencv](https://github.com/sleepingwithshoes/torchopencv).
