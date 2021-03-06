drop table m_tags;
create table m_tags
    ( run        number(24,0)     NOT NULL,
      idx        number(24,0)     NOT NULL,
      event      number(24,0)     not null,
      position   number(24,0),
      tag1       number(24,0),
      tag2       number(24,0)
    )     
      PCTFREE 1 PCTUSED 99
      STORAGE (INITIAL 100M NEXT 100M PCTINCREASE 0 )
      nologging
      PARTITION BY RANGE (RUN) 
       (PARTITION day_0   VALUES LESS THAN (896824800),
        PARTITION day_1   VALUES LESS THAN (896911200),
        PARTITION day_2   VALUES LESS THAN (896997600),
        PARTITION day_3   VALUES LESS THAN (897084000),
        PARTITION day_4   VALUES LESS THAN (897170400),
        PARTITION day_5   VALUES LESS THAN (897256800),
        PARTITION day_6   VALUES LESS THAN (897343200),
        PARTITION day_7   VALUES LESS THAN (897429600),
        PARTITION day_8   VALUES LESS THAN (897516000),
        PARTITION day_9   VALUES LESS THAN (897602400),
        PARTITION day_10  VALUES LESS THAN (897688800),
        PARTITION year_1998  VALUES LESS THAN (915145201),
        PARTITION year_1999  VALUES LESS THAN (978303601),
        PARTITION year_2000  VALUES LESS THAN (1009839601),
        PARTITION year_2001  VALUES LESS THAN (1041462001),
        PARTITION year_2002  VALUES LESS THAN (1072911601),
        PARTITION year_2003  VALUES LESS THAN (1104534001),
        PARTITION year_2004  VALUES LESS THAN (1136070001),
        PARTITION year_2005  VALUES LESS THAN (1167606001),
        PARTITION year_2006  VALUES LESS THAN (1199142001),
        PARTITION year_2007  VALUES LESS THAN (1230764401)
        );
