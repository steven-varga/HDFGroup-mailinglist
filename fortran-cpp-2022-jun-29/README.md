
Install ZMQ from your package manager, here is one for debian variants:
```bash
echo "deb http://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/ ./" >> /etc/apt/sources.list
wget https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/Release.key -O- | sudo apt-key add
apt-get install libzmq3-dev
```

Download and install [fzmq](https://github.com/richsnyder/fzmq)
```
git clone https://github.com/richsnyder/fzmq && cd fzmq
mkdir build && cd build
cmake -DBUILD_DOCS=OFF ../
sudo make install
```