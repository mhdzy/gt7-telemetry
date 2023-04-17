# gt7-telemetry

Gran Turismo 7 telemetry livestream.

## Install

The following dependency libraries are used:
 - spdlog
 - libsodium

For logging, `spdlog` is assumed to be in `/usr/local/include`.

https://github.com/gabime/spdlog#install

```sh
git clone https://github.com/gabime/spdlog.git
sudo mkdir -p /usr/local/include/spdlog
sudo cp -R spdlog/include/spdlog/* /usr/local/include/spdlog/
```

For crypto, libsodium requires compilation from source.

https://doc.libsodium.org/installation

```sh
curl -sL https://download.libsodium.org/libsodium/releases/LATEST.tar.gz | tar xz
cd libsodium-stable
./configure
make && make check
sudo make install
```

See `Makefile` for all build options.

## Usage

Compile with:
```
$ make
```

Run with:
```
$ make run <track>
```

## Notes
Packets are encrypted, currently exploring options for Salsa20 decryption.

