---
name: Feature Request
about: Suggest a new feature or improvement
title: "[FEAT] "
labels: enhancement
assignees: ''
---

## Problem Statement
What limitation or gap prompted this request?
e.g. *"When restoring files deleted across multiple executions, there is no way to preview which batch group I'm targeting before committing with `--last`."*

## Affected Command(s)
Which sub-command(s) would this involve?
- [ ] `delete`
- [ ] `restore`
- [ ] `list`
- [ ] `erase`
- [ ] `validate`
- [ ] New command / other

## Proposed Solution
Describe the feature and how you'd invoke it.

```bash
# Example of what the command might look like
rcb list --last --dry-run
```

## Schema or State Impact
Would this require changes to any of the following?
- [ ] `rcb.sqlite3` table schema
- [ ] `file/` naming or structure
- [ ] `wipe/` behavior
- [ ] `sign/` / IPC
- [ ] `RCB_DIR` / environment config
- [ ] None

## Alternatives Considered
Any workarounds you've tried, or how other tools handle this (e.g. `trash-cli`, macOS Trash, Windows Recycle Bin, `--sql` passthrough).

## Additional Context
Anything else — related issues, edge cases around external devices, multi-part extensions, directory depth, or concurrent execution behavior.