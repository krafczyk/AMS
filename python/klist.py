#!/usr/bin/env python2.4
#  $Id: klist.py,v 1.1 2009/06/02 17:27:34 choutko Exp $

import sys, os, commands
sys.path.insert(0,'/var/www/cgi-bin/mon/lib')
sys.path.insert(0,'./lib')
pair=commands.getstatusoutput("/usr/sue/bin/klist -f")
out=pair[1]
sp1=out.split('FILE:/tmp/krb5')
if(len(sp1)>1):
    sp2=sp1[1].split('\n')
    if(sp2[0].find(')')>=0):
        sp2=sp1[1].split(')')
    fileo='/tmp/krb5'+sp2[0]
    try:
        input=open('/tmp/acrontabklist.log','r')
    except IOError,e:
        print e
        exit
    buf=input.read()
    input.close()
    sp1=buf.split('FILE:/tmp/krb5')
    if(len(sp1)>1):
        sp2=sp1[1].split('\n')
        file='cp /tmp/_krb5'+sp2[0]+' '+fileo
        os.system(file)
    

