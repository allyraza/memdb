#include <iostream>
#include <sstream>
#include <algorithm>
#include <memory>
#include "memdb.h"

namespace COMMAND {
	std::string UNKNOWN = "UNKNOWN";
	std::string SET = "SET";
	std::string GET = "GET";
	std::string UNSET = "UNSET";
	std::string NUMEQUALTO = "NUMEQUALTO";
	std::string END = "END";

	std::string BEGIN = "BEGIN";
	std::string COMMIT = "COMMIT";
	std::string ROLLBACK = "ROLLBACK";
};

MemDB::MemDB() {}
MemDB::~MemDB() {}

void MemDB::set(std::string key, std::string value) {
	if (key.empty()) {
		return;
	}

	std::string prev;
	if (db.find(key) != db.end()) {
		prev = db[key];
		countMap[key]--;
	} else {
		prev = "NULL";
	}

	if (!transactionStack.empty()) {
		std::stack<Command>& commandStack = transactionStack.top();
		commandStack.push(std::make_tuple(COMMAND::SET, key, prev));
	}

	db[key] = value;
	countMap[value]++;
}

std::string MemDB::get(std::string key) {
	if (key.empty()) {
		return "NULL";
	}

	if (db.find(key) != db.end()) {
		return db[key];
	} else {
		return "NULL";
	}
}

bool MemDB::unset(std::string key){
    if(db.find(key) != db.end()){
        std::string prev = db[key];
        countMap[prev]--;
        if(!transactionStack.empty()){
            std::stack<Command>& commandStack = transactionStack.top();
            commandStack.push(std::make_tuple(COMMAND::SET, key, prev));
        }
    }
    size_t deleted = db.erase(key);
    
    return deleted == 1;
}

int MemDB::numEqualTo(std::string val){
    if (countMap.find(val) != countMap.end()){
        return countMap[val];
    }

    return 0;
}

void MemDB::beginTransaction() {
	CommandStack cs;
	transactionStack.push(cs);
}

bool MemDB::commitTransaction() {
	if (transactionStack.empty()) {
		return false;
	}

	while (!transactionStack.empty()) {
		transactionStack.pop();
	}

	return true;
}

bool MemDB::rollbackTransaction() {
	if (transactionStack.empty()) {
		return false;
	}

	std::stack<Command> commandStack = transactionStack.top();
	transactionStack.pop();

	while (!commandStack.empty()) {
		Command cmd = commandStack.top();
		commandStack.pop();
		this->execute(cmd);
	}

	return true;
}

void MemDB::execute(Command cmd){
    std::string command, key, value;
    std::tie(command, key, value) = cmd;
    
    if (command == COMMAND::SET) {
        if (value == "NULL") {
            this->unset(key);
        } else {
            this->set(key, value);
        }
    } else {
        // will not come here?
    }
}

void MemDB::start() {
    std::cout << "MemDB started!" << std::endl;
    
    std::unique_ptr<MemDB> memDb(new MemDB());
    int stop = false;
    
    while (true) {        
        std::string input;
        while (std::getline(std::cin, input)){
            std::string command = COMMAND::UNKNOWN;
            std::string key = "invalid";
            std::string value = "";
            
            std::istringstream istream(input);
            
            istream >> command;
            
            if (command == COMMAND::SET) {
                istream >> key >> value;
                memDb->set(key, value);
            } else if(command == COMMAND::GET) {
                istream >> key;
                std::string valStr = memDb->get(key);
                std::cout << valStr << std::endl;
            } else if(command == COMMAND::UNSET) {
                istream >> key;
                memDb->unset(key);
            } else if(command == COMMAND::NUMEQUALTO) {
                istream >> value;
                int val = memDb->numEqualTo(value);
                std::cout << val << std::endl;
            } else if(command == COMMAND::END) {
                stop = true;
                break;
            } else if (command == COMMAND::BEGIN) {
                memDb->beginTransaction();
            } else if(command == COMMAND::COMMIT) {
                if(!memDb->commitTransaction()){
                    std::cout << "NO TRANSACTION" << std::endl;
                }
            } else if(command == COMMAND::ROLLBACK) {
                if(!memDb->rollbackTransaction()){
                    std::cout << "NO TRANSACTION" << std::endl;
                }
            } else {
                std::cout << "Unknown command!" << std::endl;
            }

            istream.clear();
            input = "";
        }
        
        if (stop) {
            break;
        }
    }
    
    // delete memDb; 
}


















