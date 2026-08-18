#!/bin/sh
######## Definition ########
pass_sum=0
fail_sum=0
miss_sum=0

######## Function ########
function md5Verify(){
    file=$1
    #echo "input:" $abs_path ${file%:*}
    abs_path=$2

    md5=`md5sum $abs_path/${file%:*}`
    #md5=`md5sum $abs_path`
    md5=${md5% *}
    md5=${md5// /}
    #echo "expected:" ${file#*:}x "infact:" "${md5}"x

	if [ "${md5}"x = ${file#*:}x ]; then
        pass_sum=$(($pass_sum + 1))
        status="Pass"
        printf "\033[32m %-25s |%9s| %s\033[0m\n" ${file%:*}  $status $abs_path
        #echo ${file%:*} "Pass"
    else
        fail_sum=$(($fail_sum + 1))
        status="Failed"
        printf "\033[31m %-25s |%9s| %s\033[0m\n" ${file%:*}  $status $abs_path
        #echo ${file%:*} "Failed"
        #echo "Warning! Failed to verify md5sum for "$abs_path/${file%:*}"!"
        #echo "Expected MD5:" ${file#*:}
        #echo "Actually MD5:" ${md5}
        #echo ""
    fi
}

######## Main ########
root_dir=$1
#echo "Input:" $root_dir

if [ $# != 1 ]; then
    echo "Error - Please input the project directory. If onbard, please input '/'"
    exit 1
fi

set ''
set -- "$@" $1
#echo  $dir_list

files=$(cat md5_info.json | awk '{gsub(/ /,"")}1' | sed -r 's/",/"/' | egrep -v '^[{}]' | sed 's/"//g')
#echo $files

echo "====MD5 Verify Start======================================================="
printf "         File Name         |   Result|    Abs Path\n"
echo "---------------------------------------------------------------------------"
for file in $files; do
    #for dir in do "$@"; do
    for dir in "$@"; do
        #echo "AAAAA" ${file%:*} $dir
        result=`find $dir -name ${file%:*}`
        #echo "local:" $result
        if [ "$result" != "" ]; then
            #echo "local hit!"
            md5Verify $file $dir
            break
        fi
    done

    if [ "$result" = "" ]; then
        results=`find $root_dir -path /proc -prune -o -path /sys -prune -o -name ${file%:*}`
        #echo "global:" $results
        #if [ "$results" == "" ] || [ "$results" == "/proc /sys" ]; then
        rst=$(echo $results | grep "${file%:*}")
        if [ "$rst" == "" ]; then
            #echo "Warning! Not found file "$abs_path/${file%:*}"!"
            #echo ${file%:*} "Not Found"
            status="NotFound"
            printf "\033[33m %-25s |%9s| %s\033[0m\n" ${file%:*}  $status $abs_path
            miss_sum=$(($miss_sum + 1))
        else
            for result in $results; do
                if [ "$result" == "/sys" ] || [ "$result" == "/proc" ]; then
                    continue
                fi
                #echo "global hit!"
                md5Verify $file ${result%/*}
                        #echo "New path:=======" ${result%/*}
                set -- "$@" ${result%/*}
                #echo "dir_list:" ${dir_list[*]}
            done
        fi
    fi
done

echo "---------------------------------------------------------------------------"
printf "Summary: \033[32mPass file: %s\033[0m, \033[31mFailed file: %s\033[0m, \033[33mNotfound file: %s\033[0m\n" $pass_sum $fail_sum $miss_sum
echo "================MD5 Verify Finished========================================"
