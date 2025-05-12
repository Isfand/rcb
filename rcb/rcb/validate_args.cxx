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
		if (std::string(args.at(i)).substr(0, 2) == "--" || std::string(args.at(i)).substr(0, 1) == "-")
		{
			if(std::string(args.at(i)) == "--help" || std::string(args.at(i)) == "-h")
			{
				std::print("{}", m_validateHelpMsg);
				return;
			}
			else if (std::string(args.at(i)) == "--all" || std::string(args.at(i)) == "-a")
			{
#ifndef NDEBUG
				std::println("validate all triggered");
#endif
				m_vOpt.fileOption = true;
				m_vOpt.dataOption = true;
				m_vOpt.wipeOption = true;
				m_vOpt.fillDirSizeOption = true;
			}
			else if (std::string(args.at(i)) == "--verbose" || std::string(args.at(i)) == "-v")
			{
#ifndef NDEBUG
				std::println("validate verbose triggered");
#endif
				m_vOpt.verboseOption = true;
			}
			else if (std::string(args.at(i)) == "--yes" || std::string(args.at(i)) == "-y")
			{
#ifndef NDEBUG
				std::println("validate yes triggered");
#endif
				m_vOpt.yesOption = true;
			}
			else if (std::string(args.at(i)) == "--silent" || std::string(args.at(i)) == "-s")
			{
#ifndef NDEBUG
				std::println("validate silent triggered");
#endif
				m_vOpt.silentOption = true;
			}
			else if (std::string(args.at(i)) == "--file" || std::string(args.at(i)) == "-F")
			{
#ifndef NDEBUG
				std::println("validate file triggered");
#endif
				m_vOpt.fileOption = true;
			}
			else if (std::string(args.at(i)) == "--data" || std::string(args.at(i)) == "-D")
			{
#ifndef NDEBUG
				std::println("validate data triggered");
#endif
				m_vOpt.dataOption = true;
			}
			else if (std::string(args.at(i)) == "--wipe" || std::string(args.at(i)) == "-W")
			{
#ifndef NDEBUG
				std::println("validate wipe triggered");
#endif
				m_vOpt.wipeOption = true;
			}
			else if (std::string(args.at(i)) == "--fill-directorysize")
			{
#ifndef NDEBUG
				std::println("fill directory size triggered");
#endif
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
