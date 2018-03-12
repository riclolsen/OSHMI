PRAGMA page_size=4096;
CREATE TABLE "soe" (
   "NPONTO" Integer NOT NULL,
   "DATA" Integer NOT NULL,
   "HORA" Integer NOT NULL,
   "MSEC" Integer NOT NULL,
   "UTR" Integer NOT NULL,
   "ESTADO" Integer NOT NULL,
   "RECON" Integer NOT NULL,
   "TS" Integer  NOT NULL,
   PRIMARY KEY ( NPONTO, DATA, HORA, MSEC )
   );
CREATE INDEX ind_dhms on soe ( DATA, HORA, MSEC );
CREATE INDEX ind_ts on soe ( TS );
CREATE INDEX ind_recon on soe ( RECON );