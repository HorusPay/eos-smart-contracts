CLEOS="cleos"
CLEOS_PARAMS="-u http://dev.cryptolions.io:18888"
FROM="horustester1"
TO="horustester2"
OWNER="horustester1"
STAKE_ID=0
WAIT_PERIOD=7		#in seconds
###########################################################################

# if the -live flag is included, connect to an external network
if [ "$1" = "-live" ] || [ "$2" = "-live" ] || [ "$3" = "-live" ]; then
	CLEOS="$CLEOS $CLEOS_PARAMS"
fi

function printdetails () {
	echo '---------------------------------------------------------'
	echo 'STAKED HORUS'
	${CLEOS} get table horustokenio ${FROM} stakedhorus
	echo 'REFUNDS'
	${CLEOS} get table horustokenio ${FROM} refunds
	echo 'USERRES'
	${CLEOS} get table horustokenio ${FROM} userres
	echo $FROM 'BALANCE:'
	${CLEOS} get currency balance horustokenio ${FROM} HORUS
	${CLEOS} get currency balance horustokenio ${FROM} ECASH
	echo $TO 'BALANCE:'
	${CLEOS} get currency balance horustokenio ${TO} HORUS
	${CLEOS} get currency balance horustokenio ${TO} ECASH
	echo '---------------------------------------------------------'
}

function printSleepPrint () {
	printdetails
	sleep ${WAIT_PERIOD}
	printdetails
}

# if the -print flag is included, print the details and exit
if [ "$1" = "-print" ] || [ "$2" = "-print" ] || [ "$3" = "-print" ]; then
	printdetails
	exit
fi

boundstest () {
	echo 'EXPECTED: assertion fail due to staking min requirements'
	${CLEOS} push action horustokenio stakehorus '["'$FROM'","'$FROM'","10.0000 HORUS"]' -p ${FROM}
	echo 'EXPECTED: assertion fail due to staking greater than liquid balance'
	${CLEOS} push action horustokenio stakehorus '["'$FROM'","'$TO'","2000000.0001 HORUS"]' -p ${FROM}


	# echo 'EXPECTED: assertion fail staking greater than liquid balance'
	# ${CLEOS} push action horustokenio stakehorus '["'$FROM'","'$FROM'","1.0000 HORUS"]' -p ${FROM}
}

percentagetest() {
	echo 'EXPECTED: exact tokens staked'
	${CLEOS} push action horustokenio stakehorus '["'$FROM'","'$FROM'","1000000.0000 HORUS"]' -p ${FROM}

	echo 'EXPECTED: exact tokens staked'
	${CLEOS} push action horustokenio stakehorus '["'$FROM'","'$FROM'","1000.0000 HORUS"]' -p ${FROM}
}

staketest () {
	echo 'CREATE 3 STAKES'
	${CLEOS} push action horustokenio stakehorus '["'$1'","'$2'","100.0000 HORUS"]' -p ${1}
	${CLEOS} push action horustokenio stakehorus '["'$1'","'$2'","50.0000 HORUS"]' -p ${1}
	${CLEOS} push action horustokenio stakehorus '["'$1'","'$2'","25.0000 HORUS"]' -p ${1}
	printdetails

	${CLEOS} push action horustokenio unstakehorus '["'$1'","1"]' -p ${1}
	printSleepPrint

	${CLEOS} push action horustokenio unstakehorus '["'$1'","2"]' -p ${1}
	printSleepPrint

	${CLEOS} push action horustokenio claimreward '["'$1'","0"]' -p ${1}
	printdetails

	${CLEOS} push action horustokenio claimreward '["'$1'","0"]' -p ${1}
	printdetails

	# sleep ${WAIT_PERIOD}
	${CLEOS} push action horustokenio claimreward '["'$1'","0"]' -p ${1}
	printdetails

	${CLEOS} push action horustokenio unstakehorus '["'$1'","0"]' -p ${1}
	printSleepPrint
}

stakeforbob () { 
	echo $1
}


# stake for myself
# staketest $FROM $FROM

# stake for a friend
boundstest

# staketest $FROM $TO



########### COMMMAND LINE HELPERS  ###########
# cleos -u http://dev.cryptolions.io:18888 push action horustokenio transfer '[ "horustester1", "bob", "0.5250 ECASH", "Cash is King" ]' -p horustester1

# cleos -u http://dev.cryptolions.io:18888 get account horustester1
# cleos -u http://dev.cryptolions.io:18888 get table horustokenio horustester1 stakedhorus
# cleos -u http://dev.cryptolions.io:18888 get table horustokenio horustester1 refunds
# cleos -u http://dev.cryptolions.io:18888 get table horustokenio horustester1 userres
# cleos -u http://dev.cryptolions.io:18888 get currency balance horustokenio horustester1 HORUS
# cleos -u http://dev.cryptolions.io:18888 get currency balance horustokenio horustester1 ECASH
# cleos -u http://dev.cryptolions.io:18888 push action horustokenio unstakehorus '["horustester1","0"]' -p horustester1
# cleos -u http://dev.cryptolions.io:18888 push action horustokenio claimreward '["horustester1","0"]' -p horustester1

# ${CLEOS} push action horustokenio claimreward '["'$OWNER'","stake_id"]' -p ${OWNER}
# ${CLEOS} push action horustokenio stakehorus '["'$FROM'","'$TO'","100.0000 HORUS"]' -p ${FROM}
