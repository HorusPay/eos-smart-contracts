# Find Total Staked Horus
@author gunnar pope


This script collects the total amount of staked HORUS for each EOS account on the mainnet. Since it will take about 60 hours to get all of the data, the scripts provided subdivide the EOS snapshot into separate chunks of 25000 accounts so that we can pull the data in parallel.

## To run the script, give the script execute permissions:
```
$ cd Horuspay/eos-smart-contracts/scripts/get_stakedhorus
$ sudo chmod +x get_stakedhorus_yourname.sh
```


Where yourname is jarib, jack, or scott. Then run the script and you will see data being sent to a file named `staked_horus_yourname.txt`.
```
$ ./get_stakedhorus_jarib.sh
```

There were 303891 total EOS accounts at the time of the snapshot, shown in `20180907_accounts_eos.csv`. Jack is retrieving lines 275000-303891. Scott is getting lines 250000 - 274999. Jarib is getting lines 22500-249999. and I'm getting lines 1-224999. (I'm already on line 172288).
