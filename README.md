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

#Install not yet implemented. Follow the generated symlink. You can instead manually cp the generated executable binary to /usr/bin/ or /usr/local/bin/

POSIX man pages can also be manually compressed and copied over to relevant paths.

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

*WARNING*: Be sure to add a forward slash delimiter '/' at the end to signify a directory, otherwise it will concatenate would-be inner directory names and treat it as the actual inner trash directory. In other words, in the above example 'shared' which is a parent directory would become the root trash directory instead of '.trashAlt'.

---

If you find the program useful then consider [donating](https://www.paypal.com/donate/?hosted_button_id=ZP93X3GYEJBA4)

---
