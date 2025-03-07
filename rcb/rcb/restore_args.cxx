#include <print>

#include "restore_args.hxx"
#include "common/globals.hxx"
#include "restore.hxx"

namespace rcb{

RestoreArgs::RestoreArgs(std::vector<std::string>& args)
{
	run(args);
}

void RestoreArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(auto it : args)
	{
		std::println("{}", it);
	}
#endif

	if(args.size() <= 0)
	{
		std::println("{0} restore: missing operand\ntry \'{0} --help\' for more information", g_progName);
		return;
	}
	
	for(auto i { 0UL }; i < args.size(); i++)
	{
		if (std::string(args.at(i)).substr(0, 2) == "--" || std::string(args.at(i)).substr(0, 1) == "-")
		{
			if(std::string(args.at(i)) == "--help" || std::string(args.at(i)) == "-h")
			{
				std::print("{}", m_restoreHelpMsg);
					
				return;
			} 
			else if (std::string(args.at(i)) == "--all" || std::string(args.at(i)) == "-a")
			{
#ifndef NDEBUG
				std::println("restore all triggered");
#endif
				m_rOpt.allOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--verbose" || std::string(args.at(i)) == "-v")
			{
#ifndef NDEBUG
				std::println("restore verbose triggered");
#endif
				m_rOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--force" || std::string(args.at(i)) == "-f")
			{
#ifndef NDEBUG
				std::println("restore force triggered");
#endif
				m_rOpt.forceOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--force-replace")
			{
#ifndef NDEBUG
				std::println("restore force-replace triggered");
#endif
				m_rOpt.forceReplaceOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--force-rename")
			{
#ifndef NDEBUG
				std::println("restore force-rename triggered");
#endif
				m_rOpt.forceRenameOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--force-recreate-directory")
			{
#ifndef NDEBUG
				std::println("restore force-recreate-directory triggered");
#endif
				m_rOpt.forceRecreateDirectoryOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--silent" || std::string(args.at(i)) == "-s")
			{
#ifndef NDEBUG
				std::println("restore silent triggered");
#endif
				m_rOpt.silentOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--past")
			{
#ifndef NDEBUG
				std::println("restore present triggered");
#endif

				if (i + 1 < args.size())
				{
					m_rOpt.pastOption = true;
					m_rOpt.timeVec.push_back(args.at(i + 1));
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
				std::println("restore previous triggered");
#endif
				m_rOpt.previousOption = true;
				m_erasePositions.push_back(i);
			}
			else if (std::string(args.at(i)) == "--sql")
			{
#ifndef NDEBUG
				std::println("sql triggered");
#endif
				if (i + 1 < args.size())
				{
					m_rOpt.sqlOption = true;
					m_rOpt.sqlVec.push_back(args.at(i + 1));
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

	Restore{args, m_rOpt};
}
	
} // namespace rcb
