#include "sqlite.h"
#include <sqlite3.h>

namespace database
{

SQLiteStatement::SQLiteStatement(int error_code, sqlite3_stmt* stmt)
	: mErrorCode(static_cast<SQLiteCode::Enum>(error_code))
	, mStatement(stmt)
	, mNextIndex(1)
{}
SQLiteStatement::~SQLiteStatement()
{
	if(mStatement != nullptr)
	{ sqlite3_finalize(mStatement); }
}

SQLiteStatement& SQLiteStatement::bind(double value, int32_t index) 
{ 
	if(index == NEXT_INDEX) { index = mNextIndex++; }
	mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_double(mStatement, index, value)); 
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(int32_t value, int32_t index) 
{ 
	if(index == NEXT_INDEX) { index = mNextIndex++; }
	mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_int(mStatement, index, value)); 
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(int64_t value, int32_t index) 
{ 
	if(index == NEXT_INDEX) { index = mNextIndex++; }
	mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_int64(mStatement, index, value)); 
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(const std::string& value, int32_t index)
{
	if(index == NEXT_INDEX) { index = mNextIndex++; }
	mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_text(mStatement, index, value.c_str(), value.size()+1, SQLITE_TRANSIENT)); 
	return *this; 
}

SQLiteStatement& SQLiteStatement::bindNull(int32_t index) 
{ 
	if(index == NEXT_INDEX) { index = mNextIndex++; }
	mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_null(mStatement, index));
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(double value, const std::string& name) 
{ 
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	else
	{
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_double(mStatement, index, value));
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(int32_t value, const std::string& name) 
{ 
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	else
	{
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_int(mStatement, index, value)); 
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(int64_t value, const std::string& name) 
{ 
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	else
	{
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_int64(mStatement, index, value)); 	
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bind(const std::string& value, const std::string& name)
{
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	else
	{
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_text(mStatement, index, value.c_str(), value.size()+1, SQLITE_TRANSIENT)); 	
	}
	return *this; 
}

SQLiteStatement& SQLiteStatement::bindNull(const std::string& name) 
{ 
	int32_t index = sqlite3_bind_parameter_index(mStatement, name.c_str());
	if(index == 0) { mErrorCode = SQLiteCode::NOTFOUND; }
	else
	{
		mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_bind_null(mStatement, index)); 
	}
	return *this; 
}

SQLite::SQLite(const std::string& path)
	: mHandle(nullptr)
	, mErrorCode(SQLiteCode::CANTOPEN)
{
	mErrorCode = static_cast<SQLiteCode::Enum>(sqlite3_open(path.c_str(), &mHandle));
}

SQLite::~SQLite()
{
	if(mHandle != nullptr){ sqlite3_close(mHandle); }
}

bool SQLite::isOpen() const noexcept
{ return mErrorCode == SQLiteCode::OK; }

}

SQLiteStmt_sptr SQLite::prepare(const std::string& statement)
{

}
