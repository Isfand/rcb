#include <print>

#include "restore.hxx"
#include "restore_args.hxx"
#include "common/globals.hxx"

namespace rcb{

RestoreArgs::RestoreArgs(std::vector<std::string>& args)
{
	run(args);
}

void RestoreArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(const auto& it : args)
	{
		std::println("{}", it);
	}
#endif

	if(args.empty())
	{
		std::println("{0} restore: missing operand\ntry \'{0} --help\' for more information", g_kProgName);
		return;
	}
	
	for(auto i { 0UZ }; i < args.size(); i++)
	{
		if(args[i].starts_with("--") ||  args[i].starts_with("-"))
		{
			if(args[i] == "--help" || args[i] == "-h")
			{
				std::print("{}", m_restoreHelpMsg);
					
				return;
			} 
			else if (args[i] == "--all" || args[i] == "-a")
			{
				m_rOpt.allOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--verbose" || args[i] == "-v")
			{
				m_rOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--force" || args[i] == "-f")
			{
				m_rOpt.forceOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--force-replace")
			{
				m_rOpt.forceReplaceOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--force-rename")
			{
				m_rOpt.forceRenameOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--force-recreate-directory")
			{
				m_rOpt.forceRecreateDirectoryOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--past")
			{
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
			else if (args[i] == "--last")
			{
				m_rOpt.lastOption = true;
				m_erasePositions.push_back(i);
			}
			else if(args[i] == "--dry-run")
			{
				m_rOpt.dryRunOption = true;
				m_erasePositions.push_back(i);
			} 
			else if (args[i] == "--sql")
			{
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
				std::println("unknown option: {0}", args[i]);
				return; // Prevent continuation
			}
		}
		else
		{
			// Unused
		}
	}

	// Needs to be removed in reverse to prevent index shift back.
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		args.erase(args.begin() + *it);
	}

	Restore{args, m_rOpt};
}
	
} // namespace rcb
