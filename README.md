# DAEMON task ver. 1
Может выполнить 1 команду из cmd.txt по SIGINT.

По SIGTERM заканчивает работу.

# Usage command
It will create out.txt and log.txt in your directory.
```bash
gcc daemon.c -o daemon && ./daemon cmd.txt && kill -SIGINT $(pidof daemon) && kill -SIGTERM $(pidof daemon)
```


