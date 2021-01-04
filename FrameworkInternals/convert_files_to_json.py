'''
original_files.txt can be read with load_file.
That returns a list of dictionaries, where each dictionary has mandatory 'name' field and some extra options.
Also each dictionary has a list of files defined within under 'files' key.

Author: Piotr Nikiel

'''

import os
import json
import pdb
from manage_files import load_file



def process_files(files):
    ''' There we get a dict each of them describing files '''
    output = {}
    for f in files:
        fn = f['name']
        f.pop('name', None)
        output[fn] = f
    return output

def similiarity_hash(file):
    '''Takes a file entry and returns a hash from values which are relevant'''
    filtered_out_file = {key:value for (key,value) in file.items() if key not in ['path', 'name']}
    print(filtered_out_file)
    its_hash = hash(frozenset(filtered_out_file))
    return (its_hash, filtered_out_file)

def main():
    files_txt_list_of_dirs = load_file('original_files.txt', os.getcwd())
    #pdb.set_trace()
    # step 1: fix entries.
    print('--- before step 1 ---')
    print(files_txt_list_of_dirs)
    # step 1A: within files section, remove all path entries (this is not necessary for storing as JSON)
    for dir in files_txt_list_of_dirs:
        for file in dir['files']:
            print ('at ', file)
            x = file.pop('path', None)
            print ("-->", x)
    print('--- after 1A ---')
    print(files_txt_list_of_dirs)
    # step 2: remove all "None" entries
    for dir in files_txt_list_of_dirs:
        for file in dir['files']:
            for key in file:
                if file[key] is None:
                    file[key] = True

    print('--- after 2 ---')
    print(files_txt_list_of_dirs)
    # step 3: check all directories to identify defauls
    for dir in files_txt_list_of_dirs:
        if len(dir['files']) < 2: # makes sense for at least two files per dir
            continue
        hashes = {file['name']:similiarity_hash(file)[0] for file in dir['files']}
        print('For directory', dir['name'], 'hashes are', hashes)
        all_hashes_identical = len(set(hashes.values())) <= 1
        common_part = similiarity_hash(dir['files'][0])[1]
        print("Common part", common_part)
        print('Hashes are identical:', all_hashes_identical)
        if all_hashes_identical:

            dir['file_defaults'] = common_part # TODO here fix it
            for file in dir['files']:
                for key in common_part:
                    file.pop(key)
                file['apply_file_defaults'] = True

    print('--- after 3 ---')
    print(files_txt_list_of_dirs)

    r = {dir['name']:dir for dir in files_txt_list_of_dirs}
    for dir in r:
        r[dir].pop('name')
        r[dir]['files'] = {file['name']:file for file in r[dir]['files']}
        #pdb.set_trace()
        for file in r[dir]['files']:
            r[dir]['files'][file].pop('name')
            if 'apply_file_defaults' in r[dir]['files'][file]:
                r[dir]['files'][file] = "<FILE_DEFAULTS_OF_DIR>"



    f=open('original_files.json', 'w')
    f.write(json.dumps(r, indent=4, sort_keys=True))

main()
