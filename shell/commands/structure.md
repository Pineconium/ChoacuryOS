# How to structure a command

A command is contained in this folder, under this every command has it's own folder with their designated c and header file,
if you need any other functions or any other files, they can be in this folder.

They will all need to be defined and added via the list which the Shell can recognise and take into account when searching
for what the user has entered.

## Return codes

When returning a code in a command, different numbers meen different things:

| Code | Silent | Description                                          |
|------|--------|------------------------------------------------------|
| `0`  | Yes    | Success                                              |
| `1`  | Yes    | Generic fail (Command expected to print error)       |
| `2`  | No     | Incorrect args (Called help command to display args) |