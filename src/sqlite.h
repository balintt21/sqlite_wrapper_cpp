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
	class SQLiteStatement;
	using SQLiteStmt_sptr = std::shared_ptr<SQLiteStatement>;

	class SQLiteRow
	{
		SQLiteStmt_sptr mStatement;
	public:
		SQLiteRow(const SQLiteStmt_sptr& stmt);
	};
	using SQLiteRow_sptr = std::shared_ptr<SQLiteRow>;

	class SQLiteStatement : public std::enable_shared_from_this<SQLiteStatement>
	{
	private:
		SQLiteCode::Enum 	mErrorCode;
		sqlite3_stmt* 		mStatement;
		int32_t				mNextIndex;
		bool				mIsEvaluated;
		SQLiteStatement(int error_code, sqlite3_stmt* stmt);
	public:
		static constexpr int32_t NEXT_INDEX = 0;
		static SQLiteStmt_sptr makeShared(int error_code, sqlite3_stmt* stmt);
		~SQLiteStatement();
		inline SQLiteCode::Enum errorCode() const { return mErrorCode; }
		explicit operator bool() const noexcept { return errorCode() == SQLiteCode::OK; }
		sqlite3_stmt* native() const; 
		/**
		 * Evaluates statement by one row
		 * @return Optionally an SQLiteRow is returned if fetched successfully, otherwise nullopt is returned
		 */
		std::optional<SQLiteRow> step();
		/**
		 * Alias for step
		 */
		inline std::optional<SQLiteRow> evaluateByRow() { return step(); }
		/**
		 * Evaluates statement and calls the given callback for each time a row is available
		 */
		void evaluate(const std::function<bool (SQLiteRow&)>& on_row_fetched);
		/**
		 * Bind functions for adding/changing data to/of the prepared statement
		 */
		//bind by index ?NNN | ?
		SQLiteStatement& bind(double value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bind(int32_t value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bind(int64_t value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bind(const std::string& value, int32_t index = NEXT_INDEX);
		SQLiteStatement& bindNull(int32_t index = NEXT_INDEX);
		//bind by name ?AAAA
		SQLiteStatement& bind(double value, const std::string& name);
		SQLiteStatement& bind(int32_t value, const std::string& name);
		SQLiteStatement& bind(int64_t value, const std::string& name);
		SQLiteStatement& bind(const std::string& value, const std::string& name);
		SQLiteStatement& bindNull(const std::string& name);
	};

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
		 * Prepare an sql statement for further use
		 * @return On success a valid SQLiteStmt_sptr is returned, otherwise a nullptr
		 */
		SQLiteStmt_sptr prepare(const std::string& statement);
		/**
		 * Executes the given statement
		 * Good for action statements without fetchable result
		 * @return 
		 */
		SQLiteCode::Enum execute(const std::string& statement);
		
	private:
		sqlite3 * mHandle;
		const SQLiteCode::Enum mErrorCode;
	};
}

#endif /* COMPONENTS_DATABASE_SQLITE_SQLITE_H_ */
