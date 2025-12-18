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

## How to install/test
```bash
#ssh to ec2 instance:
# Install all dependencies
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r)

# Clone repo
git clone https://github.com/ImLordImpaler/Mlloc-Assignment.git

# Go into project directory
cd Mlloc-Assignment

make # Build the module

sudo insmod zero_device.ko zero_device_name=my_zero_device # Load the module

dmesg | tail # Check logs to verify

ls /sys/class/zero_device_class/ # Verify sysfs

dd if=/dev/my_zero_device of=test.bin bs=1K count=2  #Test like /dev/zero
hexdump -C test.bin

#output should be 00 

# Cleanup to remove the device
sudo rmmod zero_device
dmesg | tail # zero_device: unloaded
