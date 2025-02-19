# **RCB**

**Table of Contents**
- [**RCB**](#rcb)
	- [**Summary**](#summary)
	- [**Pre-built binaries:**](#pre-built-binaries)
	- [**Build Pre-requisites:**](#build-pre-requisites)
	- [**Build Instructions:**](#build-instructions)
	- [**Install**](#install)
		- [POSIX:](#posix)
		- [WIN32:](#win32)
	- [**Primary commands**](#primary-commands)
	- [**Basic usage:**](#basic-usage)
		- [Deleting files:](#deleting-files)
		- [Listing files:](#listing-files)
		- [Restoring files:](#restoring-files)
		- [Erasing files:](#erasing-files)
		- [Validating files:](#validating-files)
	- [**Environment variables:**](#environment-variables)

## **Summary**

This project is a cross-platform file recycling utility for the terminal that can delete, restore, erase, list and validate files.

Tested on Linux, FreeBSD & Windows. \
See [docs/](docs/) for more information.

## **Pre-built binaries:**
[Release](https://github.com/Isfand/rcb/releases) binaries are provided for Linux, FreeBSD & Windows.

## **Build Pre-requisites:**
*Requires the build2-toolchain to be pre-installed. See https://build2.org/install.xhtml \
*Requires c++23 supporting compiler. \
*(Optional)*: For building compilation database use latest [staged](https://stage.build2.org/0/) toolchain.

## **Build Instructions:**

**`mkdir rcb.d`** \
**`cd rcb.d`** \
**`git clone https://github.com/Isfand/rcb.git`** \
**`cd rcb`**

POSIX: **`./config_init.sh`**\
WIN32: **`.\config_init.ps1`**

**`bdep update -a`**

This should build all initialized build configurations.

*(Optional Information)* \
To list all build configs, type: `bdep config list` \
To clean all build configs and dependencies recursively, type: `bdep clean -a -r` \
To update a specific config: `bdep update @<config-name>` \
To clean a specific config: `bdep clean @<config-name>`

## **Install**

### POSIX:
cd into your release target build configuration directory. E.G: `cd ../rcb-x64-posix-llvm-release`

And type: \
`b install config.cxx.loptions="-L/usr/local/lib -Wl,-rpath,/usr/local/lib" config.install.root="/usr/local/" config.install.sudo="sudo"`

To **uninstall** just replace `install` with `uninstall` for the command above. 

This should work on any POSIX compliant platform.

### WIN32:
WIP

## **Primary commands**

<pre>
delete   | d
list     | l
restore  | r
erase    | e
validate | v
</pre>

## **Basic usage:**
*Note, '--help' option changes message when applied after a primary command:
<pre>
rcb --version | 
rcb --help    | -h
</pre>


### Deleting files:
<pre>
rcb delete filename
rcb delete -- filename
</pre>

### Listing files:
<pre>
rcb list, List all by default
</pre>

### Restoring files:
<pre>
rcb restore &lt;n&gt;
rcb restore --all | -a, Restore all
</pre>

### Erasing files:
<pre>
rcb erase &lt;n&gt;
rcb erase --all | -a, erase all
</pre>

### Validating files:
<pre>
rcb validate --all | -a, Validate all
</pre>

## **Environment variables:**
**RCB_DIR**\
The directory where the rcb/ folder will be located. Defaults to '$HOME/.rcb/'. The directory filename before the last delimiter will be used as the root of the rcb directory name.
E.G 'export RCB_DIR=/home/user/.local/shared/.rcbAlt/', will make .rcbAlt/ become the rcb directory name.

---

If you find the program useful then consider [donating](https://www.paypal.com/donate/?hosted_button_id=ZP93X3GYEJBA4)

---
