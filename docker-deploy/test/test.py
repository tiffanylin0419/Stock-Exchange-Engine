#! /usr/bin/env python

import os
import sys
import string
import socket
import xml.etree.ElementTree as ET

# May need to change settings in this block:
tgt_host='localhost'
tgt_port=12345
max_buff=4096
run_tests={  #first is test name, second is # expected results in xml response
    'test01' : 2,
    'test02' : 1
}


def RepresentsInt(s):
    try:
        int(s)
        return True
    except ValueError:
        return False
    
def check_test(test, response):
    root = ET.fromstring(response)
    num_expected_results = run_tests[test]
    num_results = len(root.getchildren())
    if (root.tag != 'results'):
        print '  Failed: top-level XML tag in response is not <results>'
        return
    if (num_results != num_expected_results):
        print '  Failed: found '+str(num_results)+' results, but expected '+str(num_expected_results)+' results'
        return
    for child in root:
        if (child.tag != 'created'):
            print '  Failed: found a failed result, but expected successful creation'
            return
    print '  Success!'

def run_test(test):
    req_file = open('in/'+test+'.xml', 'r')
    contents = req_file.read()
    # send_buff = str(len(contents)) + '\n' + contents
    send_buff = contents
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((tgt_host, tgt_port))
    sock.send(send_buff)

    response = ''
    while ('</results>' not in response):
        response = response + sock.recv(max_buff)
    #print response
    if RepresentsInt(response.split('\n')[0]):
        response = '\n'.join(response.split('\n')[1:])
    resp_file = open('out/'+test+'.out', 'w')
    resp_file.write(response)
    check_test(test, response)

    req_file.close()
    resp_file.close()

if __name__ == '__main__':
    for test in sorted(run_tests):
        print 'Running ' + test
        run_test(test)
