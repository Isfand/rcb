#include <string>
#include <vector>
#include <print>
#include <algorithm>

#include "args.hxx"
#include "delete_args.hxx"
#include "restore_args.hxx"
#include "erase_args.hxx"
#include "list_args.hxx"
#include "validate_args.hxx"
#include "common/env.hxx"
#include "common/database.hxx"
#include "common/globals.hxx"

namespace rcb{

Args::Args(int argc, char** argv) : m_argc{argc}, m_argv{argv}
{
	for(int i { 1 }; i < m_argc; i++) m_args.push_back(m_argv[i]);
}

void Args::runCmd()
{
	switch (m_cmd)
	{
		case Command::Delete: {
#ifndef NDEBUG
			std::println("Delete called in switch");
#endif
			DeleteArgs _{m_args};
			break;
		}
		case Command::Erase: {
#ifndef NDEBUG
			std::println("Erase called in switch");
#endif
			EraseArgs _{m_args};
			break;
		}
		case Command::List: {
#ifndef NDEBUG
			std::println("List called in switch");
#endif
			ListArgs _{m_args};
			break;
		}
		case Command::Restore: {
#ifndef NDEBUG
			std::println("Restore called in switch");
#endif
			RestoreArgs _{m_args};
			break;
		}
		case Command::Validate: {
#ifndef NDEBUG
			std::println("Validate called in switch");
#endif
			ValidateArgs _{m_args};
			break;
		}
		default: {
			std::println("missing command");
			break;
		}
	}
}

//Unused
void Args::deGlob()
{
	// Globals needs to removed so that primary commands don't see them after being parsed.
	// E.G: Remove -v and --verbose
	if (m_sharedGlobFlag)
	{
		//Used for tactac argument
		auto tactac = std::find(m_args.begin(), m_args.end(), "--");
		tactac = (tactac != m_args.end()) ? tactac : m_args.end();
		int tactac_index = std::distance(m_args.begin(), tactac);

#ifndef NDEBUG
		std::println("tactac is:{}", tactac_index);
#endif

		for(auto argTR : m_argsToRemove)
		{
			auto it = (std::find(m_args.begin(), tactac, argTR));
			if (it != m_args.end())  
			{ 
				int index = it - m_args.begin();
#ifndef NDEBUG
				std::println("{0} at index: {1}", argTR, index);
#endif
				// std::next is not needed
				m_args.erase(std::next(m_args.begin(), index));
			}
		}
	}
}

void Args::init()
{
	if(m_args.size() <= 0) // Prevent Segmentation fault
	{
		std::println("{0}: missing operand\ntry \'{0} --help\' for more information", g_progName);
		return;
	}

	for(auto arg : m_args)
	{
		if(std::string(arg) == "") // TEMPORARY. Implement in every command individually.
		{
			std::println("{0}: cannot accept empty arguments", g_progName);
			return;
		}

		if(std::string(arg) == "delete" || std::string(arg) == "d")
		{
#ifndef NDEBUG
			std::println("Delete flag triggered");
#endif
			if (m_sharedCmdFlag)
				continue;

			m_cmd = Command::Delete;
			m_sharedCmdFlag = true;

		}
		else if(std::string(arg) == "erase" || std::string(arg) == "e")
		{
#ifndef NDEBUG
			std::println("Erase flag triggered");
#endif
			if (m_sharedCmdFlag)
				continue;

			m_cmd = Command::Erase;
			m_sharedCmdFlag = true;


		}
		else if(std::string(arg) == "list" || std::string(arg) == "l")
		{
#ifndef NDEBUG
			std::println("List flag triggered");
#endif
			if (m_sharedCmdFlag)
				continue;

			m_cmd = Command::List;
			m_sharedCmdFlag = true;

		}
		else if(std::string(arg) == "restore" || std::string(arg) == "r")
		{
#ifndef NDEBUG
			std::println("Restore flag triggered");
#endif
			if (m_sharedCmdFlag)
				continue;

			m_cmd = Command::Restore;
			m_sharedCmdFlag = true;
					
		}
		else if(std::string(arg) == "validate" || std::string(arg) == "v")
		{
#ifndef NDEBUG
			std::println("Validate flag triggered");
#endif
			if (m_sharedCmdFlag)
				continue;

			m_cmd = Command::Validate;
			m_sharedCmdFlag = true;
					
		}
		else if(std::string(arg) == "--")
		{
			// if (m_tactacFlag)
			//     continue;

			// m_tactacFlag = true;
			break;
		}
		else if(std::string(arg) == "--help" || std::string(arg) == "-h")
		{
			if(m_sharedCmdFlag)
				continue;
					 
			std::println("{0}", m_helpMsg);
			return;
		}
		else if(std::string(arg) == "--verbose" || std::string(arg) == "-v")
		{
			// std::string(arg) == ("--verbose") ? m_argsToRemove.push_back("--verbose") : m_argsToRemove.push_back("-v");

			m_verboseOption = true;
			// m_sharedGlobFlag = m_verboseOption;
			//return;
		}
		else if(std::string(arg) == "--force" || std::string(arg) == "-f")
		{
			// std::string(arg) == ("--force") ? m_argsToRemove.push_back("--force") : m_argsToRemove.push_back("-f");

			m_forceOption = true;
			// m_sharedGlobFlag = m_forceOption;
			//return;
		}
		else if(std::string(arg) == "--version")
		{
			//https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
			std::println("\x1B[32m{} {} version ({})\033[0m", "♻", g_progName, g_progVersion);
			std::println("\x1B[90m{}\033[0m", m_rcb_ASCII_art);
			return;	
		}
		else
		{
			if(m_sharedCmdFlag)
				continue;

			std::println("Incorrect use of command. Enter --help for usage", g_progName);
			return;
		}
	}
	m_initFinishedFlag = true;

#ifndef NDEBUG
	std::println("Ending Args Init");
	for(auto it : m_args)
	{
		std::println("{0}", it);
	}
#endif
	
}

//Unused
void Args::setSharedOptions()
{
	//Legacy code do NOT use.
	//singleton->setVerboseOption(m_verboseOption);
	//singleton->setForceOption(m_forceOption);

	/*TODO: Set shared options that are not part of singleton.
	  That can be passed into each command's parser as argument strings. With exception to setup and cleanup code classes options can be passed in as a combined object instead.*/
	
	//Shared options can only exist IF they apply to EVERY command. E.G something like --jobs or --verbose.
}

void Args::run()
{
	/* SETUP */
	// Order of execution matters

	Args::init();
	// Args::setSharedOptions();
	Env _; // Get/Set environment variables
	Database().createTable(); // Create default table

	/* EXECUTION */
	if(m_initFinishedFlag)
	{
		// Args::deGlob();
		
		// Only erase if the args count isn't 0. As deGlob can clear all global args. Doing so causes a segfault
		// erase first element, which should only be a Primary Command at this stage.
		if(m_args.size() > 0) m_args.erase(m_args.begin());

		Args::runCmd();
	}
}

} // namespace rcb 

