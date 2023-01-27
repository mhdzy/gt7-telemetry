# gt7-telemetry

Gran Turismo 7 telemetry livestream.

## Install

This project uses the 'spdlog' library, assumed to be in `/usr/lib/include`.

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

