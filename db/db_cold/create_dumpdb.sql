PRAGMA page_size=4096;
CREATE TABLE "dumpdb" (
   "NPONTO" Integer Primary Key NOT NULL,
   "ID" Text,
   "DESCRICAO" Text,
   "VALOR" Real,
   "FLAGS" Integer,
   "LIMS" Real,
   "LIMI" Real,
   "HISTER" Real,
   "DATA" Integer,
   "HORA" Integer,
   "TS" Integer,
   "ALRIN" Integer,
   "VLRINI" Real,
   "HISTOR" Integer,
   "BMORTA" Real,
   "PERIODO" Integer,
   "TIPO" Text,
   "UNIDADE" Text,
   "ESTON" Text,
   "ESTOFF" Text,
   "SUPCMD" Integer,
   "TIMEOUT" Integer,
   "ANOTACAO" Text,
   "ESTACAO" text,
   "ESTALM" integer,
   "PRIOR" integer 
   );
CREATE UNIQUE INDEX ind_id on dumpdb ( ID );
CREATE INDEX ind_estacao on dumpdb ( ESTACAO );
CREATE INDEX ind_tipo on dumpdb ( TIPO );
CREATE INDEX ind_supcmd on dumpdb ( SUPCMD );
CREATE INDEX ind_prior on dumpdb ( PRIOR );
