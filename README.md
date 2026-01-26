# sniffles

sniffles is a C++17 packet sniffer project that starts as a CLI for listing capture interfaces. This repo provides the scaffolding for later capture, decode, and UI layers while keeping the current milestone small and testable.

## Build

```sh
cmake --preset debug
cmake --build --preset debug
```

## Example usage

```sh
./build/debug/sniffles --help
./build/debug/sniffles --list-ifaces
```
