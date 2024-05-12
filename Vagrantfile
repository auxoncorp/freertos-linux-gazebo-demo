# -*- mode: ruby -*-
# vi: set ft=ruby :

# NOTE: install the vbox plugin with: vagrant plugin install vagrant-vbguest
# may have to re-run `/sbin/rcvboxadd quicksetup all` in the guest if it fails?

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"

  config.vm.provider :virtualbox do |v|
    v.name = "freertos-linux-gazebo-demo"
    v.gui = true
    v.memory = 8192
    v.cpus = 8
    v.customize ["modifyvm", :id, "--accelerate3d", "on"]
    v.customize ["modifyvm", :id, "--vram", "64"]
  end

  config.vm.provision "shell", env: {"MODALITY_LICENSE_KEY" => ENV['MODALITY_LICENSE_KEY']}, inline: <<-SHELL
    if [ -z "$MODALITY_LICENSE_KEY" ]; then
        echo "Env var MODALITY_LICENSE_KEY is not set"
        exit 1
    fi

    echo 'demo' | sudo tee /etc/hostname

    echo 'DEBIAN_FRONTEND=noninteractive' | sudo tee -a /etc/environment

    sudo apt-get update
    sudo apt-get remove -y --purge unattended-upgrades

    sudo apt-get install -y ubuntu-desktop
    sudo sed -i 's/#WaylandEnable=false/WaylandEnable=false/g' /etc/gdm3/custom.conf

    sudo apt-get install -y --no-install-recommends \
      git \
      sudo \
      tzdata \
      locales \
      wget \
      gnupg \
      lsb-release \
      build-essential \
      ca-certificates \
      iproute2 \
      netcat \
      vim \
      tree \
      pkg-config \
      cmake \
      gcc-arm-none-eabi \
      libnewlib-dev \
      libnewlib-arm-none-eabi \
      binutils-arm-none-eabi \
      libusb-1.0-0-dev \
      libftdi1-dev \
      gettext \
      bridge-utils \
      mono-complete \
      python3-appdirs \
      python3-dateutil \
      python3-pandas \
      python3-pandas-lib \
      python3-six \
      python3-toml \
      python3-tz \
      jq

    sudo sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && sudo locale-gen
    echo 'LANG=en_US.UTF-8' | sudo tee -a /etc/environment
    echo 'LANGUAGE=en_US:en' | sudo tee -a /etc/environment
    echo 'LC_ALL=en_US.UTF-8' | sudo tee -a /etc/environment
    sudo update-ca-certificates

    sudo mkdir -p /usr/local/bin
    sudo cp /vagrant/scripts/setup-network.sh /usr/local/bin/setup-network.sh
    echo '#!/bin/bash' | sudo tee /etc/rc.local
    echo '/usr/local/bin/setup-network.sh &' | sudo tee -a /etc/rc.local
    sudo chmod a+x /etc/rc.local
    sudo /etc/rc.local

    echo 'export PATH="/usr/local/bin:$PATH"' >> /home/vagrant/.bashrc

    sudo wget --no-verbose --quiet https://packages.osrfoundation.org/gazebo.gpg -O /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null
    sudo apt-get update && sudo apt-get install -y libgz-transport12-dev gz-transport12-cli libgz-msgs9-dev gz-garden
    echo 'export GZ_SIM_SYSTEM_PLUGIN_PATH=${HOME}/gazebo_plugins:${GZ_SIM_SYSTEM_PLUGIN_PATH}' >> /home/vagrant/.bashrc
    echo 'export GZ_SIM_RESOURCE_PATH=${HOME}/gazebo_worlds:${GZ_SIM_RESOURCE_PATH}' >> /home/vagrant/.bashrc

    sudo apt-get install -y software-properties-common cmake gcc
    sudo apt-add-repository ppa:lttng/stable-2.13
    sudo apt-get install -y lttng-tools liblttng-ust-dev
    echo 'LTTNG_HOME=/home/vagrant/lttng' | sudo tee -a /etc/environment

    wget --no-verbose --quiet https://github.com/renode/renode/releases/download/v1.15.0/renode_1.15.0_amd64.deb -O /tmp/renode.deb
    sudo apt-get install -y /tmp/renode.deb

    wget --no-verbose  https://vscode.download.prss.microsoft.com/dbazure/download/stable/dc96b837cf6bb4af9cd736aa3af08cf8279f7685/code_1.89.1-1715060508_amd64.deb -O /tmp/code.deb
    sudo apt-get install -y /tmp/code.deb
    sudo -H -u vagrant bash -c 'code --install-extension auxoncorp.speqtr'

    # This is equivalent to accepting the EULA
    sudo -E bash -c 'echo debconf auxon/eula select true | debconf-set-selections'
    sudo -E bash -c 'echo debconf auxon/eula seen true | debconf-set-selections'

    wget --no-verbose --quiet https://get.keygen.sh/auxon-io/auxon.deb -O /tmp/auxon.deb
    sudo apt-get install -y /tmp/auxon.deb
    sudo apt-get update && sudo apt-get install -y modality-client modalityd modality-reflector conform deviant speqtr-lsp modality-sdk
    echo "license-key = '${MODALITY_LICENSE_KEY}'" | sudo tee -a /etc/modalityd/config.toml
    sudo systemctl restart modalityd
    sudo systemctl status modalityd --full --no-pager
    curl --retry-max-time 30 --retry 10 --retry-connrefused http://localhost:14181/v1/alive

    sudo -H -u vagrant bash -c 'modality user create --use admin'
    sudo -H -u vagrant bash -c 'modality workspace create --use demo /vagrant/config/workspace.toml'
    echo 'export MODALITY_AUTH_TOKEN=$(< ~/.config/modality_cli/.user_auth_token)' >> /home/vagrant/.bashrc

    cd /vagrant
    ./scripts/native/build.sh

    sudo reboot
SHELL

end
