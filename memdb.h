#ifndef MEMDB_H
#define MEMDB_H

#include <string>
#include <tuple>
#include <unordered_map>
#include <stack>

namespace COMMAND {
	extern std::string UNKNOWN;
	extern std::string SET;
	extern std::string GET;
	extern std::string UNSET;
	extern std::string NUMEQUALTO;
	extern std::string END;

	extern std::string BEGIN;
	extern std::string COMMIT;
	extern std::string ROLLBACK;
};

class MemDB {
public:
	typedef std::tuple<std::string, std::string, std::string> Command;
	typedef std::stack<Command> CommandStack;

	MemDB();
	~MemDB();

	void set(std::string key, std::string value);
	std::string get(std::string key);
	bool unset(std::string key);
	int numEqualTo(std::string value);

	void beginTransaction();
	bool commitTransaction();
	bool rollbackTransaction();

	static void start();

private:
	std::unordered_map<std::string, std::string> db;
	std::unordered_map<std::string, int> countMap;
	std::stack<CommandStack> transactionStack;

private:
	void execute(Command command);
};


#endif /* MEMDB_H */