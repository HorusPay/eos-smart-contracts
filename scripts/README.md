# Useful Scripts for EOS.IO Blockchain Developers
@author gunnar pope


Special thanks to from the nice work of @scott-schechter that helped me kickstart these scripts.
## Setup
The entire script base is started by setting he path to your Horuspay/ folder in scripts/setup/setup_env.sh


setup_env.sh
```
HORUS_HOME="${HOME}/repos/Horuspay"

```

If this is the first time running the scripts, create a new wallet and import they keys found in the setup_env.sh file.



## Run the scripts
This script will load all of the variables, open your default wallet,
and launch nodeos. Two accounts will also be created named acct1 and acct2


```
~/eos-scripts $ ./ignition.sh
```


## Find the open processes running (keosd and nodeos)
```
~/actions $ ./list_processes.sh 
COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
keosd   7405   gp    7u  IPv4  85037      0t0  TCP 127.0.0.1:8900 (LISTEN)
COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
nodeos  7397   gp   12u  IPv4  85050      0t0  TCP 127.0.0.1:8888 (LISTEN)
nodeos  7397   gp   13u  IPv4  85052      0t0  TCP *:8800 (LISTEN)

```

## Shut down the blockchain and wallet
```
scripts/actions $ ./shutdown.sh
-------------shutting down keosd and nodeos --------------
scripts/actions $ ./list_processes.sh 
scripts/actions $
```
