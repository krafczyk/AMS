import cx_Oracle
class DBSQLServer:
    one=0
    dbinit=0
    dbdriver="Oracle"
    dbfile="amsdb"
    dbhandler=""
    dbcursor=""
    def __init__(self,argv):
        for x in argv:
            if x[0:2] == "-F": dbfile=x[2:len(x)]
            elif x[0:2] == "-D": dbdriver=x[2:len(x)]
            elif x[0:2] =="-zero":one=1

    def Connect(self):
        user="amsdes"
        pwd=""
#        self.set_oracle_env()
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
    def Query(self,string):
        self.dbcursor.execute(string)
        return self.dbcursor.fetchall()
    def Update(self,string):
        return self.dbcursor.execute(string)
    def Commit(self):
        return self.dbhandler.commit()
    def RollBack(self):
        return self.dbhandler.rollback()
