#ifndef COMPONENTS_DATABASE_SQLITE_SQLITE_H_
#define COMPONENTS_DATABASE_SQLITE_SQLITE_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "sqlite_error_code.h"
//pre-declarations
struct sqlite3;
struct sqlite3_stmt;

namespace database
{
	class SQLiteStatement
	{
	private:
		SQLiteCode::Enum 	mErrorCode;
		sqlite3_stmt* 		mStatement;
		int32_t				mNextIndex;
	public:
		static constexpr int32_t NEXT_INDEX = 0;
		SQLiteStatement(int error_code, sqlite3_stmt* stmt);
		~SQLiteStatement();
		inline SQLiteCode::Enum errorCode() const { return mErrorCode; }
		explicit operator bool() const noexcept { return errorCode() == SQLiteCode::OK; }

		SQLiteStatement& bind(double value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bind(int32_t value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bind(int64_t value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bind(const std::string& value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bindNull(int32_t index = NEXT_INDEX);
		
		SQLiteStatement& bind(double value, const std::string& name);
		SQLiteStatement& bind(int32_t value, const std::string& name);
		SQLiteStatement& bind(int64_t value, const std::string& name);
		SQLiteStatement& bind(const std::string& value, const std::string& name);
		SQLiteStatement& bindNull(const std::string& name);

		bool step();
	};
	using SQLiteStmt_sptr = std::shared_ptr<SQLiteStatement>;

	class SQLite
	{
	public:
		SQLite(const std::string& path);
		virtual ~SQLite();
		//checkers
		bool isOpen() const noexcept;
		explicit operator bool() const noexcept { return isOpen(); }
		//members functions
		/**
		 *
		 */
		SQLiteStmt_sptr prepare(const std::string& statement);
		/**
		 *
		 */
		bool exec(const std::string& statement, const std::function<void (int32_t, char**, char**)>& yield_row);
		
	private:
		sqlite3 * mHandle;
		const SQLiteCode::Enum mErrorCode;
	};
}

#endif /* COMPONENTS_DATABASE_SQLITE_SQLITE_H_ */
