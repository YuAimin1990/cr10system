#! /bin/sh

latest_file_time=0

function get_latest_file_time()
{
    file_found=0
    for file in `ls -t $1`
    do
        #echo ---1 $file
        if [ -d $1"/"$file ] ; then
            #echo ---2 $file
            get_latest_file_time $1"/"$file $2
            if [ $? == "1" ] ; then
                return 1
            fi
        else
            if [ $file_found == "1" ] ; then
                #echo ---3 $1"/"$file
                continue
            fi
            if [ "${file##*.}" == "c" ] || [ "${file##*.}" == "h" ] || [ "${file##*.}" == "cpp" ] || [ "${file##*.}" == "cc" ] || [ "${file##*.}" == "hpp" ] ; then
                #echo ---4 $file
                file_found=1
                tmp_file_time=$(stat -c %Y  $1"/"$file)
                if [ $tmp_file_time -gt $latest_file_time ] ; then
                    latest_file_time=$tmp_file_time
                    if [ $latest_file_time -gt $2 ] ; then
                        echo -e "\033[35;1m Dectected $1"/"$file modified \033[0m"
                        return 1
                    fi
                fi
            fi
         fi
    done

    #echo $latest_file_time ---1

    return 0
}

#echo shell call: $0 $1 $2 $3 $4

ref_file_time=$(stat -c %Y $3)
get_latest_file_time $2 $ref_file_time

if [ $? == "1" ] ; then
    #echo latest_file_time $latest_file_time
    #echo ref_file_time $ref_file_time
    echo -e "\033[35;1m Some file modified in $1 \033[0m"
    rm -f $4
else
    echo -e "\033[35;1m No file modified in $1 \033[0m"
    touch $4
fi

