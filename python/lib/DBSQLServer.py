import cx_Oracle
import time
import os
import re
import traceback

class DBSQLServer:
    one=0
    dbinit=0
    dbdriver="Oracle"
    dbfile="pdb_ams"
    dbhandler=""
    dbcursor=""
    last_insert_id=None
    logger = None
    last_sql = None
    last_sql_arguments = None

    def __init__(self, argv, one=0, logger=None):
        self.one=one
        self.logger = logger
        for x in argv:
            if x[0:2] == "-F": self.dbfile = x[2:len(x)]
            elif x[0:2] == "-D": self.dbdriver = x[2:len(x)]
            elif x[0:2] =="-zero": self.one = 1

    def Connect(self):
        user="amsdes"
        oracle="/var/www/cgi-bin/mon/lib/.oracle.oracle"
        input=open(oracle,'r')
        pwd=input.read()
        input.close()
        connstring = "%s/%s@%s" % (user, pwd.replace("\n", ""), self.dbfile)
        return self.__connect(connstring)

    def ConnectRO(self):
        user="amsro"
        pwd="eto_amsro"
        connstring = "%s/%s@%s" % (user, pwd, self.dbfile)
        return self.__connect(connstring)

    def __connect(self, connstring):
        self.__set_oracle_env()

        if self.logger is not None:
            self.logger.debug("oldsql(Connect) to %s", self.dbfile)

        try:
            self.dbhandler = cx_Oracle.connect(connstring, twophase=self.one)
            self.dbcursor = self.dbhandler.cursor()
        except cx_Oracle.Error, e:
            print e
            return 0
        return 1

    def __set_oracle_env(self):
        if(os.environ.has_key('NLS_LANG')):
            del os.environ['NLS_LANG']
        if(not (os.environ.has_key('ORACLE_HOME'))):
            os.putenv('ORACLE_HOME','/afs/cern.ch/project/oracle/@sys/10205') 
            os.putenv('TNS_ADMIN','/afs/cern.ch/project/oracle/admin')

    def Query(self,string):
        if self.logger is not None:
            self.logger.debug("oldsql(Query)='%s', host=%s", string, self.dbhandler.dsn)

        self.dbcursor.execute(string)
        return self.dbcursor.fetchall()

    def QuerySafe(self,string):
        if self.logger is not None:
            self.logger.debug("oldsql(QuerySafe)='%s', host=%s", string, self.dbhandler.dsn)

        try:
            self.dbcursor.execute(string)
            return self.dbcursor.fetchall()
        except cx_Oracle.Error,e:
            print e
            return None

    def Update(self,string):
        if self.logger is not None:
            self.logger.debug("oldsql(Update)='%s', host=%s", string, self.dbhandler.dsn)

	try:
            return self.dbcursor.execute(string)
        except cx_Oracle.Error,e:
            print e
            time.sleep(2)
            return self.dbcursor.execute(string)

    def Commit(self,commit=1):
        if(commit==1):
            if self.logger is not None:
                self.logger.debug("sql='COMMIT', host=%s", self.dbhandler.dsn)
            return self.dbhandler.commit()
        else:
            if self.logger is not None:
                self.logger.debug("sql='ROLLBACK', host=%s", self.dbhandler.dsn)
            return self.dbhandler.rollback()


    def sql(self, sql, **kwargs):
        if self.dbcursor == "":
            self.Connect()

        '''
        last_insert_id = None
        insert = re.compile('^\s*INSERT\s*')
        if insert.match(sql):
            last_insert_id = self.dbcursor.var(int)
            kwargs['returning_id'] = last_insert_id
            sql = "%s %s" % (sql, " RETURNING ID INTO :returning_id")
        '''

        self.dbcursor.prepare(sql)
        result = self.dbcursor.execute(None, kwargs)

        self.last_sql = sql
        self.last_sql_arguments = kwargs

        commit = re.compile('^\s*(COMMIT|ROLLBACK)')
        if commit.match(sql):
            if self.logger is not None:
                self.logger.debug("sql='%s', args=%s, host=%s", sql, kwargs, self.dbhandler.dsn)
        else:
            edit = re.compile('^\s*(UPDATE |DELETE |INSERT )')
            if edit.match(sql):
                if self.logger is not None:
                    self.logger.debug("sql='%s', args=%s, host=%s, rows=%d", sql, kwargs, self.dbhandler.dsn, self.dbcursor.rowcount)
            else:
                result = self.dbcursor.fetchall()
                if self.logger is not None:
                    self.logger.debug("sql='%s', args=%s, host=%s, rows=%d", sql, kwargs, self.dbhandler.dsn, self.dbcursor.rowcount)

        '''
        if last_insert_id is not None:
            self.last_insert_id = last_insert_id.getvalue()
        '''

        return result
