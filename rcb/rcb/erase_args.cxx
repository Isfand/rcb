#include <print>

#include "erase_args.hxx"
#include "erase.hxx"
#include "common/globals.hxx"

namespace rcb{

EraseArgs::EraseArgs(std::vector<std::string>& args)
{
	run(args);
}

void EraseArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(auto it : args)
	{
		std::println("{}", it);
	}
#endif

	if(args.size() <= 0)
	{
		std::print("{0} erase: missing operand\ntry \'{0} --help\' for more information", g_kProgName);
		return;
	}


	for(auto i { 0UL }; i < args.size(); i++)
	{
	
		if (args.at(i).substr(0, 2) == "--" || args.at(i).substr(0, 1) == "-")
		{
			if(args.at(i) == "--help" || args.at(i) == "-h")
			{
				std::println("{}", m_eraseHelpMsg);
				return;
			} 
			else if (args.at(i) == "--all" || args.at(i) == "-a")
			{
				m_eOpt.allOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--past")
			{
				if (i + 1UL < args.size())
				{
					m_eOpt.pastOption = true;
					m_eOpt.timeVec.push_back(args.at(i + 1));
					m_erasePositions.push_back(i);
					m_erasePositions.push_back(i + 1);
				}
				else 
				{
					m_erasePositions.push_back(i);
					std::println("expected string value after index:{}", i);
					return;
				}
			}
			else if (args.at(i) == "--previous")
			{
				m_eOpt.previousOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--verbose" || args.at(i) == "-v")
			{
				m_eOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args.at(i) == "--sql")
			{
				if (i + 1UL < args.size())
				{
					m_eOpt.sqlOption = true;
					m_eOpt.sqlVec.push_back(args.at(i + 1));
					m_erasePositions.push_back(i);
					m_erasePositions.push_back(i + 1);
				}
				else 
				{
					m_erasePositions.push_back(i);
					std::println("expected string value after index:{}", i);
					return;
				}
			}
			else
			{
				std::println("unknown option: {0}", args.at(i));
				return; //Prevent continuation
			}
		}
		else
		{
			//Unused
		}
	}

	//Needs to be removed in reverse to prevent index shift back.
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		args.erase(args.begin() + *it);
	}

	Erase{args, m_eOpt};
}

} // namespace rcb
