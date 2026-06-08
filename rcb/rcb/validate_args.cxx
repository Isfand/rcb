#include <vector>
#include <string>
#include <print>

#include "validate_args.hxx"
#include "validate.hxx"
#include "common/globals.hxx"

namespace rcb{

ValidateArgs::ValidateArgs(std::vector<std::string>& args)
{
	run(args);
}

void ValidateArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(const auto& it : args)
	{
		std::println("{}", it);
	}
#endif

	if(args.empty())
	{
		std::println("{0} validate: missing operand\ntry \'{0} validate --help\' for more information", g_kProgName);
		return;
	}

	for(auto i { 0UZ }; i < args.size(); i++)
	{
		if (args[i].starts_with("--") || args[i].starts_with("-"))
		{
			if(args[i] == "--help" || args[i] == "-h")
			{
				std::print("{}", m_validateHelpMsg);
				return;
			}
			else if (args[i] == "--all" || args[i] == "-a")
			{
				m_vOpt.fileOption = true;
				m_vOpt.dataOption = true;
				m_vOpt.wipeOption = true;
				m_vOpt.fillDirSizeOption = true;
			}
			else if (args[i] == "--verbose" || args[i] == "-v")
			{
				m_vOpt.verboseOption = true;
			}
			else if (args[i] == "--yes" || args[i] == "-y")
			{
				m_vOpt.yesOption = true;
			}
			else if (args[i] == "--file" || args[i] == "-F")
			{
				m_vOpt.fileOption = true;
			}
			else if (args[i] == "--data" || args[i] == "-D")
			{
				m_vOpt.dataOption = true;
			}
			else if (args[i] == "--wipe" || args[i] == "-W")
			{
				m_vOpt.wipeOption = true;
			}
			else if (args[i] == "--fill-directorysize")
			{
				m_vOpt.fillDirSizeOption = true;
			}
			else if(args[i] == "--dry-run")
			{
				m_vOpt.dryRunOption = true;
			} 
			else
			{
				std::println("unknown option: {0}", args[i]);
				return; // Prevent continuation
			}
		}
		else
		{
			std::println("incorrect use of command\nuse --help for usage");
			return; // Prevent continuation
		}
	} 
	Validate{m_vOpt};
}
	
}// namespace rcb
