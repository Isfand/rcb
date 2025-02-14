#pragma once
#ifndef VALIDATE_HXX
#define VALIDATE_HXX

#include "common/database.hxx"
#include "validate_args.hxx"

namespace rcb{

class Validate
{
public:
	Validate(const ValidateOptions& vOpt);

private:
	const ValidateOptions& m_vOpt;
	Database m_db;
	
	//Unused
	void allFile();

	//removes dangling files in file/ by checking data/.
	void file();
	//removes dangling & duplicate records in data/ by checking file/.
	void data();
	//removes dangling files in wipe/.
	void wipe();
	//evaluate size:NULL of filetype:directory
	void fillDirectorySize();
};

	
}//namespace rcb

#endif
