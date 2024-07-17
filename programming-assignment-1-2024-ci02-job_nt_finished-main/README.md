## How to compile and run your shell 
To compile the shell, run in root direcory:

The Makefile will compile and clean the build
```bash
make clean 
```

Run this next to compile and build
```bash
make
```

After building, start the shell by running:

```bash
./cseshell
```
 
## Builtin functions supported

cd - changes the current working directory
help - display help information for the builtin commands
exit - terminates the shell
usage - displays the usage information for specific commands
env - lists all registered environment variable 
setenv - sets a new environment variable 
unsetenv - removes environment variable 

## Additional features supported

history - the shell maintains a history of commands entered during the session, just typing history will show the list of commands used 

settheme - allows the user to change the theme/colour, and user can choose between default (blue), yellow (yellow), green (green)

## Considering sustainability and inclusivity 

Sustainable: It is energy efficent algorithm as the shell is efficient when a command is typed as well as managing the history of the commands used by optimizing minimal CPU usage. A fixed size array is set which limits the amount of memoery used, this prevents excessive memory comsunption as well as ensuring it does not grow indefinitely. The implmentation also ensures that it is efficient as it runs the command history very quickly, which minimises impact on shell performance

Inclusivity: It is inclusive as it allows user to choose their own colour and they can change it anytime to their preference, ensuring a customisable and comfortable user experience. Moreover, the help and usage commands allows users to understand the list of commands they can use and the usage of each commands.