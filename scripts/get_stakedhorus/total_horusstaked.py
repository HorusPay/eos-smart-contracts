# find the total amount of staked HORUS from the scripts that we ran on 9/10/18
# by: gunnar pope
# telegram: @gunnarpope


if __name__ == '__main__':

    f = open('totalstaked_horus_accounts.txt','r').readlines()
    owners =  [ line.split(":")[1].split(",")[0] for line in f if "owner" in line]
    staked =  [ line.split(":")[1].split(",")[0] for line in f if "HORUS" in line]

    print( owners )
    print ( staked)
    print (len(owners))
    print (len(staked))


    all_data = [ [acct, staked.strip('\n')] for (acct, staked) in  zip(owners, staked) ]

    with open("staked_horus_FINAL_09112018.txt", "w") as output:
        output.write(str(all_data))


    fname = 'totalstaked_horus_amounts.txt'

    with open(fname) as f:
        content = f.readlines()

    staked = [x.strip() for x in content]

    total_staked = 0
    for horus in staked:
       total_staked += float( horus)

    print('The total staked HORUS: ', total_staked)
    print('The number of staking accounts', len(all_data))







