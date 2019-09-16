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

	class SQLiteColumn
	{
		SQLiteStmt_sptr mStatement;
		int32_t			mCol;
	public:
		SQLiteColumn(const SQLiteStmt_sptr& stmt, int32_t col);
		bool valid() const noexcept;
		explicit operator bool() const noexcept { return valid(); }
		double asDouble();
		int32_t asInt();
		int64_t asInt64();
		std::string asString();
		std::wstring asWString();//unsupported
	};

	class SQLiteRow
	{
		SQLiteStmt_sptr mStatement;
		SQLiteColumn	mColumn;
	public:
		SQLiteRow(const SQLiteStmt_sptr& stmt);
		SQLiteColumn& operator[]( const size_t index ) noexcept;
	};

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
		SQLiteStatement(const SQLiteStatement& other) = delete;
		SQLiteStatement(SQLiteStatement&& other) = delete;
		~SQLiteStatement();
		/**
		 * Returns the actual error code
		 */
		inline SQLiteCode::Enum errorCode() const { return mErrorCode; }
		inline bool valid() const { return errorCode() == SQLiteCode::OK; }
		explicit operator bool() const noexcept { return valid(); }
		/**
		 * Returns the native statement handler
		 */
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
		 * Execute statement without fetching results
		 */
		SQLiteCode::Enum execute();
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
		 * @return On success a valid SQLiteStatement is returned, otherwise an invalid SQliteStatement containing a proper error code
		 */
		SQLiteStmt_sptr prepare(const std::string& statement);
		/**
		 * Executes the given statement
		 * Good for action statements without fetchable result
		 * @return An SQLiteCode is returned
		 */
		SQLiteCode::Enum execute(const std::string& statement);
		//higher level functions
		/**
		 * List table names
		 */
		std::vector<std::string> listTables();
		/**
		 * Describe table
		 * return An sql string is returned on success as description, otherwise an empty string
		 */
		std::string	describeTable(const std::string& table_name);
		/**
		 * Drop table
		 * @return An SQLiteCode is returned
		 */	 
		SQLiteCode::Enum dropTable(const std::string& table_name);

		
	private:
		sqlite3 * mHandle;
		const SQLiteCode::Enum mErrorCode;
	};
}

#endif /* COMPONENTS_DATABASE_SQLITE_SQLITE_H_ */
