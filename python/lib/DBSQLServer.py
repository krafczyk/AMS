import cx_Oracle
import time
import os
class DBSQLServer:
    one=0
    dbinit=0
    dbdriver="Oracle"
    dbfile="pdb_ams"
    dbhandler=""
    dbcursor=""
    def __init__(self,argv,one=0):
        self.one=one
        for x in argv:
            if x[0:2] == "-F": dbfile=x[2:len(x)]
            elif x[0:2] == "-D": dbdriver=x[2:len(x)]
            elif x[0:2] =="-zero":self.one=1
        
    def Connect(self):
        user="amsdes"
        pwd=""
        self.set_oracle_env()
        oracle="/var/www/cgi-bin/mon/lib/.oracle.oracle"
        input=open(oracle,'r')
        pwd=input.read()
        input.close()
        connstring=user+"/"+pwd+"@"+self.dbfile
        try:

            self.dbhandler=cx_Oracle.connect(connstring,twophase=self.one)
            self.dbcursor=self.dbhandler.cursor()
        except cx_Oracle.Error,e:
            print e
            return 0
        return 1
    def set_oracle_env(self):
        if(not (os.environ.has_key('ORACLE_HOME'))):
            os.putenv('ORACLE_HOME','/afs/cern.ch/project/oracle/@sys/prod') 
            os.putenv('TNS_ADMIN','/afs/cern.ch/project/oracle/admin')
    def Query(self,string):
        self.dbcursor.execute(string)
        return self.dbcursor.fetchall()
    def QuerySafe(self,string):
        try:
            self.dbcursor.execute(string)
            return self.dbcursor.fetchall()
        except cx_Oracle.Error,e:
            print e
            return None
    def Update(self,string):
	try:
            return self.dbcursor.execute(string)
        except cx_Oracle.Error,e:
            print e
            time.sleep(2)
            return self.dbcursor.execute(string)
    def Commit(self,commit=1):
        if(commit==1):
            return self.dbhandler.commit()
        else: return self.dbhandler.rollback()
