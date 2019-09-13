#include "sqlite.h"
#include <sqlite3.h>

namespace database
{

SQLiteColumn::SQLiteColumn(const SQLiteStmt_sptr& stmt, int32_t col) : mStatement(stmt), mCol(col) {}

bool SQLiteColumn::valid() const noexcept { return (mStatement != nullptr) && (mCol > 0); }

double SQLiteColumn::asDouble() 
{ 
	return sqlite3_column_double(mStatement->native(), mCol); 
}

int32_t SQLiteColumn::asInt() 
{ 
	return sqlite3_column_int(mStatement->native(), mCol); 
}

int64_t SQLiteColumn::asInt64() 
{ 
	return sqlite3_column_int64(mStatement->native(), mCol); 
}

std::string SQLiteColumn::asString() 
{ 
	const uint8_t* data = sqlite3_column_text(mStatement->native(), mCol);
	return std::string(reinterpret_cast<const char*>(data)); 
}

std::wstring SQLiteColumn::asWString() 
{ 
	return L"unsupported"; 
}

SQLiteRow::SQLiteRow(const SQLiteStmt_sptr& stmt) 
	: mStatement(stmt)
	, mColumn(nullptr, 0)
{}

SQLiteColumn& SQLiteRow::operator[]( const size_t index ) noexcept
{
	mColumn = SQLiteColumn(mStatement, index);
	return mColumn;
}

SQLiteStatement::SQLiteStatement(int error_code, sqlite3_stmt* stmt)
	: mErrorCode(static_cast<SQLiteCode::Enum>(error_code))
	, mStatement(stmt)
	, mNextIndex(1)
	, mIsEvaluated(false)
{}

SQLiteStmt_sptr SQLiteStatement::makeShared(int error_code, sqlite3_stmt* stmt)
{
	return std::shared_ptr<SQLiteStatement>(new SQLiteStatement(error_code, stmt));
}

SQLiteStatement::~SQLiteStatement()
{
	if(mStatement != nullptr)
	{ sqlite3_finalize(mStatement); }
}

sqlite3_stmt* SQLiteStatement::native() const
{ return mStatement; }

std::optional<SQLiteRow> SQLiteStatement::step()
{
	if(mIsEvaluated) 
	{ 
		sqlite3_reset(mStatement);
		sqlite3_clear_bindings(mStatement);
		mNextIndex = 1;
	}
	auto error_code = static_cast<SQLiteCode::Enum>(sqlite3_step(mStatement));
	if(error_code != SQLiteCode::ROW)
	{ mIsEvaluated = true; }
	return (error_code == SQLiteCode::ROW) ? std::make_optional<SQLiteRow>(shared_from_this()) : std::nullopt;
}

void SQLiteStatement::evaluate(const std::function<bool (SQLiteRow&)>& on_row_fetched)
{
	if(on_row_fetched != nullptr)
	{
		while(auto opt = step())
		{
			on_row_fetched(opt.value());
		}
	}
}

SQLiteStatement& SQLiteStatement::bind(double value, int32_t index) 
{ 
	if(mStatement != nullptr)
	{
		if(index == NEXT_INDEX) { index = mNextIndex++; }
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_double(mStatement, index, value));
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(int32_t value, int32_t index) 
{ 	
	if(mStatement != nullptr)
	{
		if(index == NEXT_INDEX) { index = mNextIndex++; }
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_int(mStatement, index, value));
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(int64_t value, int32_t index) 
{ 
	if(mStatement != nullptr)
	{
		if(index == NEXT_INDEX) { index = mNextIndex++; }
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_int64(mStatement, index, value)); 
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(const std::string& value, int32_t index)
{
	if(mStatement != nullptr)
	{
		if(index == NEXT_INDEX) { index = mNextIndex++; }
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_text(mStatement, index, value.c_str(), value.size()+1, SQLITE_TRANSIENT)); 
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bindNull(int32_t index) 
{ 
	if(mStatement != nullptr)
	{
		if(index == NEXT_INDEX) { index = mNextIndex++; }
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_null(mStatement, index));
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(double value, const std::string& name) 
{ 
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	return bind(value, index);
}

SQLiteStatement& SQLiteStatement::bind(int32_t value, const std::string& name) 
{
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	return bind(value, index);
}

SQLiteStatement& SQLiteStatement::bind(int64_t value, const std::string& name) 
{
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	return bind(value, index); 
}

SQLiteStatement& SQLiteStatement::bind(const std::string& value, const std::string& name)
{
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	return bind(value, index);
}

SQLiteStatement& SQLiteStatement::bindNull(const std::string& name) 
{
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	return bindNull(index); 
}

SQLite::SQLite(const std::string& path)
	: mHandle(nullptr)
	, mErrorCode(static_cast<SQLiteCode::Enum>(sqlite3_open(path.c_str(), &mHandle)))
{}

SQLite::~SQLite()
{
	if(mHandle != nullptr){ sqlite3_close_v2(mHandle); }
}

bool SQLite::isOpen() const noexcept
{ return mErrorCode == SQLiteCode::OK; }

SQLiteStmt_sptr SQLite::prepare(const std::string& statement)
{
	SQLiteCode::Enum error_code = SQLiteCode::CANTOPEN;
	sqlite3_stmt* stmt = nullptr;
	if(mHandle)
	{
		error_code = static_cast<SQLiteCode::Enum>(sqlite3_prepare_v2(mHandle, statement.c_str(), statement.size()+1, &stmt, nullptr));
	}
	return SQLiteStatement::makeShared(error_code, (error_code == SQLiteCode::OK) ? stmt : nullptr);
}

SQLiteCode::Enum SQLite::execute(const std::string& statement)
{
	SQLiteCode::Enum error_code = SQLiteCode::CANTOPEN;
	if(mHandle)
	{
		sqlite3_stmt* stmt = nullptr;
		error_code = static_cast<SQLiteCode::Enum>(sqlite3_prepare_v2(mHandle, statement.c_str(), statement.size()+1, &stmt, nullptr));
		if(error_code == SQLiteCode::OK)
		{
			error_code = static_cast<SQLiteCode::Enum>(sqlite3_step(stmt));
		}

		if(stmt != nullptr)
		{ sqlite3_finalize(stmt); }
	}
	return error_code;
}

std::vector<std::string> SQLite::listTables()
{
	std::vector<std::string> result;
	auto stmt = prepare("SELECT `name` FROM `sqlite_master` WHERE type = 'table' AND `name` NOT LIKE 'sqlite_%'");
	if(*stmt)
    {
        while(auto opt = stmt->evaluateByRow())
        {
            result.emplace_back(opt.value()[0].asString());
        }
    }
	return result;
}

std::string	SQLite::describeTable(const std::string& table_name)
{
	std::string result;
	auto stmt = prepare("SELECT `sql` FROM `sqlite_master` WHERE `name` LIKE ?");
	if(*stmt)
	{
		if(auto opt = stmt->bind(table_name).evaluateByRow())
		{
			result = opt.value()[0].asString();
		}
	}
	return result;
}

SQLiteCode::Enum SQLite::dropTable(const std::string& table_name)
{
	return execute("DROP TABLE IF EXISTS`" + table_name + "`;");
}

}
