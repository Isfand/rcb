<span style="font-family:arial; font-size:2em;">**Design Overview**</span>

# Setup

## Directory structure

### Directory defaults

<pre>
$HOME/.rcb
├── data
│   └── rcb.sqlite3
├── file
├── wipe
└── word
</pre>

The rcb directory location and name should be changeable through the use of a single environment variable. I.E RCB_DIR.


### **data/**
Contains saved details on the file that can be used to list, restore and erase. 

**sqlite3 database and tables:**\
By default the database 'rcb.sqlite3' has a table called 'rcb'
it has the following schema:
<pre>
CREATE TABLE IF NOT EXISTS "rcb" (
        "id" INTEGER PRIMARY KEY AUTOINCREMENT,
        "file" varchar(65535) UNIQUE,
        "directory" varchar(65535),
        "timestamp" BIGINT UNSIGNED,
        "size" BIGINT UNSIGNED,
        "filetype" varchar(65535),
        "depth" BIGINT UNSIGNED,
        "user" varchar(65535),
        "execution" BIGINT UNSIGNED);
</pre>

**Explanation:**\
"id" is the unique auto incremented identifier, which can be used to select a record to perform actions on.\
"file" is the possibly altered unique filename within ~/.rcb/file/.\
"directory" is the original system filepath. This is where the file can be restored as.\
"timestamp" POSIX timestamp for the exact time in seconds when the file was 'deleted' and saved as a record.\
"size" is the size contents of the file or directory(recursive).\
"filetype" is a type of file which the filesystem supports.\
"depth" is the directory depth from root.\
"user" is the one who ran the delete command on the file.\
"execution" is the relative execution order which the files where deleted in. 

### **file/**
Contains deleted files that can be restored. The name is changed if needed to prevent overwriting existing ones through the use of regex.
E.G 'filename', if a duplicate, it will be incremented to 'filename(1)'.

### **wipe/**
This is where the files are moved to be permanently wiped/removed. It exists as a way to mark files to be permanently removed by placing them inside a specialized directory. Because using 'unlink()' is faster than 'rename()'. This means if there was a large file that needed to be removed and something cancelled the process, the file would not be removed. But because we have wipe/ we know everything inside this directory is marked for removal. It is also useful for using a 'shred' like feature for files inside wipe/.

### **word/**
Contains files used for IPC (Inter-Process Communication).

## Globals

<pre>
g_progName    | Program name
g_progVersion | Program version
g_singleton   | Forced single class instance
</pre>

The following variables have global immutable access after the first assignment via g_singleton. They can only be mutated once and have a single instance of access.

<pre>
get/set+workingProgDir     | Defaults to $HOME/.rcb/ or use RCB_DIR
get/set+workingProgFileDir | file/ location
get/set+workingProgDataDir | data/ location
get/set+workingProgWipeDir | wipe/ location
get/set+workingProgWordDir | word/ location
get/set+workingUsername    | Environment variable $USER 
</pre>

# Commands
The primary commands should be parsed **independently**, every command should be parsed by their respective parsers that set options that are **local** to the commands, with the **exception** of having internal shared utilities, global environment for **directories**, **config** and the **user** who executed the program. The general idea is to make the program have usable utilities, like, for example, BusyBox.

Parsing the arguments from the **main entry point** should involve **redirecting** the arguments depending on what **command** is entered. As you should only use one primary command per program execution.

As the primary commands themselves have their own argument parsers to set options. Once the parser has **completely** parsed all arguments, it should **invoke** the primary command object itself as the **last** action, passing the options as an object. The primary commands then change their **behavior** depending on what options are **enabled**.

Some options such as '--help' are messages displaying parser usage and not behavior changes. They are specific to the parser's implementation and should only be invoked by the parser directly.

'--' is when options end. This is also local to the parser as it would simply pass any remaining argument(s) AS-IS. 

**--default**\
Optional: Performs default execution behavior. Does not need parsing.

Reference for units of time:

| Unit          | shorthand |
| ------------- | --------- |
| Posixtime     | t         |
| Second(s)     | s         |
| Minute(s)     | m         |
| Hour(s)       | h         |
| Day(s)        | d         |
| Week(s)       | w         |
| Month(s)      | mo        |
| Year(s)       | y         |
| Decade(s)     | dec       |
| Century(s)    | c         |
| Millennium(s) | k         |
| Aeon(s)       | ae        |

## Delete
Transfer existing files into the file/ directory. This is done by checking if the filepath exists and checking if the filename already exists or not inside of file/, if it does exist the filename is altered in the format of '\<filename>(n)'. Do not modify any extensions. Store the values for each column which match the 'rcb' table schema in data/. The final action should be to 'rename()' the original path to the new one inside file/.

For files inside external devices you cannot use 'rename()'. Instead use 'copy_file_range()', followed by 'unlink()' to the original path. In other words copy and remove the original.
### Options:
**--** \
Managed by parser. Options end after this point \
**--verbose** \
Print extra information on program state \
**--silent** \
Prevent printing anything with auto default(s). \
**--force** \
Force file deletion if possible. \
**--no-directorysize** \
Do not save size for directories. Write as 'NULL' \

## Restore
Transfer files back to their original paths. This is done by reading the 'directory' column value(s) for the corresponding id entered and using 'rename()' on the filename on the file(s) inside file/.

The final action should be to DELETE the record inside the table.

For files inside external devices you cannot use 'rename()'. Instead use 'copy_file_range()', followed by 'unlink()' to the original path. In other words copy and remove the original.
### Options:
**\<n>**... \
Restore the files by record id number(s) entered. \
**--all** \
Restore all files with lowest depth ascending. \
**--verbose** \
Print extra information on program state. \
**--force** \
Do not ask the user for input. Perform the action(s) requested. \
**--force-replace** \
Restore by replacing any existing file from original path. \
**--force-rename** \
Restore by renaming any existing file from original path by incrementing filename. \
**--force-recreate-directory**\
Restore by recreating any directories leading up-to the original filepath. \
**--silent** \
Prevent printing anything with auto default(s). \
**--previous** \
Restores file(s) with record(s) with the highest execution number. \
**--past \<n>(unit)** \
Restore file(s) with relative time from system time to epoch. \
**--sql '\<s>'** \
Allows for SQL passthrough. AKA SQL injection.

## List
Print record(s) inside data/ - database 'rcb.sqlite3' - table 'rcb'
### Options:
**--all** \
Prints record(s), size and count. \
**--total-size** \
Prints total size of every file recorded in rcb/, but ignore duplicate ino, dev. \
**--total-count** \
Prints total count of files recorded in rcb/. \
**--human-readable** \
Prints records in human readable form. \
**--no-format** \
Prints only the values in-line separated with spaces and nothing else as default. Allow custom delimiter. \
**--previous** \
Print file(s) with record(s) with the highest execution number. \
**--past \<n>(unit)** \
Print file(s) with relative time from system time to epoch. \
**--sql '\<s>'** \
Allows for SQL passthrough. AKA SQL injection.


## Erase
Permanently removes files by reading table records inside data/, matching the corresponding filename(s) to the file(s) inside file/, moving them into wipe/ and then finally permanently removing them.
### Options:
**\<n>**... \
Remove the files by record id number(s) entered \
**--all** \
Remove every file recorded inside data/ by id. Check against 'file' column for matching values that correspond to a file inside file/ \
**--verbose** \
Print extra information on program state. \
**--silent** \
Prevent printing anything with auto default(s). \
**--force** \
Do not ask the user for input. Perform the action(s) requested. \
**--previous** \
Erase file(s) with record(s) with the highest execution number. \
**--past \<n>(unit)** \
Erase file(s) with relative time from system time to epoch. \
**--sql '\<s>'** \
Allows for SQL passthrough. AKA SQL injection.


## Validate
Checks for dangling file(s) and record(s) inside of the rcb directory along with any other corrections. Print the file(s)/record(s)/corrections(s) found with verbose and prompt user a message asking input for removal/correction.
### Options:
**--all** \
Checks for all of the below. \
**--file** \
Checks for dangling file(s) in file/ that have no corresponding record in data/. \
**--data** \
Checks for dangling record(s) in data/ that have no corresponding file in file/. \
**--wipe** \
Checks for dangling file(s) in wipe/ that exist. \
**--fill-directorysize** \
Checks for 'NULL' directory sizes and evaluates them. \
**--verbose** \
Print extra information on program state. \
**--silent** \
Prevent printing anything with auto default(s). \
**--yes** \
Do not ask the user for input. Perform the action(s) requested.

---
#### Copyright © 2024-2025, Isfand Mirza
---