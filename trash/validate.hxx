#pragma once
#ifndef VALIDATE_HXX
#define VALIDATE_HXX

#include "common/database.hxx"
#include "validate_args.hxx"

namespace trash{

class Validate
{
public:
	Validate(ValidateOptions& vOpt);

private:
	Database m_db;

	ValidateOptions& m_vOpt;
	
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

	
}//namespace trash

#endif
