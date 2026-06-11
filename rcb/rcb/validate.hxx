#pragma once

#include "common/database.hxx"
#include "common/env.hxx"

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
	Validate(const ValidateOptions& vOpt, const Env& env);

private:
	const ValidateOptions& m_vOpt;
	const Env& m_env;
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
