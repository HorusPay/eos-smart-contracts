#!/bin/bash

# list the running keosd and nodeos processes
pgrep keosd | xargs printf " -p %d" | xargs lsof -Pani
pgrep nodeos | xargs printf " -p %d" | xargs lsof -Pani

