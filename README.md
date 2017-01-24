# Nss - Ncurses search sploit

Exploit-DB's (Offensive Security) Ncurses search tool.

## INSTALLATION

```shell
ncurses  >= 6.0
libxml++ >= 3.0
exploit-database (https://github.com/offensive-security/exploit-database.git)
```
```shell
make
make install
```
## CONFIGURATION

~/.nss

```shell
# Path to exploit-database directory.
git_path=/home/user/exploit-database/

# Path to copy exploits directory.
cp_path=/home/user/

# Editor to open exploits.
editor=vim
```
## LICENSE

Free Software under GPL 2.
