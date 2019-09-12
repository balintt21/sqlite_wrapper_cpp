#ifndef COMPONENTS_DATABASE_SQLITE_ERROR_CODE_H_
#define COMPONENTS_DATABASE_SQLITE_ERROR_CODE_H_

namespace database
{
	struct SQLiteCode
	{
		enum Enum
		{
			OK		= 0,
			ERROR	   ,   /* Generic error */
			INTERNAL    ,   /* Internal logic error in SQLite */
			PERM        ,   /* Access permission denied */
			ABORT       ,   /* Callback routine requested an abort */
			BUSY        ,   /* The database file is locked */
			LOCKED      ,   /* A table in the database is locked */
			NOMEM       ,   /* A malloc() failed */
			READONLY    ,   /* Attempt to write a readonly database */
			INTERRUPT   ,   /* Operation terminated by sqlite3_interrupt()*/
			IOERR       ,   /* Some kind of disk I/O error occurred */
			CORRUPT     ,   /* The database disk image is malformed */
			NOTFOUND    ,   /* Unknown opcode in sqlite3_file_control() */
			FULL        ,   /* Insertion failed because database is full */
			CANTOPEN    ,   /* Unable to open the database file */
			PROTOCOL    ,   /* Database lock protocol error */
			EMPTY       ,   /* Internal use only */
			SCHEMA      ,   /* The database schema changed */
			TOOBIG      ,   /* String or BLOB exceeds size limit */
			CONSTRAINT  ,   /* Abort due to constraint violation */
			MISMATCH    ,   /* Data type mismatch */
			MISUSE      ,   /* Library used incorrectly */
			NOLFS       ,   /* Uses OS features not supported on host */
			AUTH        ,   /* Authorization denied */
			FORMAT      ,   /* Not used */
			RANGE       ,   /* 2nd parameter to sqlite3_bind out of range */
			NOTADB      ,   /* File opened that is not a database file */
			NOTICE      ,   /* Notifications from sqlite3_log() */
			WARNING     ,   /* Warnings from sqlite3_log() */
			ROW    = 100,  /* sqlite3_step() has another row ready */
			DONE   = 101  /* sqlite3_step() has finished executing */
		};
	};
}

#endif /* COMPONENTS_DATABASE_SQLITE_ERROR_CODE_H_ */
