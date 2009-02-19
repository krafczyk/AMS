set echo off;

---------------------------------
-- New Production Set creation --
---------------------------------

--
-- Close old production set
--

update productionset set end=&&unix_timestart - 1 
where status='Active' and name like '%20__A%';

update productionset set status='Finished'
where status='Active' and name like '%20__A%';

-- Output the last A production set
select * from productionset where did = (select max(did) from productionset where name like '%A'); 

--
-- Open new production set
--

-- Data, precious data...
insert into productionset values (
      (select max(did)+1 
         from productionset), -- unique ID 
      'AMS02/&&year.A',          -- name
      &&unix_timestart,             -- begin
      0,                      -- end
      'Active',               -- status
      'v4.00/build&&build./os2',   -- version. For the build number check with <AMS_CVS_ROOT>/CC/commonsi.C
      '&&period_description', -- description
      '&&build',                  -- same old build number. Shorter though
      'v4.00',                -- constant ATM
      'os2'                   -- Looks like it's OS/2. Is it a joke? o_O
);

-- CHECK THE TABLE. MAKE SURE EVERYTHING IS OK AT THIS POINT!!!

select * from productionset where name like '&&year';

-- You may want to check the table again before executing the next line

--rollback; -- for test purposes only

COMMIT;

--
-- Create snapshots
--

-- As SYSDBA

CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_CASTORNTUPLES" ("TOTALFILES", "SIZEMB") AS 
SELECT COUNT(ntuples.run), 
       SUM(ntuples.SIZEMB) 
  FROM ntuples  
 WHERE ntuples.path LIKE '%&&year.A%' 
   AND ntuples.castortime != 0 
;
             

CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_ENDRUNS" ("TOTALRUNS", "SUMFEVENT", "SUMLEVENT") AS  
SELECT COUNT(Runs.jid), 
       SUM(Runs.fevent), 
       SUM(Runs.levent)  
  FROM Jobs, Runs
 WHERE Runs.jid=Jobs.jid 
   AND (Runs.status='Finished' OR Runs.status='Completed')
   AND Jobs.pid=(select max(did) from productionset where name like '%&&year.A%')  
   AND Jobs.cid != 2 
;

CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_FAILEDRUNS" ("TOTALRUNS") AS 
SELECT COUNT(jid) 
  FROM Jobs_deleted
 WHERE pid=(select max(did) from productionset where name like '%&&year.A%') 
   AND cid != 2 ;
   
CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_JOBS" ("FIRSTJOBTIME", "LASTJOBTIME", "TOTALJOBS", "TOTALTRIGGERS") AS  
SELECT MIN(Jobs.time), 
       MAX(Jobs.timestamp), 
       Count(jobs.jid), 
       SUM(Jobs.realtriggers)
 FROM Jobs 
WHERE Jobs.pid=(select max(did) from productionset where name like '%&&year.A%') 
  and jobs.realtriggers>0 
;

CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_JOBS_A" ("FIRSTJOBTIME", "LASTJOBTIME", "TOTALJOBS", "TOTALTRIGGERS") AS 
SELECT MIN(Jobs.time), 
       MAX(Jobs.timestamp), 
       Count(jobs.jid), 
       SUM(Jobs.triggers)
  FROM Jobs 
 WHERE Jobs.pid=(select max(did) from productionset where name like '%&&year.A%')   
 ; 
 
CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_NTUPLES" ("TOTALFILES", "SIZEMB") AS 
SELECT COUNT(ntuples.run), 
       SUM(ntuples.SIZEMB) 
  FROM ntuples  
WHERE ntuples.path LIKE '%&&year.A%'  
;


CREATE OR REPLACE FORCE VIEW "AMSDES"."STAT_&&year.A_TIMEOUTRUNS" ("TOTALRUNS") AS  
SELECT COUNT(jid) 
  FROM jobs 
 WHERE timekill>0 
   and pid=(select max(did) from productionset where name like '%&&year.A%') 
;

-- Check views existence
select view_name "VIEWS CREATED" from user_views where view_name like '%&&year.A%'


rollback;

exit;
