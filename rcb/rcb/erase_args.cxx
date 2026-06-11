#include <print>

#include "erase.hxx"
#include "erase_args.hxx"
#include "common/globals.hxx"
#include "common/env.hxx"

namespace rcb{

EraseArgs::EraseArgs(std::vector<std::string>& args)
{
	run(args);
}

void EraseArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(const auto& it : args)
	{
		std::println("{}", it);
	}
#endif

	if(args.empty())
	{
		std::print("{0} erase: missing operand\ntry \'{0} --help\' for more information", g_kProgName);
		return;
	}


	for(auto i { 0UZ }; i < args.size(); i++)
	{
	
		if(args[i].starts_with("--") ||  args[i].starts_with("-"))
		{
			if(args[i] == "--help" || args[i] == "-h")
			{
				std::print("{}", m_eraseHelpMsg);
				return;
			} 
			else if (args[i] == "--all" || args[i] == "-a")
			{
				m_eOpt.allOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--past")
			{
				if (i + 1UZ < args.size())
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
			else if (args[i] == "--last")
			{
				m_eOpt.lastOption = true;
				m_erasePositions.push_back(i);
			}
			else if (args[i] == "--verbose" || args[i] == "-v")
			{
				m_eOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			}
			else if(args[i] == "--dry-run")
			{
				m_eOpt.dryRunOption = true;
				m_erasePositions.push_back(i);
			} 
			else if (args[i] == "--sql")
			{
				if (i + 1UZ < args.size())
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

	Erase{args, m_eOpt, EnvResolver{}.resolve()};
}

} // namespace rcb
