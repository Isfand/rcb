---
name: Bug Report
about: Report a bug or unexpected behavior
title: "[BUG] "
labels: bug
assignees: ''
---

## Describe the Bug
A clear description of what went wrong.

## Command Used
```bash
# The exact command you ran
rcb delete --verbose ./myfile.txt
```

## Expected Behavior
What did you expect to happen?

## Actual Behavior
What actually happened? Include any error output.

```bash
# Paste error output here
```

## Bin State
- Did the file appear in `rcb list`? Yes / No
- Was the file present in `~/.rcb/file/`? Yes / No
- Was the database record created in `~/.rcb/data/rcb.sqlite3`? Yes / No
- Was the file recoverable via `rcb restore`? Yes / No
- Were there dangling files or records (`rcb validate --all`)? Yes / No

## File Details (if relevant)
### Report who the caller was and groups
`whoami`
`groups`

### If the file is a non-directory, report the permissions of the file
`ls -lahFZ`

### If the type is a directory, report the whole tree with permissions
Depending on size, it may be appropriate to add this as a file attachment \
Linux: `find <path> -exec ls -lahFZd {} +` \
Other: There is a shell script provided if you do not know your OS's equivalent in path `rcb/rcb/tools/scripts/fls.sh`

## Environment
- OS: [e.g. Windows 11 / Ubuntu 26.04 / macOS 26 / FreeBSD 15]
- filesystem type (e.g. ext4, NTFS, zfs, APFS)
- Shell: [e.g. bash 5.1, zsh 5.9]
- rcb version: [e.g. 1.2.3] (`rcb --version`)
- Installation method: [e.g. built from source, binary]
- `RCB_DIR` set to custom path: Yes / No

## Additional Context
Any other context such as concurrent executions