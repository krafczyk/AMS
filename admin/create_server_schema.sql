--
-- interface Client
--

-- Enums
 
--   enum AccessType{Any,LessThan,Self,AnyButSelf,GreaterThan};+
create table amsdes.senum_accesstype as
(select 0 id, 'Any' value from dual
  union all
 select 1 id, 'LessThan' value from dual
  union all
 select 2 id, 'Self' value from dual
  union all
 select 3 id, 'AnyButSelf' value from dual
  union all
 select 4 id, 'GreaterThan' value from dual
 
);
alter table amsdes.senum_accesstype add CONSTRAINT pk_accesstype PRIMARY KEY (id)
;

--   enum RecordChange{Delete,Update,Create,ClearCreate};
create table amsdes.senum_recordchange as
(select 0 id, 'Delete' value from dual
  union all
 select 1 id, 'Update' value from dual
  union all
 select 2 id, 'Create' value from dual
  union all
 select 3 id, 'ClearCreate' value from dual
);
alter table amsdes.senum_recordchange add CONSTRAINT pk_recordchange PRIMARY KEY (id)
;

--   enum ErrorType{Info,Warning,Error,Severe};

create table amsdes.senum_errortype as
(select 0 id, 'Info' value from dual
  union all
 select 1 id, 'Warning' value from dual
  union all
 select 2 id, 'Error' value from dual
  union all
 select 3 id, 'Severe' value from dual
);
alter table amsdes.senum_errortype add CONSTRAINT pk_errortype PRIMARY KEY (id)
;

   
--   enum ClientStatus{Lost,Unknown,Submitted,Registered,Active,TimeOut,Killed};
create table amsdes.senum_clientstatus as
(select 0 id, 'Lost' value from dual
  union all
 select 1 id, 'Unknown' value from dual
  union all
 select 2 id, 'Submitted' value from dual
  union all
 select 3 id, 'Registered' value from dual
  union all
 select 4 id, 'Active' value from dual
  union all
 select 5 id, 'TimeOut' value from dual
  union all
 select 6 id, 'Killed' value from dual
);
alter table amsdes.senum_clientstatus add CONSTRAINT pk_clientstatus PRIMARY KEY (id)
;


--   enum ClientStatusType{Permanent,OneRunOnly};
create table amsdes.senum_clientstatustype as
(select 0 id, 'Permanent' value from dual
  union all
 select 1 id, 'OneRunOnly' value from dual
);
alter table amsdes.senum_clientstatustype add CONSTRAINT pk_clientstatustype PRIMARY KEY (id)
;

--   enum ClientType{Generic,Server,Consumer,Producer,Monitor,Killer,DBServer,ODBServer,Standalone};
create table amsdes.senum_clienttype as
(select 0 id, 'Generic' value from dual
  union all
 select 1 id, 'Server' value from dual
  union all
 select 2 id, 'Consumer' value from dual
  union all
 select 3 id, 'Producer' value from dual
  union all
 select 4 id, 'Monitor' value from dual
  union all
 select 5 id, 'Killer' value from dual
  union all
 select 6 id, 'DBServer' value from dual
  union all
 select 7 id, 'ODBServer' value from dual
  union all
 select 8 id, 'Standalone' value from dual
);
alter table amsdes.senum_clienttype add CONSTRAINT pk_clienttype PRIMARY KEY (id)
;

--   enum ClientExiting{NOP,CInExit,SInExit,CInAbort,SInAbort,SInKill};
create table amsdes.senum_clientexiting as
(select 0 id, 'NOP' value from dual
  union all
 select 1 id, 'CInExit' value from dual
  union all
 select 2 id, 'SInExit' value from dual
  union all
 select 3 id, 'CInAbort' value from dual
  union all
 select 4 id, 'SInAbort' value from dual
  union all
 select 5 id, 'SInKill' value from dual
);
alter table amsdes.senum_clientexiting add CONSTRAINT pk_clientexiting PRIMARY KEY (id)
;

--   enum HostStatus{NoResponse,InActive,LastClientFailed,OK};
create table amsdes.senum_hoststatus as
(select 0 id, 'NoResponse' value from dual
  union all
 select 1 id, 'InActive' value from dual
  union all
 select 2 id, 'LastClientFailed' value from dual
  union all
 select 3 id, 'OK' value from dual
);
alter table amsdes.senum_hoststatus add CONSTRAINT pk_hoststatus PRIMARY KEY (id)
;


-- Tables

/*
struct CID{
    string HostName;
    unsigned long uid;
    unsigned long pid;
    unsigned long ppid;
    unsigned long threads;
    long threads_change;
    ClientType   Type;
    ClientStatusType StatusType;
    ClientExiting Status;
    string Interface;

    float Mips;
};
*/
create table amsdes.serv_cid (
  id             integer primary key,
  hostname       varchar2(255) not null,
  "UID"          integer not null,
  pid            integer not null,
  ppid           integer,
  threads        integer,
  threads_change integer,
  TYPE           integer,
  statustype     integer,
  status         integer,
  interface      varchar(255),
  mips           float,
  
  CONSTRAINT fk_cid_clienttype FOREIGN KEY (type) REFERENCES  amsdes.senum_clienttype,
  CONSTRAINT fk_cid_clientstatustype FOREIGN KEY (statustype) REFERENCES  amsdes.senum_clientstatustype,
  CONSTRAINT fk_cid_clientexiting FOREIGN KEY (status) REFERENCES  amsdes.senum_clientexiting
  
);

-- create sequence amsdes.seq_serv_cid INCREMENT BY 1 START WITH 10000  NOMAXVALUE NOMINVALUE;

/*
   struct NominalClient{
    unsigned long uid;
    ClientType Type;
    short MaxClients;
    float CPUNeeded; 
    short MemoryNeeded;
    string WholeScriptPath;
    string LogPath;
    string SubmitCommand;
    string HostName;
    short LogInTheEnd;
   }; 
*/
create table amsdes.serv_nominalclient (
  id             integer primary key,
  "UID"          integer not null,
  type           integer,
  maxclients     number(3,0),
  cpuneeded      float,
  memoryneeded   number(3,0),
  WholeScriptPath varchar2(255),
  LogPath         varchar2(255),
  SubmitCommand   varchar2(255),
  HostName        varchar2(255),
  LogInTheEnd     number(3,0),

  CONSTRAINT fk_nominalclient_clienttype FOREIGN KEY (type) REFERENCES  amsdes.senum_clienttype
);
create sequence amsdes.seq_serv_nominalclient INCREMENT BY 1 START WITH 10000  NOMAXVALUE NOMINVALUE;

/*
struct ActiveRef{
  string IOR;
  string Interface;
  ClientType Type;
  unsigned long uid;
};*/

create table amsdes.serv_ActiveRef (
  id             integer primary key,
  IOR            varchar2(255),
  Interface      varchar2(255),
  Type           integer,
  "UID"            integer,

  CONSTRAINT fk_activeref_clienttype FOREIGN KEY (type) REFERENCES  amsdes.senum_clienttype
);
create sequence amsdes.seq_serv_ActiveRef INCREMENT BY 1 START WITH 10000  NOMAXVALUE NOMINVALUE;


/*
   struct NominalHost{
    string HostName;
    string Interface;
    string OS;
    short   CPUNumber;
    short   Memory;
    long    Clock;      
   };
*/
create table amsdes.serv_NominalHost (
  id             integer primary key,
  hostname       varchar2(255),
  Interface      varchar2(255),
  os             varchar2(255),
  CPUNumber      number(3,0),
  Memory         number(3,0),
  clock          integer
);
create sequence amsdes.seq_serv_NominalHost INCREMENT BY 1 START WITH 10000  NOMAXVALUE NOMINVALUE;


/*
struct ActiveHost{
  string HostName;
  string Interface;
  HostStatus Status;
  short ClientsRunning;
  short ClientsAllowed; 
  long ClientsProcessed;
  long ClientsFailed;
  long ClientsKilled;
  time LastFailed;
  time LastUpdate;
  long    Clock;      
};
*/

create table amsdes.serv_ActiveHost (
  id             integer primary key,
  hostname       varchar2(255),
  Interface      varchar2(255),
  status         integer,
  ClientsRunning number(3,0),
  ClientsAllowed number(3,0),
  ClientsProcessed integer,
  ClientsFailed integer,
  ClientsKilled integer,
  LastFailed    integer, --time
  LastUpdate    integer, --time
  clock         integer,

  CONSTRAINT fk_serv_ActiveHost_hoststatus FOREIGN KEY (status) REFERENCES  amsdes.senum_hoststatus
);
create sequence amsdes.seq_serv_ActiveHost INCREMENT BY 1 START WITH 10000  NOMAXVALUE NOMINVALUE;

--
--  interface Producer: Client{
--


--    enum TransferStatus{Begin,Continue,End};
create table amsdes.senum_TransferStatus as
(select 0 id, 'Begin' value from dual
  union all
 select 1 id, 'Continue' value from dual
  union all
 select 2 id, 'End' value from dual
);
alter table amsdes.senum_TransferStatus add CONSTRAINT pk_ PRIMARY KEY (id)
;

--    enum RunStatus{ToBeRerun, Failed,Processing, Finished, Unknown, Allocated,Foreign,Canceled};
create table amsdes.senum_RunStatus as
(select 0 id, 'ToBeRerun' value from dual
  union all
 select 1 id, 'Failed' value from dual
  union all
 select 2 id, 'Processing' value from dual
  union all
 select 3 id, 'Finished' value from dual
  union all
 select 4 id, 'Unknown' value from dual
  union all
 select 5 id, 'Allocated' value from dual
  union all
 select 6 id, 'Foreign' value from dual
  union all
 select 7 id, 'Canceled' value from dual
);
alter table amsdes.senum_RunStatus add CONSTRAINT pk_RunStatus PRIMARY KEY (id)
;

--    enum DSTStatus{Success,InProgress,Failure,Validated};
create table amsdes.senum_DSTStatus as
(select 0 id, 'Success' value from dual
  union all
 select 1 id, 'InProgress' value from dual
  union all
 select 2 id, 'Failure' value from dual
  union all
 select 3 id, 'Validated' value from dual
);
alter table amsdes.senum_DSTStatus add CONSTRAINT pk_DSTStatus PRIMARY KEY (id)
;

--    enum RunMode{RILO,LILO,RIRO,LIRO};
create table amsdes.senum_RunMode as
(select 0 id, 'RILO' value from dual
  union all
 select 1 id, 'LILO' value from dual
  union all
 select 2 id, 'RIRO' value from dual
  union all
 select 3 id, 'LIRO' value from dual
);
alter table amsdes.senum_RunMode add CONSTRAINT pk_RunMode PRIMARY KEY (id)
;

--    enum DSTType{Ntuple,EventTag,RootFile,RawFile};
create table amsdes.senum_DSTType as
(select 0 id, 'Ntuple' value from dual
  union all
 select 1 id, 'EventTag' value from dual
  union all
 select 2 id, 'RootFile' value from dual
  union all
 select 3 id, 'RawFile' value from dual
);
alter table amsdes.senum_DSTType add CONSTRAINT pk_DSTType PRIMARY KEY (id)
;



/*
    struct TDVTableEntry{
     unsigned long id;
     time Insert;
     time Begin;
     time End;
    };
*/
create table amsdes.senum_TDVTableEntry (
  id             integer primary key,
  "INSERT"       integer,
  begin          integer, --time
  end            integer --time
);


/*    
struct TDVName{
     string Name;
     unsigned long Size;
     unsigned long CRC;
     TDVTableEntry Entry;
     long DataMC;
     boolean Success;
};  
*/
create table amsdes.serv_TDVName (
  id             integer primary key,
  name           varchar2(255),
  "SIZE"           integer,
  CRC            integer, 
  Entry          integer,
  DataMC         integer, 
  Success        varchar2(5), -- boolean
  
  CONSTRAINT fk_serv_TDVName_TDVTableEntry FOREIGN KEY (entry) REFERENCES  amsdes.senum_TDVTableEntry
 
);

/*   struct CurrentInfo{
      unsigned long Run;
      long EventsProcessed;     
      long LastEventProcessed;
      long CriticalErrorsFound;
      long ErrorsFound;
      float CPUTimeSpent;
      float TimeSpent;
      float Mips;
      float CPUMipsTimeSpent;
      RunStatus Status;
      string HostName;
    }; 
*/
create table amsdes.serv_CurrentInfo (
  id                  integer primary key,
  run                 integer,
  EventsProcessed     integer,
  LastEventProcessed  integer,
  CriticalErrorsFound integer,
  ErrorsFound         integer,
  CPUTimeSpent        float,
  TimeSpent           float,
  Mips                float,
  CPUMipsTimeSpent    float,
  Status              integer,
  
  CONSTRAINT fk_serv_CurrentInfo_RunStatus FOREIGN KEY (status) REFERENCES  amsdes.senum_RunStatus
 
);

/*
    struct DSTInfo{
     DSTType type;
     unsigned long uid;
     string HostName;
     string OutputDirPath;
     RunMode Mode;
     long UpdateFreq;
     long DieHard;
     long FreeSpace;
     long TotalSpace;
    };
*/
create table amsdes.serv_DSTInfo (
  id                  integer primary key,
  type                integer,
  "UID"               integer,
  hostname            varchar2(255),
  OutputDirPath       varchar2(255),
  "MODE"              integer,
  UpdateFreq          integer,
  DieHard             integer,
  FreeSpace           integer,
  TotalSpace          integer,
  
  CONSTRAINT fk_serv_DSTInfo_DSTType FOREIGN KEY ("MODE") REFERENCES  amsdes.senum_DSTType,
  CONSTRAINT fk_serv_DSTInfo_RunMode FOREIGN KEY (type) REFERENCES  amsdes.senum_RunMode
);

/*
     struct RunEvInfo{
     unsigned long uid;
     unsigned long Run;
     long FirstEvent;
     long LastEvent;
     time TFEvent;
     time TLEvent; 
     long Priority;
     string FilePath;
     RunStatus Status;
     RunStatus History;
     unsigned long      CounterFail;
     time SubmitTime;
     unsigned long cuid;
     unsigned long rndm1;
     unsigned long rndm2;
     long DataMC;
     CurrentInfo cinfo;     
    };
*/
create table amsdes.serv_RunEvInfo (
  id                  integer primary key,
  "UID"               integer,
  run                 integer,
  FirstEvent          integer,
  LastEvent           integer,
  Priority            integer,
  filepath            varchar2(255),
  Status              integer,
  History             integer,
  CounterFail         integer,
  SubmitTime          integer, -- time
  cuid                integer,
  rndm1               integer,
  rndm2               integer,
  DataMC              integer,
  cinfo               integer,
  
  CONSTRAINT fk_serv_RunEvInfo_RunStatus FOREIGN KEY (status) REFERENCES  amsdes.senum_RunStatus,
  CONSTRAINT fk_serv_RunEvInfo_RunStatus1 FOREIGN KEY (history) REFERENCES  amsdes.senum_RunStatus,
  CONSTRAINT fk_serv_RunEvInfo_CurrentInfo FOREIGN KEY (cinfo) REFERENCES  amsdes.serv_CurrentInfo
);




/*
    struct DST{
     time   Insert;
     time   Begin;
     time   End;
     long   Run;
     long   FirstEvent;
     long   LastEvent;
     long   EventNumber;
     long   ErrorNumber;
     DSTStatus Status;
     DSTType Type;
     long   size;
     string Name;
     string Version;
     unsigned long crc;
     unsigned long rndm1;
     unsigned long rndm2;
     long FreeSpace;
     long TotalSpace;

    };    
*/
create table amsdes.serv_DST (
  id                  integer primary key,
  "INSERT"            integer, --time
  Begin               integer, --time
  End                 integer, --time
  Run                 integer,
  FirstEvent          integer,
  LastEvent           integer,
  EventNumber         integer,
  ErrorNumber         integer,
  Status              integer,
  Type                integer,
  "SIZE"              integer,
  Name                varchar2(255),
  Version             varchar2(255),
  cuid                integer,
  rndm1               integer,
  rndm2               integer,
  FreeSpace           integer,
  TotalSpace          integer,
  
  CONSTRAINT fk_serv_DST_DSTStatus FOREIGN KEY (Status) REFERENCES  amsdes.senum_DSTStatus,
  CONSTRAINT fk_serv_DST_DSTType FOREIGN KEY (Type) REFERENCES  amsdes.senum_DSTType
);

/*
    struct FPath{
     string fname;
     unsigned long pos;
    };  
*/
create table amsdes.serv_FPath (
  id                  integer primary key,
  fname               varchar2(255),
  pos                 integer
);


/*
interface Server: Client{
*/


--    enum OpType{StartClient,KillClient,CheckClient,ClearStartClient,ClearKillClient,ClearCheckClient};
create table amsdes.senum_OpType as
(select 0 id, 'StartClient' value from dual
  union all
 select 1 id, 'KillClient' value from dual
  union all
 select 2 id, 'CheckClient' value from dual
  union all
 select 3 id, 'ClearStartClient' value from dual
  union all
 select 4 id, 'ClearKillClient' value from dual
  union all
 select 5 id, 'ClearCheckClient' value from dual 
);
alter table amsdes.senum_OpType add CONSTRAINT pk_OpType PRIMARY KEY (id)
;



/*    struct CriticalOps{
     time TimeOut;
     OpType Action;
     time TimeStamp;
     ClientType Type;
     unsigned long id; 
    };
*/
create table amsdes.serv_CriticalOps (
  id                  integer primary key,
  TimeOut             integer, --time
  Action              integer, 
  TimeStamp           integer, --time
  Type                integer,

  CONSTRAINT fk_serv_CriticalOps_OpType FOREIGN KEY (Action) REFERENCES  amsdes.senum_OpType,
  CONSTRAINT fk_serv_CriticalOps_ClientType FOREIGN KEY (Type) REFERENCES  amsdes.senum_ClientType
);


/*
    struct DB{
     string fs;
     float dbfree;
     float dbtotal;
     float bs;
    };

*/

create table amsdes.serv_DB (
  id                  integer primary key,
  fs                  varchar2(255),
  dbfree              float,
  dbtotal             float,
  bs                  float
);
