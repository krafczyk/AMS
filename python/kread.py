#!/usr/bin/env python2.4
#  $Id: kread.py,v 1.2 2009/06/02 17:00:21 choutko Exp $

import sys, os, commands
sys.path.insert(0,'/var/www/cgi-bin/mon/lib')
sys.path.insert(0,'./lib')
pair=commands.getstatusoutput("klist -f")
out=pair[1]
sp1=out.split('FILE:/tmp/krb5')
if(len(sp1)>1):
    sp2=sp1[1].split('\n')
    if(sp2[0].find(')')>=0):
        sp2=sp1[1].split(')')
    fileo='/tmp/krb5'+sp2[0]
    fileon='/tmp/_krb5'+sp2[0]
    file='cp '+fileo+' '+fileon
    os.system(file)
    os.system("klist -f >/tmp/acrontabklist.log")    

