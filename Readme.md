# Custom Zero Device Kernel Module

## Overview

This project implements a Linux kernel module that provides a custom
read-only character device which behaves identically to `/dev/zero`.

When read, the device returns an infinite stream of null bytes (0x00).
The device name is configurable via a kernel command-line (module) parameter.

The device is registered using the Linux device model and is visible under:
- `/dev/<device_name>`
- `/sys/class/zero_device_class/<device_name>/`

---

## Design Summary

- Implemented as a **character device**
- Supports **read-only** operations
- Read semantics match `/dev/zero`:
  - Always fills the requested buffer with zeros
  - Never returns EOF
- The device name is provided dynamically using a kernel parameter
- sysfs is used only for device visibility and registration, not for data I/O

---

## Requirements

- Linux system with kernel headers installed
- Build tools (`gcc`, `make`)

On Ubuntu:
```bash
sudo apt install build-essential linux-headers-$(uname -r)
