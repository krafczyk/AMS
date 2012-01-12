#!/usr/bin/env python2.4
#  $Id: checkds.cgi,v 1.5 2012/01/12 17:05:03 ams Exp $
# this script suppose to check datasets
#
import sys, os
import cgi
import cgitb; cgitb.enable()
sys.path.insert(0,'/afs/cern.ch/ams/Offline/AMSDataDir' + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient
body="""
<html>
<head>
<title>AMS DataProduction CheckDataSet Form</title>
<!-- Changed by: Alexei KLIMENTOV, 18-Sep-2002 -->
<META HTTP-EQUIV="Expires" CONTENT="Fri, Jun 12 1981 08:20:00 GMT">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
<!--
A:link    {  text-decoration: none}
A:visited {  text-decoration: none}
A:hover   {  text-decoration: none}
A:link.nav {  text-decoration: none}
A:visited.nav {  text-decoration: none}
A:hover.nav {  text-decoration: none}
.nav {  color: #000000}
</style>
</head>
<body bgcolor=cornsilk text=black link=#007746 vlink=navy alink=tomato onLoad=getDir()>
<basefont face="verdana,arial,helvetica,sans-serif">

<!-- Header material -->
<table border=0   cellpadding=5 cellspacing=0 width="100%">
        <tr bgcolor="#ffdc9f">
        <td align=left> <font size="-1"><b>
        <a href="/AMS/ams_homepage.html">AMS</a>
        &nbsp; <a href="/AMS/Computing/computing.html">Computing</a>
        </b></td>
        <td align=right> <font size="-1">
        &nbsp;  <!-- top right corner  --> </font></td>
        </tr>
        <tr bgcolor="#ffdc9f"><td align=center colspan=2><font size="+2" color="#3366ff"> <b>
        AMS Production DataSet Check Form
        </b></font></td></tr>
        <tr bgcolor="#ffdc9f">
        <td align=left> <font size="-1">
        <!-- lower left text --> &nbsp;
        </td>
        <td align=right> <font size="-1">
&nbsp; </font></td>
        </tr>
        <tr><td colspan=2 align=right> <font size="-1">
<!-- Generated at Thu Jul 22 10:55:06 1999 -->
 </font></td></tr>
</table>

<p>
<!-- Content -->
<center><font size="-1">
</font></center>
<FORM action="http://pcamss0.cern.ch/cgi-bin/mon/checkds.cgi" METHOD="POST">
<p>
<ul>
 </ul>
<p>



<table border="1" width="100%">
<tr>
<td>
</tr>
<tr><td>
<tr><td>
<b><font color="red" size="-1"> </font></b>
</td><td>
<table border=0 width="100%" cellpadding=0 cellspacing=0>
</tr>
</b>
</font>
</td>
<tr>
<td width>
<font size="-1">
<b>
DataSetName : </td><td><font size="-1"><input type="text" size="24" value="" name="CEM"><i>(cern.test.remote) </i>
</b>
</font>
</td>
<tr>
<td width>
<font size="-1">
<b>

Use DataFiles : </td><td><font size="-1"><input type=checkbox size="4" name="cem_frc" value="force">
</b>
</font>
</td>

</tr>
</table>
</table>
<p>
<br>
<input type="submit" value="Continue" name="RemoteClientDSTCheck"> &nbsp
<input type="hidden" value="Continue" name="RemoteClientDSTCheck">
</form>
<p><tr><td><i><b>
</i></td></tr>
</body>
</html>
"""

form = cgi.FieldStorage()
if not (form.has_key("CEM") and form.has_key("RemoteClientDSTCheck") ):
    print body
    #sys.exit()
#print "<p>name:", form["CEM"].value
source=form["CEM"].value
print body
f=0
v=1
i=0
run2p=0
h=0
tabulated=1
if(form.has_key("cem_frc")):
    f=1
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-v": v=1
    elif x == "-f": f=1
    elif x == "-t": tabulated=0
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
html.CheckDataSet(run2p,source,v,f,tabulated)


