#!/usr/bin/env python2.4
#  $Id: checkds.cgi,v 1.1 2008/09/12 10:17:12 choutko Exp $
# this script suppose to check datasets
#
import sys, os
import cgi
import cgitb; cgitb.enable()
sys.path.insert(0,'/afs/ams.cern.ch/AMSDataDir' + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient
print "Content-Type: text/html"     # HTML is following
print                               # blank line, end of headers

print "<TITLE>CGI script output</TITLE>"
form = cgi.FieldStorage()
if not (form.has_key("CEM")):
    print "<H1>Error</H1>"
    print "Please fill in DataSetName"
    sys.exit()
#print "<p>name:", form["CEM"].value
source=form["CEM"].value
f=0
v=1
i=0
run2p=0
h=0
tabulated=1
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-v": v=1
    elif x == "-f": f=1
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
html.CheckDataSet(run2p,source,v,f,tabulated)


