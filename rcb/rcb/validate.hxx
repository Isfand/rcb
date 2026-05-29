#pragma once

#include "common/database.hxx"

namespace rcb{

struct ValidateOptions
{
	bool verboseOption{};
	bool yesOption{};
	bool fileOption{};
	bool dataOption{};
	bool wipeOption{};
	bool fillDirSizeOption{};
	bool dryRunOption{};
};

class Validate
{
public:
	Validate(const ValidateOptions& vOpt);

private:
	const ValidateOptions& m_vOpt;
	Database m_db;
	
	// Unused
	void allFile();

	// removes dangling files in file/ by checking data/.
	void file();
	// removes dangling & duplicate records in data/ by checking file/.
	void data();
	// removes dangling files in wipe/.
	void wipe();
	// evaluate size:NULL of filetype:directory
	void fillDirectorySize();
};

	
}// namespace rcb
