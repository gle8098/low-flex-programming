#!/usr/bin/env bash
qemu-system-i386 -m 1980m -d int -no-reboot -monitor stdio -m 4g -cdrom kernel.iso -s
