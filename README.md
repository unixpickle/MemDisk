Using MemDisk
=============

MemDisk requires a kernel extension, `MemDisk.kext`, to run. This kernel extension can be built through this Xcode project. If Xcode complains that it cannot find the libraries to link with, note that I had to use a special OS X 10.7 SDK and that you may have to manually re-add the libraries.

Once you compile both the `MemDisk` and `MemDiskController` targets, you may run MemDisk by running the following commands as root in your terminal:

    # chown -R root:wheel MemDisk.kext
    # cp -r MemDisk.kext
    # kextload /System/Library/Extensions/MemDisk.kext

This will run and install the MemDisk kernel extension. Now, launch `MemDiskController.app` and add disks as you please!

A note on unloading/uninstalling
================================

In order for `MemDisk.kext` to unload properly, you must first quit all running instances of `MemDiskController.app`. Then, run these commands as root in your terminal:

    # kextunload /System/Library/Extensions/MemDisk.kext
    # rm -rf /System/Library/Extensions/MemDisk.kext

You will no longer have `MemDisk.kext` running or installed.

Under the hood
==============

MemDisk essentially fools your computer into thinking new block devices have been connected. Instead, however, these block devices are really just memory buffers in the kernel.

The controller program, `MemDiskController.app`, controls the kernel extension using Apple's user client interface. This allows user-space application to communicate easily with I/O Kit drivers.
