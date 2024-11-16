# **trash**

Tested on Linux & FreeBSD. Windows is WIP.

## **Pre-requisites:**
*Requires the build2-toolchain to be pre-installed. See https://build2.org/install.xhtml \
*Requires c++23 supporting compiler.

## **Build Instructions:**

mkdir trash.d \
cd trash.d \
git clone https://github.com/Isfand/trash.git \
cd trash/

POSIX: **./config-init.sh**\
WIN32: **.\config-init.ps1**

**bdep update -a**

This should build all initialized build configurations. \
To list all build configs, type: `bdep config list` \
To clean all build configs and dependencies recursively, type: `bdep clean -a -r` \
To update a specific config: `bdep update @<config-name>` \
To clean a specific config: `bdep update @<config-name>`

## **Install**

### POSIX:
cd into your release target build configuration directory. E.G: `cd ../trash-x64-posix-llvm-release`

And type: \
`b install config.cxx.loptions="-L/usr/local/lib -Wl,-rpath,/usr/local/lib" config.install.root="/usr/local/" config.install.sudo="sudo"`

To uninstall just replace install with uninstall for the command above. 

This should work on any posix compliant platform.

### WIN32:
WIP

## **basic usage:**
*Note, '--help' option changes message when applied after a primary command:
<pre>
trash --version | 
trash --help    | -h
</pre>

## **Primary commands**

<pre>
delete   | d
list     | l
restore  | r
erase    | e
validate | v
</pre>

### Deleting files:
<pre>
trash delete filename
trash delete -- filename
</pre>

### Listing files:
<pre>
trash list, List all by default
</pre>

### Restoring files:
<pre>
trash restore &lt;n&gt;
trash restore --all | -a, Restore all
</pre>

### Erasing files:
<pre>
trash erase &lt;n&gt;
trash erase --all | -a, erase all
</pre>

### Validating files:
<pre>
trash validate --all | -a, Validate all
</pre>

## **Environment variables:**
**TRASH_DIR**\
The directory where the trash/ folder will be located. Defaults to '$HOME/.trash/'. The directory filename before the last delimiter will be used as the root of the trash directory name.
E.G 'export TRASH_DIR=/home/user/.local/shared/.trashAlt/', will make .trashAlt/ become the trash directory name.

---

If you find the program useful then consider [donating](https://www.paypal.com/donate/?hosted_button_id=ZP93X3GYEJBA4)

---
