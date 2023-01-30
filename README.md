# gt7-telemetry

Gran Turismo 7 telemetry livestream.

## Install

The following dependency libraries are used:
 - spdlog
 - libsodium

For logging, `spdlog` is assumed to be in `/usr/lib/include`.

For crypto, libsodium requires compilation from source.

See `Makefile` for all build options.

## Usage

Compile with:
```
$ make
```

Run with:
```
$ make run
```

## Notes
Packets are encrypted, currently exploring options for Salsa20 decryption.

