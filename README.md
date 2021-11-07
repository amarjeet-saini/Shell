# Shell
osh shell - a simple unix shell implementation.

### Overview
Shell is basically an interactive loop: it repeatedly prints a prompt `osh> `, it parses the input, executes the command
specified on that line of input, and waits for the command to finish. This is repeated until the user types `exit`.
```
prompt> ./osh
osh> 
```
#### Features
1. implementation of basic shell
2. can specify search path for commands
3. added some built-in Commands like `history`, `cd`, `exit`
4. supports redirection i.e `osh> ls -l > output`

Detail implementation and notes @[blog](https://amarjeet-saini.github.io/projects/osh)
