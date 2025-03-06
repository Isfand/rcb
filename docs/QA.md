# Q&A:
The goal behind this project is to create an independent cross-platform CLI file recycling implementation with a focus around scripting. 

Prospective: In the future it can optionally interact with a given OS's native trash implementation through the use of a helper program,
which will act as a bridge between rcb and the OS specific implementation. 
This will allow rcb to remain independent and make the helper program entirely optional.

## 1. Why not follow the [XDG Trash Specification](https://specifications.freedesktop.org/trash-spec/latest/)?

1.1. The XDG trash specification doesn't have the features or the design goals the program is set out to provide. As one of the design intentions is to provide enough metadata that can be narrowed or expanded to another format.

1.2. XDG's trash spec assumes you are writing a daemon/service that actively monitors files. 
And not a CLI program. Albeit can be made to work reasonably well, but not fully compliant. This is because of features like 'Automatic trash cleaning'.

1.3. On Linux, desktop environments don't all perfectly follow the spec itself. 
E.G Gnome has an expunged/ folder, which is not part of the spec and has no directorySizes at time of writing. 
KDE on the other hand follows it more closely. In-fact they both had their own trashing mechanisms originally before the XDG specification, which is why there are differences.

## 2. Why not use an existing trash cli program.
2.1. I made this program for my own use cases as nothing suitable at the time existed. I found it useful and shared it.

'trash-cli' is non-native with a requirement on python with limited portability. 

'trashy' doesn't support Mac. \
https://github.com/oberblastmeister/trashy

'gtrash' doesn't support windows. \
https://github.com/umlx5h/gtrash

'gio trash', doesn't support Windows or Mac, also the requirements on operating systems like FreeBSD require over 600mb worth of dependencies. I.E gvfs. This is significantly smaller in size and adds more features. For one you can just use literal SQL to query files. Instead of using POSIX file utilities, which otherwise wouldn't exist on Windows.

'gomi' would have been the closest option. I recommend using this if your focus is around XDG-Trash and TUIs. It works on all major operating systems, including FreeBSD. Even has a config implemented. It's also mature and actively developed.

## 3. Why use build2 instead of defacto cmake. 
3.1. I like build2 because it's better in almost every way except for the lack of IDE integration.
Build2 is probably the closest thing to a unified c++ ecosystem. Similar to rust's cargo.
You can very easily add and remove dependencies from cppget.org or external repos with little hassle. 
cppget also shows if a library works or not on many different targets including previous versions.  
Composite static and dynamic linking is also made easy.
Along with the fact you need minimal syntax to get a project up an working compared to cmake.

To achieve a similar effect on cmake I would have to use something like microsoft's VCPKG or conan
along with copy pasting a heap of boilerplate code for every project and file. 
VCPKG packages only have the latest version provided and I do not know if the newest package will build on my target or not.
