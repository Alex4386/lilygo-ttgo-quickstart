#!/usr/bin/env python3

# This script pulls the list of Mozilla trusted certificate authorities
# from the web at the "mozurl" below, parses the file to grab the PEM
# for each cert, and then generates DER files in a new ./data directory
# Upload these to an on-chip filesystem and use the CertManager to parse
# and use them for your outgoing SSL connections.
#
# Script by Earle F. Philhower, III.  Released to the public domain.
from __future__ import print_function
import csv
import os
import sys
from shutil import which

from subprocess import Popen, PIPE, call
try:
    from urllib.request import urlopen
except Exception:
    from urllib2 import urlopen
try:
    from StringIO import StringIO
except Exception:
    from io import StringIO

if which('openssl') is None and not os.path.isfile('./openssl') and not os.path.isfile('./openssl.exe'):
    raise Exception("You need to have openssl in PATH, installable from https://www.openssl.org/")
    
# Mozilla's URL for the CSV file with included PEM certs
mozurl = "https://ccadb-public.secure.force.com/mozilla/IncludedCACertificateReportPEMCSV"

# Load the names[] and pems[] array from the URL
names = []
pems = []
response = urlopen(mozurl)
csvData = response.read()
if sys.version_info[0] > 2:
    csvData = csvData.decode('utf-8')
csvFile = StringIO(csvData)
csvReader = csv.reader(csvFile)
for row in csvReader:
    names.append(row[0]+":"+row[1]+":"+row[2])
    for item in row:
        if item.startswith("'-----BEGIN CERTIFICATE-----"):
            pems.append({
              'corp': row[0],
              'org': row[1],
              'data': item
            })
del names[0] # Remove headers
del pems[0] # Remove headers

# Try and make ./data, skip if present
try:
    os.mkdir("data")
except Exception:
    pass

derFiles = []
idx = 0

# Process the text PEM using openssl into DER files
for i in range(0, len(pems)):
    b = pems[i]
    certName = "data/certstore/"+b['corp']+"/"+b['org']+"/ca_%03d.pem" % (idx)
    
    try:
        os.mkdir("data/certstore/"+b['corp'])
    except Exception:
        pass

    try:
        os.mkdir("data/certstore/"+b['corp']+'/'+b['org'])
    except Exception:
        pass

    data = pems[i]['data']
    thisPem = data.replace("'", "")
    print(names[i] + " -> " + certName)
    ssl = Popen(['openssl','x509','-inform','PEM','-outform','PEM','-out', certName], shell = False, stdin = PIPE)
    pipe = ssl.stdin
    pipe.write(thisPem.encode('utf-8'))
    pipe.close()
    ssl.wait()
    if os.path.exists(certName):
        derFiles.append(certName)
        idx = idx + 1
