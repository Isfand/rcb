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
	
		if (std::string(args.at(i)).substr(0, 2) == "--" || std::string(args.at(i)).substr(0, 1) == "-")
		{
			if(std::string(args.at(i)) == "--help" || std::string(args.at(i)) == "-h")
			{
				std::println("{}", m_eraseHelpMsg);
				return;
			} 
			else if (std::string(args.at(i)) == "--all" || std::string(args.at(i)) == "-a")
			{
#ifndef NDEBUG
				std::println("all triggered");
#endif
				m_eOpt.allOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--past")
			{
#ifndef NDEBUG
				std::println("present triggered");
#endif
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
			else if (std::string(args.at(i)) == "--previous")
			{
#ifndef NDEBUG
				std::println("previous triggered");
#endif
				m_eOpt.previousOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--verbose" || std::string(args.at(i)) == "-v")
			{
#ifndef NDEBUG
				std::println("verbose triggered");
#endif
				m_eOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--silent" || std::string(args.at(i)) == "-s")
			{
#ifndef NDEBUG
				std::println("silent triggered");
#endif
				m_eOpt.silentOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--sql")
			{
#ifndef NDEBUG
				std::println("sql triggered");
#endif
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
