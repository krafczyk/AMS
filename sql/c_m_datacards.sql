drop table m_datacard_sets;
create table m_datacard_sets
    ( 
      tinsert   date    not null,
      idx       number(6),
      datacards varchar2(3000)
)
;

