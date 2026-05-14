#include <vector>
#include <string>
#include <print>

#include "validate_args.hxx"
#include "common/globals.hxx"
#include "validate.hxx"

namespace rcb{

ValidateArgs::ValidateArgs(std::vector<std::string>& args)
{
	run(args);
}

void ValidateArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(auto it : args)
	{
		std::println("{}", it);
	}
#endif

	if(args.size() <= 0)
	{
		std::println("{0} validate: missing operand\ntry \'{0} validate --help\' for more information", g_kProgName);
		return;
	}

	for(auto i { 0UL }; i < args.size(); i++)
	{
		if (args.at(i).substr(0, 2) == "--" || args.at(i).substr(0, 1) == "-")
		{
			if(args.at(i) == "--help" || args.at(i) == "-h")
			{
				std::print("{}", m_validateHelpMsg);
				return;
			}
			else if (args.at(i) == "--all" || args.at(i) == "-a")
			{
				m_vOpt.fileOption = true;
				m_vOpt.dataOption = true;
				m_vOpt.wipeOption = true;
				m_vOpt.fillDirSizeOption = true;
			}
			else if (args.at(i) == "--verbose" || args.at(i) == "-v")
			{
				m_vOpt.verboseOption = true;
			}
			else if (args.at(i) == "--yes" || args.at(i) == "-y")
			{
				m_vOpt.yesOption = true;
			}
			else if (args.at(i) == "--file" || args.at(i) == "-F")
			{
				m_vOpt.fileOption = true;
			}
			else if (args.at(i) == "--data" || args.at(i) == "-D")
			{
				m_vOpt.dataOption = true;
			}
			else if (args.at(i) == "--wipe" || args.at(i) == "-W")
			{
				m_vOpt.wipeOption = true;
			}
			else if (args.at(i) == "--fill-directorysize")
			{
				m_vOpt.fillDirSizeOption = true;
			}
			else
			{
				std::println("unknown option: {0}", args.at(i));
				return; //Prevent continuation
			}
		}
		else
		{
			std::println("incorrect use of command\nuse --help for usage");
			return; //Prevent continuation
		}
	} 
	Validate{m_vOpt};
}
	
}//namespace rcb
