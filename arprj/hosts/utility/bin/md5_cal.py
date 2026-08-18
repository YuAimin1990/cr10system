#!/usr/bin/env python

import os
import sys
import json
import subprocess

#### Definitions ####
SDK_OUT_DIRS = "./release/out"
SDK_OUT_FILES = []

SDK_OUT_JSON = {}

def md5_cal(file, dir):
    command = "md5sum " + dir + "/" + file
    md5sum = subprocess.check_output(command, shell=True).partition(' ')[0]

    content = file + ":" + md5sum
    SDK_OUT_JSON[file] = md5sum

    print SDK_OUT_JSON

def main():
    result = ""

    if len(sys.argv) <= 1:
        print("Error - Please specify the chipset.")
        return

    chip = sys.argv[1]

    for root, dirs, files in os.walk(SDK_OUT_DIRS):
        for file in files:
            eachfile = os.path.join(root, file)
            SDK_OUT_FILES.append(eachfile)

    for file in SDK_OUT_FILES:
        filename = file.rpartition('/')[2]
        filepath = file.rpartition('/')[0]
        print filename
        print filepath
        #for dir in INCLUDES_DIR:
        command = "find " + filepath + " -name " + filename
        result = subprocess.check_output(command, shell=True)
            #if result != "":
        print("result:", result)
        md5_cal(filename, filepath)
                #break;

        #if result == "":
            #command = "find /home/slchen/hal_mpp_debug/ar9301sw -name " + file
            #result = subprocess.check_output(command, shell=True)
            #if result != "":
                #INCLUDES_DIR.append(os.getcwd())
                #print("file in global:", file, "result:", result, "dir:", INCLUDES_DIR)

    target_path = './configs/' + chip + '/common/tools/md5_info.json'
    with open(target_path, 'w+') as f:
        json.dump(SDK_OUT_JSON, f, indent=1)

    sys.exit(0)


if __name__ == '__main__':
    main()
